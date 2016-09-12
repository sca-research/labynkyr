/*
 * University of Bristol – Open Access Software Licence
 * Copyright (c) 2016, The University of Bristol, a chartered
 * corporation having Royal Charter number RC000648 and a charity
 * (number X1121) and its place of administration being at Senate
 * House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Any use of the software for scientific publications or commercial
 * purposes should be reported to the University of Bristol
 * (OSI-notifications@bristol.ac.uk and quote reference 2514). This is
 * for impact and usage monitoring purposes only.
 *
 * Enquiries about further applications and development opportunities
 * are welcome. Please contact elisabeth.oswald@bristol.ac.uk
*/
/*
 * PEU.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_PEU_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_PEU_HPP_

#include "labynkyr/search/parallel/Queue.hpp"
#include "labynkyr/search/parallel/SearchTaskRunner.hpp"
#include "labynkyr/search/verify/KeyVerifier.hpp"

#include <stdint.h>

#include <mutex>
#include <stdexcept>
#include <thread>

namespace labynkyr {
namespace search {

/**
 *
 * A PEU operates in a single-thread managed by itself, and reads from a queue of SearchTasks to be executed sequentially on the thread.
 *
 * Each PEU is given a KeyVerifier instance that will be supplied to SearchTaskRunner instances.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType, typename SubkeyType>
class PEU {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	using QueueType = Queue<SearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>>;

	/**
	 *
	 * Functor object that should be passed to a std::thread constructor to begin running a PEU's main method in a thread.
	 */
	class PEUThreadRunner {
	public:
		PEUThreadRunner(PEU & peu)
		: peu(peu)
		{
		}

		void operator()() const {
			peu.run();
		}
	private:
		PEU<VecCount, VecLenBits, WeightType, SubkeyType> & peu;
	};

	/**
	 *
	 * @param uuid
	 * @param keyVerifier
	 * @param readQueue the PEU will take fresh SearchTaskRunners to execute from this queue
	 * @param writeQueue the PEU will place completed SearchTaskRunners on this queue
	 * @param sleepNanoseconds when not processing a SearchTaskRunner, the PEU will check the read queue every sleepNanoseconds
	 */
	PEU(uint32_t uuid, KeyVerifier<KeyLenBits> & keyVerifier, QueueType & readQueue, QueueType & writeQueue, uint64_t sleepNanoseconds)
	: uuid(uuid)
	, keyVerifier(keyVerifier)
	, readQueue(readQueue)
	, writeQueue(writeQueue)
	, sleepNanoseconds(sleepNanoseconds)
	, workerThread(0)
	, isStop(true)
	, exceptionThrown(false)
	{
	}

	virtual ~PEU() {}

	/**
	 *
	 * Startup the internal thread and begin listening on the read queue
	 */
	void processAsynchronously() {
		PEUThreadRunner runner(*this);
		isStop = false;
		workerThread = new std::thread(runner);
	}

	/**
	 *
	 * Tear-down the internal thread and stop listening on the read queue
	 */
	void stop() {
		std::unique_lock<std::mutex> lock(mutex);
		if(isStop != true) {
			// Then not already stopped
			isStop = true;
			lock.unlock();
			workerThread->join();
			delete workerThread;
		}
	}

	uint32_t getUUID() const {
		return uuid;
	}

	KeyVerifier<KeyLenBits> & getKeyVerifier() {
		return keyVerifier;
	}

	/**
	 *
	 * @return true if an exception was thrown during the processing of a SearchTaskRunner
	 */
	bool isExceptionThrown() {
		std::unique_lock<std::mutex> lock(mutex);
		return exceptionThrown;
	}

	/**
	 *
	 * @return a thrown std::exception_ptr
	 */
	std::exception_ptr const & getExceptionPtr() const {
		return exceptionPtr;
	}
private:
	uint32_t const uuid;
	KeyVerifier<KeyLenBits> & keyVerifier;
	QueueType & readQueue;
	QueueType & writeQueue;
	uint64_t const sleepNanoseconds;

	std::thread *workerThread;
	bool isStop;
	std::mutex mutex;

	bool exceptionThrown;
	std::exception_ptr exceptionPtr;

	friend class PEUThreadRunner;

	void run() {
		while(true) {
			std::unique_lock<std::mutex> lock(mutex);
			if(isStop) {
				lock.unlock();
				break;
			}
			lock.unlock();
			// Can now query the queue
			auto job = readQueue.nonBlockingTake();
			if(job.get() != 0) {
				// If we've got a SearchTaskRunnner to run, run it
				try {
					job->processSequentially(keyVerifier);
					writeQueue.put(std::move(job));
				} catch(std::exception const & ex) {
					// The task execution failed, set the exception_ptr for read elsewhere
					std::unique_lock<std::mutex> lock(mutex);
					exceptionThrown = true;
					exceptionPtr = std::current_exception();
					lock.unlock();
				}
			} else {
				std::this_thread::sleep_for(std::chrono::nanoseconds(sleepNanoseconds));
			}
		}
	}
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_PEU_HPP_ */

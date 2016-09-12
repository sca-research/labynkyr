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
 * WorkScheduler.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_WORKSCHEDULER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_WORKSCHEDULER_HPP_

#include "labynkyr/search/enumerate/ActiveNodeFinder.hpp"
#include "labynkyr/search/parallel/ANFForestSearchTaskRunner.hpp"
#include "labynkyr/search/parallel/EnvironmentManager.hpp"
#include "labynkyr/search/parallel/PEUPool.hpp"
#include "labynkyr/search/parallel/SortedSearchTaskRunner.hpp"
#include "labynkyr/search/EffortAllocation.hpp"

#include <stdint.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

namespace labynkyr {
namespace search {

/**
 *
 * WorkScheduler manages a pool of PEUs.  It will take a pre-defined EffortAllocation and manage the distribution of the associated SearchTasks
 * amongst a set of PEUs in parallel.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType, typename SubkeyType>
class WorkScheduler {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	/**
	 *
	 * @param sleepNanoseconds the scheduler will check internal queues for completed search tasks every sleepNanoseconds.
	 */
	WorkScheduler(uint64_t sleepNanoseconds)
	: sleepNanoseconds(sleepNanoseconds)
	, lastTimeTakenToFindKey(0UL)
	, lastTotalTimeTaken(0UL)
	{
	}

	virtual ~WorkScheduler() {}

	/**
	 *
	 * Run a parallel key search
	 *
	 * @param peuPool
	 * @param tasks
	 * @throws std::exception if any PEU encounters an exception, the WorkScheduler will catch the exception, signal the PEUs to stop, and then
	 * re-throw the exception once all PEUs have stopped
	 */
	void runSearch(PEUPool<VecCount, VecLenBits, WeightType, SubkeyType> & peuPool, EffortAllocation<VecCount, VecLenBits, WeightType> & tasks) {
		// Enqueue all tasks
		uint32_t const taskCount = tasks.tasksRemaining();
		WeightType const maxWeight = tasks.getWeightTable().maximumWeight();
		ActiveNodeFinder<VecCount, VecLenBits, WeightType> const activeNodeFinder(tasks.getWeightTable(), maxWeight);
		enqueueAllTasks(peuPool, tasks, activeNodeFinder);
		// Startup all the PEUs
		uint32_t tasksCompleted = 0;
		bool isKeyFound = false;
		auto const start = std::chrono::high_resolution_clock::now();
		peuPool.processAllPEUsAsynchronously();
		// Loop until all necessary search tasks are completed
		while(tasksCompleted < taskCount && isKeyFound == false) {
			// If a task has been completed, check whether a key was found, and break out of execution if so
			auto completedBatch = peuPool.getWriteQueue().nonBlockingTake();
			if(completedBatch != 0) {
				tasksCompleted++;
				EnvironmentManager::getInstance().logTaskCompletion<KeyLenBits>(
					completedBatch->size(),
					completedBatch->getDuration(),
					completedBatch->methodName()
				);
				isKeyFound = completedBatch->isKeyFound();
				if(isKeyFound) {
					auto const end = std::chrono::high_resolution_clock::now();
					lastTimeTakenToFindKey = std::chrono::duration<uint64_t, std::nano>(end - start);
					EnvironmentManager::getInstance().logKeyFound(peuPool.correctKey().asBytes(), lastTimeTakenToFindKey);
					break;
				}
			}
			// Check for exceptions -- will throw if found
			try {
				peuPool.checkForThrownExceptions();
			} catch(std::exception const & ex) {
				peuPool.stopAllPEUs(); // Stop the PEUs from processing
				throw ex;
			}
			// Now sleep until next tick
			std::this_thread::sleep_for(std::chrono::nanoseconds(sleepNanoseconds));
		}
		// Stop all PEUs
		peuPool.stopAllPEUs();
		auto const end = std::chrono::high_resolution_clock::now();
		lastTotalTimeTaken = std::chrono::duration<uint64_t, std::nano>(end - start);
	}

	/**
	 *
	 * @return the time taken to find the correct key (or zero if the key was not found or no parallel search has been executed yet)
	 */
	std::chrono::duration<uint64_t, std::nano> getLastTimeTakenToFindKey() const {
		return lastTimeTakenToFindKey;
	}

	/**
	 *
	 * @return the total time spent searching (or zero if no parallel search has been executed yet)
	 */
	std::chrono::duration<uint64_t, std::nano> getLastTotalTimeTaken() const {
		return lastTotalTimeTaken;
	}
private:
	uint64_t const sleepNanoseconds;
	std::chrono::duration<uint64_t, std::nano> lastTimeTakenToFindKey;
	std::chrono::duration<uint64_t, std::nano> lastTotalTimeTaken;

	/**
	 *
	 * The simplest way to allocate work is to take the set of SearchTasks described by the EffortAllocation and push them all onto a queue
	 * prior to execution
	 */
	void enqueueAllTasks(PEUPool<VecCount, VecLenBits, WeightType, SubkeyType> & peuPool, EffortAllocation<VecCount, VecLenBits, WeightType> & tasks, ActiveNodeFinder<VecCount, VecLenBits, WeightType> const & activeNodeFinder) {
		while(tasks.tasksRemaining() > 0) {
			auto const nextTaskDef = tasks.removeNextTask();
			if(nextTaskDef.second.isInitialTask()) {
				// If its the initial task (starts searching the most likely key), then we use the sorted enumeration method
				auto * runner = new SortedSearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>(nextTaskDef.second, nextTaskDef.first);
				std::unique_ptr<SortedSearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>> runnerPtr(runner);
				peuPool.addTasking(std::move(runnerPtr));
			} else {
				// If its not the initial task, then we use the ANF/Forest enumeration method
				auto * runner = new ANFForestSearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>(nextTaskDef.second, nextTaskDef.first, activeNodeFinder);
				std::unique_ptr<ANFForestSearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>> runnerPtr(runner);
				peuPool.addTasking(std::move(runnerPtr));
			}
		}
	}
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_WORKSCHEDULER_HPP_ */

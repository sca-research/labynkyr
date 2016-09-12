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
 * SearchTaskRunner.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_SEARCHTASKRUNNER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_SEARCHTASKRUNNER_HPP_

#include "labynkyr/search/verify/KeyVerifier.hpp"
#include "labynkyr/search/SearchTask.hpp"
#include "labynkyr/BigInt.hpp"

#include <stdint.h>

#include <chrono>
#include <string>

namespace labynkyr {
namespace search {

/**
 *
 * SearchTaskRunner is an interface for defining sequential enumeration tasks that may be run in parallel.
 *
 * An instance of a SearchTaskRunner will be placed on a queue and made available to PEUs.  A PEU will take the instance from the queue,
 * and call the processSequentially(KeyVerifier &) function with the PEUs internal KeyVerifier.  Once this call is complete, the
 * PEU will place the instance onto an output queue.
 *
 * This approach decouples the PEU from needing to know how to execute the enumeration task.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType, typename SubkeyType>
class SearchTaskRunner {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	/**
	 *
	 * @param task the search task to execute
	 * @param expectedTaskSize the number of candidate keys the task will enumerate and verify
	 */
	SearchTaskRunner(SearchTask<VecCount, VecLenBits, WeightType> const & task, BigInt<KeyLenBits> expectedTaskSize)
	: task(task)
	, expectedTaskSize(expectedTaskSize)
	, keyFound(false)
	, duration(0)
	{
	}

	virtual ~SearchTaskRunner() {}

	/**
	 *
	 * Implementing classes must execute all the work necessary to enumerate and verify the keys specified by the internal
	 * SearchTask in this function call.
	 *
	 * @param keyVerifier
	 */
	virtual void processSequentially(KeyVerifier<KeyLenBits> & keyVerifier) = 0;

	SearchTask<VecCount, VecLenBits, WeightType> const & getTask() const {
		return task;
	}

	/**
	 *
	 * @return the number of candidate keys the task will enumerate and verify
	 */
	BigInt<KeyLenBits> size() const {
		return expectedTaskSize;
	}

	/**
	 *
	 * @return whether the correct key was found during the execution of this task
	 */
	bool isKeyFound() const {
		return keyFound;
	}

	/**
	 *
	 * @return the time spent during the execution of this task
	 */
	std::chrono::duration<uint64_t, std::nano> const & getDuration() const {
		return duration;
	}

	/**
	 *
	 * @return a human-readable name for the task
	 */
	virtual std::string methodName() const = 0;
protected:
	SearchTask<VecCount, VecLenBits, WeightType> const task;
	BigInt<KeyLenBits> const expectedTaskSize;
	bool keyFound;
	std::chrono::duration<uint64_t, std::nano> duration;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_SEARCHTASKRUNNER_HPP_ */

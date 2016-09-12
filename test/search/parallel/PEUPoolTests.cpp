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
 * PEUPoolTests.cpp
 *
 */

#include "src/labynkyr/search/parallel/PEUPool.hpp"

#include "src/labynkyr/search/parallel/ANFForestSearchTaskRunner.hpp"
#include "src/labynkyr/search/parallel/Queue.hpp"
#include "src/labynkyr/search/verify/ListKeyVerifier.hpp"
#include "test/search/parallel/ExceptionThrowingSearchTaskRunner.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <memory>
#include <stdexcept>
#include <thread>

namespace labynkyr {
namespace search {

TEST(PEUPool_constructor) {
	ListKeyVerifierFactory<4> verifierFactory;
	uint32_t const peuCount = 6;
	uint32_t const verifierCount = 3;
	PEUPool<2, 2, uint32_t, uint32_t> pool(peuCount, verifierFactory, verifierCount, 200UL);

	CHECK_EQUAL(verifierCount, pool.getVerifiers().size());
	CHECK_EQUAL(peuCount, pool.getPEUs().size());
}

TEST(PEUPool_processThenStop) {
	ListKeyVerifierFactory<6> verifierFactory;
	uint32_t const peuCount = 6;
	uint32_t const verifierCount = 3;
	PEUPool<3, 2, uint32_t, uint32_t> pool(peuCount, verifierFactory, verifierCount, 200UL);

	pool.processAllPEUsAsynchronously();

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0, 1, 1, 0, 1};
	WeightTable<3, 2, uint32_t> const weightTable(weights);
	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 5;
	SearchTask<3, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<3, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());

	auto * runnerPtr = new ANFForestSearchTaskRunner<3, 2, uint32_t, uint32_t>(task, 53, activeNodeFinder);
	std::unique_ptr<ANFForestSearchTaskRunner<3, 2, uint32_t, uint32_t>> runner(runnerPtr);
	pool.addTasking(std::move(runner));

	std::unique_ptr<SearchTaskRunner<3, 2, uint32_t, uint32_t>> produce;
	uint32_t attempts = 0;
	while(true) {
		produce = pool.getWriteQueue().nonBlockingTake();
		attempts++;
		if(produce != 0 || attempts == 5) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	CHECK(attempts != 5);
	uint64_t const verifierKeysChecked = pool.keysVerified();
	CHECK_EQUAL(53, verifierKeysChecked);
	pool.stopAllPEUs();
}

TEST(PEUPool_exceptionHandling) {
	ListKeyVerifierFactory<6> verifierFactory;
	uint32_t const peuCount = 6;
	uint32_t const verifierCount = 3;
	PEUPool<3, 2, uint32_t, uint32_t> pool(peuCount, verifierFactory, verifierCount, 200UL);

	pool.processAllPEUsAsynchronously();

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0, 1, 1, 0, 1};
	WeightTable<3, 2, uint32_t> const weightTable(weights);
	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 5;
	SearchTask<3, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);

	pool.checkForThrownExceptions(); // Shouldn't throw

	auto * runnerPtr = new ExceptionThrowingSearchTaskRunner<3, 2, uint32_t, uint32_t>(task);
	std::unique_ptr<ExceptionThrowingSearchTaskRunner<3, 2, uint32_t, uint32_t>> runner(runnerPtr);
	pool.addTasking(std::move(runner));

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK_THROW(pool.checkForThrownExceptions(), std::logic_error);

	pool.stopAllPEUs();
}

} /* namespace search */
} /* namespace labynkyr */



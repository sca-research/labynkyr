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
 * ANFForestSearchTaskRunnerTests.cpp
 *
 */

#include "src/labynkyr/search/parallel/ANFForestSearchTaskRunner.hpp"

#include "src/labynkyr/search/enumerate/ActiveNodeFinder.hpp"
#include "src/labynkyr/search/verify/ComparisonKeyVerifier.hpp"
#include "src/labynkyr/search/verify/ListKeyVerifier.hpp"
#include "src/labynkyr/search/SearchTask.hpp"

#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <vector>

namespace labynkyr {
namespace search {

TEST(ANFForestSearchTaskRunner_searchWithANFForest_size0) {
	ListKeyVerifier<6> verifier;

	std::vector<uint32_t> const weights = {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4};
	WeightTable<3, 2, uint32_t> const weightTable(weights);

	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 3;
	SearchTask<3, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<3, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());

	ANFForestSearchTaskRunner<3, 2, uint32_t, uint32_t> runner(task, 0, activeNodeFinder);
	runner.processSequentially(verifier);
	CHECK_EQUAL(0, verifier.keysChecked());
}

TEST(ANFForestSearchTaskRunner_searchWithANFForest_size15_3vectors) {
	ListKeyVerifier<6> verifier;

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0, 1, 1, 0, 1};
	WeightTable<3, 2, uint32_t> const weightTable(weights);

	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 5;
	SearchTask<3, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<3, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());

	ANFForestSearchTaskRunner<3, 2, uint32_t, uint32_t> runner(task, 53, activeNodeFinder);
	runner.processSequentially(verifier);
	CHECK_EQUAL(53, verifier.keysChecked());
}

TEST(ANFForestSearchTaskRunner_searchWithANFForest_size15_fail) {
	std::vector<uint8_t> const targetKey = {0x0A}; // Any key other than 0x0A should verify
	ComparisonKeyVerifier<4> verifier(targetKey);

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 6;
	SearchTask<2, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<2, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());

	ANFForestSearchTaskRunner<2, 2, uint32_t, uint32_t> runner(task, 15, activeNodeFinder);
	runner.processSequentially(verifier);
	CHECK(!verifier.success());
}

TEST(ANFForestSearchTaskRunner_searchWithANFForest_size15_success) {
	std::vector<uint8_t> const targetKey = {0x06}; // Any key other than 0x0A should verify
	ComparisonKeyVerifier<4> verifier(targetKey);

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 6;
	SearchTask<2, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<2, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());

	ANFForestSearchTaskRunner<2, 2, uint32_t, uint32_t> runner(task, 15, activeNodeFinder);
	runner.processSequentially(verifier);
	CHECK(verifier.success());
}

TEST(ANFForestSearchTaskRunner_searchWithANFForest_size15) {
	ListKeyVerifier<4> verifier;

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 6;
	SearchTask<2, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<2, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());

	ANFForestSearchTaskRunner<2, 2, uint32_t, uint32_t> runner(task, 15, activeNodeFinder);
	runner.processSequentially(verifier);
	CHECK_EQUAL(15, verifier.keysChecked());
}

TEST(ANFForestSearchTaskRunner_searchWithANFForest_size4) {
	ListKeyVerifier<4> verifier;

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	uint32_t const minKeyWeight = 0;
	uint32_t const maxKeyWeight = 1;
	SearchTask<2, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<2, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());;

	ANFForestSearchTaskRunner<2, 2, uint32_t, uint32_t> runner(task, 4, activeNodeFinder);
	runner.processSequentially(verifier);
	CHECK_EQUAL(4, verifier.keysChecked());
}

TEST(ANFForestSearchTaskRunner_searchWithANFForest_size11) {
	ListKeyVerifier<4> verifier;

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	uint32_t const minKeyWeight = 1;
	uint32_t const maxKeyWeight = 6;
	SearchTask<2, 2, uint32_t> const task(minKeyWeight, maxKeyWeight, weightTable);
	ActiveNodeFinder<2, 2, uint32_t> const activeNodeFinder(weightTable, weightTable.maximumWeight());

	ANFForestSearchTaskRunner<2, 2, uint32_t, uint32_t> runner(task, 11, activeNodeFinder);
	runner.processSequentially(verifier);
	CHECK_EQUAL(11, verifier.keysChecked());
}

} /* namespace search */
} /* namespace labynkyr */

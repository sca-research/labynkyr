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
 * WorkSchedulerTests.cpp
 *
 */

#include "src/labynkyr/search/parallel/WorkScheduler.hpp"

#include "src/labynkyr/search/parallel/PEUPool.hpp"
#include "src/labynkyr/search/verify/ComparisonKeyVerifier.hpp"
#include "src/labynkyr/search/verify/ListKeyVerifier.hpp"
#include "src/labynkyr/search/EffortAllocation.hpp"
#include "src/labynkyr/search/SearchSpec.hpp"
#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <stdexcept>
#include <vector>

namespace labynkyr {
namespace search {

TEST(WorkScheduler_size15_fail) {
	std::vector<uint8_t> const targetKey = {0x0A}; // Any key other than 0x0A should verify
	ComparisonKeyVerifierFactory<4> verifierFactory(targetKey);
	uint32_t const peuCount = 3;
	uint32_t const verifierCount = 3;
	PEUPool<2, 2, uint32_t, uint32_t> pool(peuCount, verifierFactory, verifierCount, 200UL);

	BigInt<4> const budget(15);
	SearchSpecBuilder<4> const searchSpecBuilder(budget);
	auto const searchSpec = searchSpecBuilder.createSpec();

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	EffortAllocation<2, 2, uint32_t> effort(searchSpec, weightTable, 0); // One key batch size (2^0)

	WorkScheduler<2, 2, uint32_t, uint32_t> scheduler(100UL);
	scheduler.runSearch(pool, effort);
	CHECK(!pool.isKeyFound());
	CHECK_THROW(pool.correctKey(), std::logic_error);
}

TEST(WorkScheduler_size15_success) {
	std::vector<uint8_t> const targetKey = {0x06}; // Any key other than 0x0A should verify
	ComparisonKeyVerifierFactory<4> verifierFactory(targetKey);
	uint32_t const peuCount = 3;
	uint32_t const verifierCount = 3;
	PEUPool<2, 2, uint32_t, uint32_t> pool(peuCount, verifierFactory, verifierCount, 200UL);

	BigInt<4> const budget(15);
	SearchSpecBuilder<4> const searchSpecBuilder(budget);
	auto const searchSpec = searchSpecBuilder.createSpec();

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	EffortAllocation<2, 2, uint32_t> effort(searchSpec, weightTable, 0); // One key batch size (2^0)

	WorkScheduler<2, 2, uint32_t, uint32_t> scheduler(100UL);
	scheduler.runSearch(pool, effort);
	CHECK(pool.isKeyFound());
	auto const foundKey = pool.correctKey();
	CHECK_ARRAY_EQUAL(targetKey, foundKey.asBytes(), targetKey.size());
}

} /* namespace search */
} /* namespace labynkyr */



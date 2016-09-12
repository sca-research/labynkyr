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
 * PathCountRankTests.cpp
 *
 */

#include "src/labynkyr/rank/PathCountRank.hpp"

#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/Key.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <vector>

namespace labynkyr {
namespace rank {

TEST(PathCountRank_simpleExample_rankUsingKey_uint8_rank14) {
	// 0b0110
	Key<4> const key("06");
	std::vector<uint8_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint8_t> const weightTable(weights);

	BigInt<4> const rank = PathCountRank<2, 2, uint8_t>::rank(key, weightTable);
	BigInt<4> const expectedRank(14);
	CHECK_EQUAL(expectedRank, rank);
}

TEST(PathCountRank_simpleExample_rankUsingKey_uint32_rank0) {
	// 0b0110
	Key<4> const key("06");
	std::vector<uint32_t> const weights = {11, 15, 3, 6, 7, 2, 6, 19};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const rank = PathCountRank<2, 2, uint32_t>::rank(key, weightTable);
	BigInt<4> const expectedRank(0);
	CHECK_EQUAL(expectedRank, rank);
}

TEST(PathCountRank_simpleExample_rankUsingWeight_uint8_rank14) {
	// 0b0110
	Key<4> const key("06");
	std::vector<uint8_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint8_t> const weightTable(weights);

	uint32_t const keyWeight = weightTable.weightForKey(key);
	BigInt<4> const rank = PathCountRank<2, 2, uint8_t>::rank(keyWeight, weightTable);
	BigInt<4> const expectedRank(14);
	CHECK_EQUAL(expectedRank, rank);
}

TEST(PathCountRank_simpleExample_rankUsingWeight_uint32_rank0) {
	// 0b0110
	Key<4> const key("06");
	std::vector<uint32_t> const weights = {11, 15, 3, 6, 7, 2, 6, 19};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	uint32_t const keyWeight = weightTable.weightForKey(key);
	BigInt<4> const rank = PathCountRank<2, 2, uint32_t>::rank(keyWeight, weightTable);
	BigInt<4> const expectedRank(0);
	CHECK_EQUAL(expectedRank, rank);
}

TEST(PathCountRank_simpleExample_rankAllWeights_uint32_rank15) {
	// 0b1010
	Key<4> const key("0A");
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	auto const ranks = PathCountRank<2, 2, uint32_t>::rankAllWeights(6, weightTable);
	std::vector<BigInt<4>> expected(6);
	for(uint32_t weight = 6 ; weight > 0 ; weight--) {
		expected[6 - weight] = PathCountRank<2, 2, uint32_t>::rank(weight, weightTable);
	}
	CHECK_ARRAY_EQUAL(expected, ranks, expected.size());
}

TEST(PathCountRank_simpleExample_rankAllWeights_uint8_rank15) {
	// 0b1010
	Key<4> const key("0A");
	std::vector<uint8_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint8_t> const weightTable(weights);

	auto const ranks = PathCountRank<2, 2, uint8_t>::rankAllWeights(6, weightTable);
	std::vector<BigInt<4>> expected(6);
	for(uint32_t weight = 6 ; weight > 0 ; weight--) {
		expected[6 - weight] = PathCountRank<2, 2, uint8_t>::rank(weight, weightTable);
	}
	CHECK_ARRAY_EQUAL(expected, ranks, expected.size());
}

} /* namespace rank */
} /* namespace labynkyr */

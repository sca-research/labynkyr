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
 * WeightTableTests.cpp
 *
 */

#include "src/labynkyr/WeightTable.hpp"

#include "src/labynkyr/Key.hpp"

#include <unittest++/UnitTest++.h>

#include <stdexcept>
#include <vector>

namespace labynkyr {

TEST(WeightTable_weight_uint32_t) {
	std::vector<uint32_t> const weights = {3, 4, 6, 7, 0, 1, 3, 4};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	CHECK_EQUAL(3, weightTable.weight(0, 0));
	CHECK_EQUAL(4, weightTable.weight(0, 1));
	CHECK_EQUAL(6, weightTable.weight(0, 2));
	CHECK_EQUAL(7, weightTable.weight(0, 3));
	CHECK_EQUAL(0, weightTable.weight(1, 0));
	CHECK_EQUAL(1, weightTable.weight(1, 1));
	CHECK_EQUAL(3, weightTable.weight(1, 2));
	CHECK_EQUAL(4, weightTable.weight(1, 3));
}

TEST(WeightTable_weight_uint8_t) {
	std::vector<uint8_t> const weights = {3, 4, 6, 7, 0, 1, 3, 4};
	WeightTable<2, 2, uint8_t> const weightTable(weights);
	CHECK_EQUAL(3, weightTable.weight(0, 0));
	CHECK_EQUAL(4, weightTable.weight(0, 1));
	CHECK_EQUAL(6, weightTable.weight(0, 2));
	CHECK_EQUAL(7, weightTable.weight(0, 3));
	CHECK_EQUAL(0, weightTable.weight(1, 0));
	CHECK_EQUAL(1, weightTable.weight(1, 1));
	CHECK_EQUAL(3, weightTable.weight(1, 2));
	CHECK_EQUAL(4, weightTable.weight(1, 3));
}

TEST(WeightTable_allWeights_uint32_t) {
	std::vector<uint32_t> const weights = {3, 4, 6, 7, 0, 1, 3, 4};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	CHECK_ARRAY_EQUAL(weights, weightTable.allWeights(), weights.size());
}

TEST(WeightTable_weightForKey_6bits_uint32_t) {
	std::vector<uint32_t> const weights = {4, 3, 1, 1, 	6, 4, 3, 1,  5, 7, 8, 9};
	WeightTable<3, 2, uint32_t> const weightTable(weights);
	Key<6> const secretKey("09");
	uint32_t const expected = 3 + 3 + 5;
	CHECK_EQUAL(expected, weightTable.weightForKey(secretKey));
}

TEST(WeightTable_weightForKey_6bits_uint8_t) {
	std::vector<uint8_t> const weights = {4, 3, 1, 1, 	6, 4, 3, 1,  5, 7, 8, 9};
	WeightTable<3, 2, uint8_t> const weightTable(weights);
	Key<6> const secretKey("09");
	uint8_t const expected = 3 + 3 + 5;
	CHECK_EQUAL(expected, weightTable.weightForKey(secretKey));
}

TEST(WeightTable_weightForKey_36bits_uint32_t) {
	uint32_t const vectorSize = 1U << 12U;
	std::vector<uint8_t> weights(3 * vectorSize);
	weights[0 * vectorSize + 513] = 3;
	weights[1 * vectorSize + 48] = 3;
	weights[2 * vectorSize + 772] = 5;
	WeightTable<3, 12, uint8_t> const weightTable(weights);
	Key<36> const secretKey("0102030403");
	uint32_t const expected = 3 + 3 + 5;
	CHECK_EQUAL(expected, weightTable.weightForKey(secretKey));
}

TEST(WeightTable_rebase_minus_0) {
	std::vector<uint8_t> const weights = {9, 3, 4, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.rebase(0);

	std::vector<uint8_t> const expected = {8, 2, 3, 0, 	5, 3, 2, 0, 	4, 6, 3, 0};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

TEST(WeightTable_rebase_minus_0_v2) {
	std::vector<uint8_t> const weights = {10, 4, 5, 2, 	7, 5, 4, 2, 	6, 8, 5, 2};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.rebase(0);

	std::vector<uint8_t> const expected = {8, 2, 3, 0, 	5, 3, 2, 0, 	4, 6, 3, 0};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

TEST(WeightTable_rebase_minus_1) {
	std::vector<uint8_t> const weights = {9, 3, 4, 2, 	6, 4, 3, 2, 	5, 7, 4, 2};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.rebase(1);

	std::vector<uint8_t> const expected = {8, 2, 3, 1, 	5, 3, 2, 1, 	4, 6, 3, 1};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

TEST(WeightTable_rebase_minus_1_v2) {
	std::vector<uint8_t> const weights = {10, 4, 5, 3, 	7, 5, 4, 3, 	6, 8, 5, 3};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.rebase(1);

	std::vector<uint8_t> const expected = {8, 2, 3, 1, 	5, 3, 2, 1, 	4, 6, 3, 1};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

TEST(WeightTable_rebase_addition_1) {
	std::vector<uint8_t> const weights = {9, 3, 4, 0, 	6, 4, 3, 0, 	5, 7, 4, 0};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.rebase(1);

	std::vector<uint8_t> const expected = {10, 4, 5, 1, 	7, 5, 4, 1, 	6, 8, 5, 1};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

TEST(WeightTable_rebase_addition_3) {
	std::vector<uint8_t> const weights = {10, 4, 5, 1, 	7, 5, 4, 1, 	6, 8, 5, 1};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.rebase(3);

	std::vector<uint8_t> const expected = {12, 6, 7, 3, 	9, 7, 6, 3, 	8, 10, 7, 3};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

TEST(WeightTable_minimumWeight_uint8_t) {
	std::vector<uint8_t> const weights = {4, 3, 1, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint8_t> const weightTable(weights);
	CHECK_EQUAL(3, weightTable.minimumWeight());
}

TEST(WeightTable_minimumWeight_zeros_uint8_t) {
	std::vector<uint8_t> const weights = {4, 3, 1, 0, 	6, 4, 3, 0, 	5, 7, 4, 1};
	WeightTable<3, 2, uint8_t> const weightTable(weights);
	CHECK_EQUAL(1, weightTable.minimumWeight());
}

TEST(WeightTable_maximumWeight_uint8_t) {
	std::vector<uint8_t> const weights = {4, 3, 1, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint8_t> const weightTable(weights);
	CHECK_EQUAL(17, weightTable.maximumWeight());
}

TEST(WeightTable_maximumWeight_zeros_uint8_t) {
	std::vector<uint8_t> const weights = {0, 0, 0, 0, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint8_t> const weightTable(weights);
	CHECK_EQUAL(13, weightTable.maximumWeight());
}

TEST(WeightTable_sortAscendingAndTrackIndexes_6bit_uint32_t) {
	std::vector<uint32_t> const weights = {0, 3, 4, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint32_t> weightTable(weights);
	std::vector<uint8_t> indexes(3 * 4);
	weightTable.sortAscendingAndTrackIndexes(indexes);

	// Expected weights
	std::vector<uint32_t> const expected = {0, 1, 3, 4,    1, 3, 4, 6,   1, 4, 5, 7};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
	// Expected indexes
	std::vector<uint8_t> const expectedIndexes = {0, 3, 1, 2,    3, 2, 1, 0,   3, 2, 0, 1};
	CHECK_ARRAY_EQUAL(expectedIndexes, indexes, expectedIndexes.size());
}

TEST(WeightTable_sortAscendingAndTrackIndexes_indexesTooSmall) {
	std::vector<uint32_t> const weights = {0, 3, 4, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint32_t> weightTable(weights);
	std::vector<uint8_t> indexes(3 * 4 - 1);
	CHECK_THROW(weightTable.sortAscendingAndTrackIndexes(indexes), std::length_error);
}

TEST(WeightTable_sortAscendingAndTrackIndexes_indexesTooBig) {
	std::vector<uint32_t> const weights = {0, 3, 4, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint32_t> weightTable(weights);
	std::vector<uint8_t> indexes(3 * 4 + 1);
	CHECK_THROW(weightTable.sortAscendingAndTrackIndexes(indexes), std::length_error);
}

TEST(WeightTable_sortAscending_6bit_uint8_t) {
	std::vector<uint8_t> const weights = {0, 3, 4, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.sortAscending();

	std::vector<uint8_t> const expected = {0, 1, 3, 4, 	1, 3, 4, 6, 	1, 4, 5, 7};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

TEST(WeightTable_sortDescending_6bit_uint8_t) {
	std::vector<uint8_t> const weights = {0, 3, 4, 1, 	6, 4, 3, 1, 	5, 7, 4, 1};
	WeightTable<3, 2, uint8_t> weightTable(weights);
	weightTable.sortDescending();

	std::vector<uint8_t> const expected = {4, 3, 1, 0, 	6, 4, 3, 1, 	7, 5, 4, 1};
	CHECK_ARRAY_EQUAL(expected, weightTable.allWeights(), expected.size());
}

} /* namespace labynkyr */



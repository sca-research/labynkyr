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
 * WeightFinderTests.cpp
 *
 */

#include "src/labynkyr/search/enumerate/WeightFinder.hpp"

#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <vector>

namespace labynkyr {
namespace search {

TEST(WeightFinder_twoVector_findBestWeight_overflow) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	BigInt<4> const budget(15);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(6, actual.first);
	BigInt<4> const expectedAllocated(15);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_twoVector_findBestWeight_last2) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	BigInt<4> const budget(1);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(1, actual.first);
	BigInt<4> const expectedAllocated(4);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_twoVector_findBestWeight_last) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	BigInt<4> const budget(4);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(1, actual.first);
	BigInt<4> const expectedAllocated(4);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_twoVector_findBestWeight_first) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	BigInt<4> const budget(15);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(6, actual.first);
	BigInt<4> const expectedAllocated(15);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_twoVector_findBestWeight_middleSplit) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	BigInt<4> const budget(7);
	auto const actual = weightFinder.findBestWeight(budget);
	if(actual.first == 3) {
		BigInt<4> const expectedAllocated(8);
		CHECK_EQUAL(expectedAllocated, actual.second);
	} else if(actual.first == 2) {
		BigInt<4> const expectedAllocated(6);
		CHECK_EQUAL(expectedAllocated, actual.second);
	} else {
		CHECK(false);
	}

}

TEST(WeightFinder_twoVector_findBestWeight_inexact) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	BigInt<4> const budget(9);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(3, actual.first);
	BigInt<4> const expectedAllocated(8);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_twoVector_findBestWeight_exactMatch) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	BigInt<4> const budget(13);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(4, actual.first);
	BigInt<4> const expectedAllocated(13);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_twoVector_list) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	WeightFinder<2, 2, uint32_t> weightFinder(weights);

	auto const & list = weightFinder.list();
	std::vector<BigInt<4>> const expected = {15, 14, 13, 8, 6, 4};
	CHECK_ARRAY_EQUAL(expected, list, expected.size());
}

TEST(WeightFinder_listPreConstructed_twoVector_findBestWeight_last2) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	std::vector<BigInt<4>> const rankList = {15, 14, 13, 8, 6, 4};
	WeightFinder<2, 2, uint32_t> weightFinder(weights, rankList);

	BigInt<4> const budget(1);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(1, actual.first);
	BigInt<4> const expectedAllocated(4);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_listPreConstructed_twoVector_findBestWeight_last) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	std::vector<BigInt<4>> const rankList = {15, 14, 13, 8, 6, 4};
	WeightFinder<2, 2, uint32_t> weightFinder(weights, rankList);

	BigInt<4> const budget(4);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(1, actual.first);
	BigInt<4> const expectedAllocated(4);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_listPreConstructed_twoVector_findBestWeight_first) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	std::vector<BigInt<4>> const rankList = {15, 14, 13, 8, 6, 4};
	WeightFinder<2, 2, uint32_t> weightFinder(weights, rankList);

	BigInt<4> const budget(15);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(6, actual.first);
	BigInt<4> const expectedAllocated(15);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_listPreConstructed_twoVector_findBestWeight_middleSplit) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	std::vector<BigInt<4>> const rankList = {15, 14, 13, 8, 6, 4};
	WeightFinder<2, 2, uint32_t> weightFinder(weights, rankList);

	BigInt<4> const budget(7);
	auto const actual = weightFinder.findBestWeight(budget);
	if(actual.first == 3) {
		BigInt<4> const expectedAllocated(8);
		CHECK_EQUAL(expectedAllocated, actual.second);
	} else if(actual.first == 2) {
		BigInt<4> const expectedAllocated(6);
		CHECK_EQUAL(expectedAllocated, actual.second);
	} else {
		CHECK(false);
	}

}

TEST(WeightFinder_listPreConstructed_twoVector_findBestWeight_inexact) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	std::vector<BigInt<4>> const rankList = {15, 14, 13, 8, 6, 4};
	WeightFinder<2, 2, uint32_t> weightFinder(weights, rankList);

	BigInt<4> const budget(9);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(3, actual.first);
	BigInt<4> const expectedAllocated(8);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_listPreConstructed_twoVector_findBestWeight_exactMatch) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	std::vector<BigInt<4>> const rankList = {15, 14, 13, 8, 6, 4};
	WeightFinder<2, 2, uint32_t> weightFinder(weights, rankList);

	BigInt<4> const budget(13);
	auto const actual = weightFinder.findBestWeight(budget);
	CHECK_EQUAL(4, actual.first);
	BigInt<4> const expectedAllocated(13);
	CHECK_EQUAL(expectedAllocated, actual.second);
}

TEST(WeightFinder_listPreConstructed_twoVector_list) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	std::vector<BigInt<4>> const rankList = {15, 14, 13, 8, 6, 4};
	WeightFinder<2, 2, uint32_t> weightFinder(weights, rankList);
	auto const & list = weightFinder.list();
	CHECK_ARRAY_EQUAL(rankList, list, rankList.size());
}

} /* namespace search */
} /* namespace labynkyr */


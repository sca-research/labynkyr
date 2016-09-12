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
 * ActiveNodeFinderTests.cpp
 *
 */

#include "src/labynkyr/search/enumerate/ActiveNodeFinder.hpp"

#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/Key.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <memory>
#include <set>
#include <vector>

namespace labynkyr {
namespace search {

TEST(ActiveNodeFinder_zeroWeightActive_twoCol) {
	// 0b0110
	Key<4> const key("06");

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	ActiveNodeFinder<2, 2, uint32_t> finder(weightTable, weightTable.weightForKey(key));

	auto & set1 = finder.nextWeightIndexes(1);
	CHECK_EQUAL(3, set1.size());
	CHECK_EQUAL(1, set1.count(0));
	CHECK_EQUAL(1, set1.count(1));
	CHECK_EQUAL(1, set1.count(3));
	auto & set2 = finder.nextWeightIndexes(0);
	CHECK_EQUAL(1, set2.size());
	CHECK_EQUAL(1, set2.count(0));
}

TEST(ActiveNodeFinder_zeroWeightActive_threeCol) {
	// 0b0110
	Key<6> const key("16");

	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0, 1, 0, 1, 1};
	WeightTable<3, 2, uint32_t> const weightTable(weights);

	ActiveNodeFinder<3, 2, uint32_t> finder(weightTable, weightTable.weightForKey(key));

	auto & set1 = finder.nextWeightIndexes(2);
	CHECK_EQUAL(5, set1.size());
	CHECK_EQUAL(1, set1.count(0));
	CHECK_EQUAL(1, set1.count(1));
	CHECK_EQUAL(1, set1.count(2));
	CHECK_EQUAL(1, set1.count(3));
	CHECK_EQUAL(1, set1.count(4));
	auto & set2 = finder.nextWeightIndexes(1);
	CHECK_EQUAL(3, set2.size());
	CHECK_EQUAL(1, set2.count(0));
	CHECK_EQUAL(1, set2.count(1));
	CHECK_EQUAL(1, set2.count(3));
	auto & set3 = finder.nextWeightIndexes(0);
	CHECK_EQUAL(1, set3.size());
	CHECK_EQUAL(1, set3.count(0));
}

} /* namespace search */
} /* namespace labynkyr */

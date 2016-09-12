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
 * PathCountGraphTests.cpp
 *
 */

#include "src/labynkyr/rank/PathCountGraph.hpp"

#include "src/labynkyr/rank/GraphCoordinate.hpp"
#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/Key.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <vector>

namespace labynkyr {
namespace rank {

TEST(PathCountGraph_workedExample1) {
	// 0b0110
	Key<4> const key("06");
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	PathCountGraph<2, 2, uint32_t> graph(weightTable.weightForKey(key));

	// Run the rank algorithm manually
	uint32_t const keyWeight = weightTable.weightForKey(key);

	for(uint32_t vectorIndex = 2 ; vectorIndex > 0 ; vectorIndex--) {
		for(uint64_t weightIndex = keyWeight ; weightIndex > 0 ; weightIndex--) {
			for(uint64_t subkeyIndex = 4 ; subkeyIndex > 0 ; subkeyIndex--) {
				GraphCoordinate const coord(vectorIndex - 1, subkeyIndex - 1, weightIndex - 1);
				GraphCoordinate const rightChildIndex = graph.rightChildIndex(coord, weightTable);
				BigInt<4> const newValue = graph.rightChild(rightChildIndex);
				graph.set(coord, newValue);
			}
		}
		if(vectorIndex > 1) {
			graph.rotateBuffers();
		}
	}

	CHECK_EQUAL(14, graph.first());
}

TEST(PathCountGraph_workedExample2) {
	// 0b0110
	Key<4> const key("06");

	std::vector<uint32_t> const weights = {11, 15, 3, 6, 7, 2, 6, 19};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	PathCountGraph<2, 2, uint32_t> graph(weightTable.weightForKey(key));

	// Run the rank algorithm manually
	uint32_t const keyWeight = weightTable.weightForKey(key);

	for(uint32_t vectorIndex = 2 ; vectorIndex > 0 ; vectorIndex--) {
		for(uint64_t weightIndex = keyWeight ; weightIndex > 0 ; weightIndex--) {
			for(uint64_t subkeyIndex = 4 ; subkeyIndex > 0 ; subkeyIndex--) {
				GraphCoordinate const coord(vectorIndex - 1, subkeyIndex - 1, weightIndex - 1);

				GraphCoordinate const rightChildIndex = graph.rightChildIndex(coord, weightTable);

				BigInt<4> const newValue = graph.rightChild(rightChildIndex);
				graph.set(coord, newValue);
			}
		}
		if(vectorIndex > 1) {
			graph.rotateBuffers();
		}
	}
	CHECK_EQUAL(0, graph.first());
}

} /* namespace rank */
} /* namespace labynkyr */



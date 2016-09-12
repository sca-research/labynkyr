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
 * ActiveNodeFinder.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_ACTIVENODEFINDER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_ACTIVENODEFINDER_HPP_

#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace labynkyr {
namespace search {

/**
 *
 * ActiveNodeFinder traverses the path count graph associated with a WeightTable, and identifies the weights that are required to be
 * visited by an enumeration algorithm.  The class stores these weights in a list of sets.  An enumeration algorithm can take these
 * sets and save computation by merging and updating only the nodes stored in each set.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to score weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class ActiveNodeFinder {
public:
	enum {
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * Traverses the graph structure to uncover the required weight/column indexes.  Assumes only the weight 0 is active in the
	 * first distinguishing vector. Weight 0 is the only weight required in the first distinguishing for enumeration and rank to be possible
	 *
	 * @param integerScores
	 * @param maxWeight
	 */
	ActiveNodeFinder(WeightTable<VecCount, VecLenBits, WeightType> const & weightTable, WeightType const maxWeight)
	{
		// Zero-th vector just has first column
		std::set<uint64_t> * firstSet = new std::set<uint64_t>();
		firstSet->insert(0);
		validIndexes.push_back(std::unique_ptr<std::set<uint64_t>>(firstSet));
		// Rest of the vectors
		for(uint64_t vectorIndex = 1 ; vectorIndex < VecCount ; vectorIndex++) {
			std::vector<bool> bitset(maxWeight);
			std::set<uint64_t> * indexes = new std::set<uint64_t>();

			uint64_t const previousVectorIndex = vectorIndex - 1;
			std::set<uint64_t> const & previousVectorCols = *validIndexes[previousVectorIndex].get();

			for(auto relativeColIndex : previousVectorCols) {
				for(uint64_t rowIndex = VectorSize ; rowIndex > 0 ; rowIndex--) {
					WeightType const weight = weightTable.weight(previousVectorIndex, rowIndex - 1);
					if(relativeColIndex + weight < maxWeight && !bitset[relativeColIndex + weight]) {
						indexes->insert(relativeColIndex + weight);
						bitset[relativeColIndex + weight] = true;
					}
				}
			}
			validIndexes.push_back(std::unique_ptr<std::set<uint64_t>>(indexes));
		}
	}

	~ActiveNodeFinder() {}

	/**
	 *
	 * @param vectorIndex
	 * @return the set of weights that are 'active' for the specified distinguishing vector.
	 * @throws std::length_error
	 */
	std::set<uint64_t> const & nextWeightIndexes(uint64_t vectorIndex) const {
		if(vectorIndex >= validIndexes.size()) {
			throw std::length_error("Invalid vector index");
		}
		return *validIndexes[vectorIndex].get();
	}
private:
	std::vector<std::unique_ptr<std::set<uint64_t>>> validIndexes;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_ACTIVENODEFINDER_HPP_ */

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
 * PathCountSearch.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PATHCOUNTSEARCH_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PATHCOUNTSEARCH_HPP_

#include "labynkyr/rank/GraphCoordinate.hpp"
#include "labynkyr/search/enumerate/ActiveNodeFinder.hpp"
#include "labynkyr/search/enumerate/PathCountEnumerationGraph.hpp"
#include "labynkyr/search/enumerate/SortedEnumeration.hpp"
#include "labynkyr/search/verify/FullKeyBuilder.hpp"
#include "labynkyr/search/verify/KeyVerifier.hpp"
#include "labynkyr/search/SearchTask.hpp"
#include "labynkyr/BigInt.hpp"
#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <set>
#include <stdexcept>

namespace labynkyr {
namespace search {

/**
 *
 * Contains various methods for enumerating and verifying candidate keys.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType, typename SubkeyType>
class PathCountSearch {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits,
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * @param keyVerifier the verifier used to check whether a key candidate is correct
	 */
	PathCountSearch(KeyVerifier<KeyLenBits> & keyVerifier)
	: keyVerifier(keyVerifier)
	{
	}

	~PathCountSearch() {}

	/**
	 *
	 * Enumerate keys using the ActiveNodeFinder/Forest algorithm as described in:
	 *
	 * How low can you go? Using side-channel data to enhance brute-force key recovery
	 * Jake Longo and Daniel P. Martin and Luke Mather and Elisabeth Oswald and Benjamin Sach and Martijn Stam
	 * http://eprint.iacr.org/2016/609
	 *
	 * @param task
	 * @param activeNodeFinder the set of active nodes in the path count graph
	 */
	void searchWithANFForest(SearchTask<VecCount, VecLenBits, WeightType> const & task, ActiveNodeFinder<VecCount, VecLenBits, WeightType> const & activeNodeFinder) {
		auto const & weightTable = task.getWeightTable();

		PathCountEnumerationGraph<VecCount, VecLenBits, WeightType, SubkeyType> graph(task);

		WeightType breakWeight = task.getMaxKeyWeight();
		for(uint32_t vectorIndex = VecCount ; vectorIndex > 1 ; vectorIndex--) {
			WeightType minScore = task.getMaxKeyWeight();
			for(uint32_t subkeyIndex = 0 ; subkeyIndex <  VectorSize ; subkeyIndex++) {
				WeightType const score = weightTable.weight(vectorIndex - 1, subkeyIndex);
				minScore = (score < minScore) ? score : minScore;
			}

			auto const & colIndexes = activeNodeFinder.nextWeightIndexes(vectorIndex - 1);
			for(auto weightIndex : colIndexes) {
				if(weightIndex > breakWeight) {
					break;
				}
				for(uint64_t subkeyIndex = VectorSize ; subkeyIndex > 0 ; subkeyIndex--) {
					rank::GraphCoordinate const coord(vectorIndex - 1, subkeyIndex - 1, weightIndex);
					rank::GraphCoordinate const rightChildIndex = graph.rightChildIndex(coord);

					if(rightChildIndex.isReject() == false) {
						auto const & rightSet = graph.rightChild(rightChildIndex);
						auto & setAtCoord = graph.at(coord);
						setAtCoord.merge(rightSet, subkeyIndex - 1);
					}
				}
			}
			breakWeight -= minScore;
			graph.rotateBuffers();
		}
		// Can skip all but nodes with weight 0 in the last vector
		for(uint64_t subkeyIndex = VectorSize ; subkeyIndex > 0 ; subkeyIndex--) {
			if(keyVerifier.success()) break;
			rank::GraphCoordinate const coord(0, subkeyIndex - 1, 0);
			rank::GraphCoordinate const rightChildIndex = graph.rightChildIndex(coord);

			if(rightChildIndex.isReject() == false) {
				auto const & rightSet = graph.rightChild(rightChildIndex);
				auto & setAtCoord = graph.at(coord);
				setAtCoord.verifyMergeCandidates(keyVerifier, rightSet, subkeyIndex - 1);
			}
		}
	}

	/**
	 *
	 * Enumerate keys using the Sorted algorithm as described in:
	 *
	 * How low can you go? Using side-channel data to enhance brute-force key recovery
	 * Jake Longo and Daniel P. Martin and Luke Mather and Elisabeth Oswald and Benjamin Sach and Martijn Stam
	 * http://eprint.iacr.org/2016/609
	 *
	 * @param weightTable an integer representation of the distinguishing scores.  This object will be sorted, so copy
	 * before use if it is to be re-used in original form elsewhere.
	 */
	void searchWithSorted(WeightType maxKeyWeight, WeightTable<VecCount, VecLenBits, WeightType> & weightTable) {
		SortedEnumeration<VecCount, VecLenBits, WeightType, SubkeyType> enumerator(keyVerifier, weightTable);
		enumerator.enumerate(maxKeyWeight);
	}
private:
	KeyVerifier<KeyLenBits> & keyVerifier;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PATHCOUNTSEARCH_HPP_ */

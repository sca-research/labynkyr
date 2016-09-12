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
 * PathCountRank.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_RANK_PATH_COUNT_PATHCOUNTRANK_HPP_
#define LABYNKYR_SRC_LABYNKYR_RANK_PATH_COUNT_PATHCOUNTRANK_HPP_

#include "labynkyr/rank/PathCountGraph.hpp"

#include "labynkyr/BigInt.hpp"
#include "labynkyr/Key.hpp"
#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <stdexcept>

namespace labynkyr {
namespace rank {

/**
 *
 * Implementation of the path-counting rank estimation algorithm of Martin et al.:
 *
 * 		How to Enumerate Your Keys Accurately and Efficiently After a Side Channel Attack.
 * 		Daniel P. Martin, Jonathan F. O'Connell, Elisabeth Oswald, Martijn Stam
 * 		ASIACRYPT (2) 2015: 313-337 (2015)
 *
 * with improvements from:
 *
 *		Characterisation and Estimation of the Key Rank Distribution in the Context of Side Channel Evaluations.
 * 		Daniel P. Martin, Luke Mather, Elisabeth Oswald, Martijn Stam
 *		IACR Cryptology ePrint Archive 2016: 491 (2016)
 *
 * The integer representation of the distinguishing scores should ideally be positive integers.  Technically, as long as the
 * sum of the integer scores for the correct key is positive, the algorithm is correct, but for safety it is likely easier
 * to ensure all values are >= 1.
 *
 * IMPORTANT - definitions of 'rank':
 * 		- The rank is defined as the number of keys with a better distinguishing score.  Thus, a perfect attack is rank 0.
 * 		- In the case of ties, the number of ties do not count towards the rank.  E.g. if three keys, including the true one, have the
 * 		  same (best) distinguishing score, then the rank of the correct key is still 0, and is not 3.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class PathCountRank {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits,
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * Estimates the rank of a key.
	 *
	 * Both the fastest and smallest memory footprint implementation.
	 *
	 * Optimises by only checking the nodes corresponding to weight = 0 in the final distinguishing vector
	 *
	 * @param key the known key
	 * @param weightTable an integer representation of the distinguishing scores
	 * @return the estimated rank of the key
	 * @throws std::invalid_argument
	 */
	static BigInt<KeyLenBits> rank(Key<KeyLenBits> const & key, WeightTable<VecCount, VecLenBits, WeightType> const & weightTable) {
		WeightType const keyWeight = weightTable.weightForKey(key);
		if(keyWeight == static_cast<WeightType>(0)) {
			throw std::invalid_argument("The weight for the known key must be > 0.");
		}

		return rank(keyWeight, weightTable);
	}

	/**
	 *
	 * Estimates the rank of a *weight*, rather than a key.  Finding the rank of a key is equivalent to finding the rank of its
	 * weight. Counts all keys with a weight strictly smaller than the provided weight.
	 *
	 * Both the fastest and smallest memory footprint implementation.
	 *
	 * Optimises by only checking the nodes corresponding to weight = 0 in the final distinguishing vector
	 *
	 * @param maxWeight the weight to be ranked up to.  In standard 'rank' problems, this value should be equivalent to the weight
	 * of the known key.  In enumeration problems, this may be any bound.
	 * @param weightTable an integer representation of the distinguishing scores
	 * @return the estimated rank of the weight
	 * @throws std::invalid_argument
	 */
	static BigInt<KeyLenBits> rank(WeightType maxWeight, WeightTable<VecCount, VecLenBits, WeightType> const & weightTable) {
		if(maxWeight == static_cast<WeightType>(0)) {
			throw std::invalid_argument("The weight rank at must be > 0.");
		}

		PathCountGraph<VecCount, VecLenBits, WeightType> graph(maxWeight);

		for(uint32_t vectorIndex = VecCount ; vectorIndex > 1 ; vectorIndex--) {
			for(uint64_t subkeyIndex = VectorSize ; subkeyIndex > 0 ; subkeyIndex--) {
				for(uint64_t weightIndex = maxWeight ; weightIndex > 0 ; weightIndex--) {
					GraphCoordinate const coord(vectorIndex - 1, subkeyIndex - 1, weightIndex - 1);
					GraphCoordinate const rightChildIndex = graph.rightChildIndex(coord, weightTable);
					graph.set(coord, graph.rightChild(rightChildIndex));
				}
			}
			graph.rotateBuffers();
		}
		// Can skip all but nodes with weight 0 in the last vector
		for(uint64_t subkeyIndex = VectorSize ; subkeyIndex > 0 ; subkeyIndex--) {
			GraphCoordinate const coord(0, subkeyIndex - 1, 0);
			GraphCoordinate const rightChildIndex = graph.rightChildIndex(coord, weightTable);
			graph.set(coord, graph.rightChild(rightChildIndex));
		}
		return graph.first();
	}

	/**
	 *
	 * Computes a list of the rank of keys associated with the weights {maxWeight - 1,....,1}
	 *
	 * Runs in approximately the same time as a single run. Considerably more efficient than running the above algorithm maxWeight times
	 *
	 * @param maxWeight the weight to be ranked up to.  In standard 'rank' problems, this value should be equivalent to the weight
	 * of the known key.  In enumeration problems, this may be any bound.
	 * @param weightTable an integer representation of the distinguishing scores
	 * @return a list of the rank of the weights {maxWeight - 1,....,1}
	 * @throws std::invalid_argument
	 */
	static std::vector<BigInt<KeyLenBits>> rankAllWeights(WeightType maxWeight, WeightTable<VecCount, VecLenBits, WeightType> const & weightTable) {
		if(maxWeight == static_cast<WeightType>(0)) {
			throw std::invalid_argument("The maximum weight ranked up to must > 0.");
		}
		PathCountGraph<VecCount, VecLenBits, WeightType> graph(maxWeight);
		for(uint32_t vectorIndex = VecCount ; vectorIndex > 0 ; vectorIndex--) {
			for(uint64_t subkeyIndex = VectorSize ; subkeyIndex > 0 ; subkeyIndex--) {
				for(uint64_t weightIndex = maxWeight ; weightIndex > 0 ; weightIndex--) {
					GraphCoordinate const coord(vectorIndex - 1, subkeyIndex - 1, weightIndex - 1);
					GraphCoordinate const rightChildIndex = graph.rightChildIndex(coord, weightTable);
					graph.set(coord, graph.rightChild(rightChildIndex));
				}
			}
			graph.rotateBuffers();
		}
		return graph.previousRow();
	}
};

} /*namespace rank */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_RANK_PATHCOUNTRANK_HPP_ */

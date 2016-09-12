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
 * WeightFinder.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_WEIGHTFINDER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_WEIGHTFINDER_HPP_

#include "labynkyr/rank/PathCountRank.hpp"
#include "labynkyr/BigInt.hpp"
#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <cmath>
#include <utility>
#include <vector>

namespace labynkyr {
namespace search {

/**
 *
 * WeightFinder provides a method to find the closest weight for a specified rank.  This is needed to pick weight boundaries for enumeration
 * budgets -- for example, if I wish to enumerate the first 2^50 keys, then I need to know the weight Wmax, and will then check all keys
 * with 1 <= weight < Wmax.
 *
 * This can be achieved using a rank estimation algorithm taking the maximum weight in the table as input.  This will produce a list containing
 * the number of keys with each unique weight.  By binary searching across this list, we can pick the best weight for a required rank.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to score weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class WeightFinder {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	/**
	 *
	 * Construct from a WeightTable.  This constructor will run the path count rank estimation algorithm to estimate the number of key
	 * candidates associated with each unique weight in the table.  On high precision tables, this operation may take a significant
	 * amount of time.
	 *
	 * @param weightTable
	 */
	WeightFinder(WeightTable<VecCount, VecLenBits, WeightType> const & weightTable)
	: weightTable(weightTable)
	, maxWeight(weightTable.maximumWeight())
	, rankList(rank::PathCountRank<VecCount, VecLenBits, WeightType>::rankAllWeights(maxWeight, weightTable))
	{
	}

	/**
	 *
	 * Construct using a pre-constructed list of ranks for each possible key weight
	 *
	 * @param weightTable
	 * @param rankList
	 */
	WeightFinder(WeightTable<VecCount, VecLenBits, WeightType> const & weightTable, std::vector<BigInt<KeyLenBits>> const & rankList)
	: weightTable(weightTable)
	, maxWeight(weightTable.maximumWeight())
	, rankList(rankList)
	{
	}

	~WeightFinder() {}

	/**
	 *
	 * @param depth the index of the deepest/least-likely key required to be enumerated
	 * @return a pair containing the weight and the actual number of keys that will be enumerated given that weight
	 */
	std::pair<WeightType, BigInt<KeyLenBits>> findBestWeight(BigInt<KeyLenBits> depth) const {
		uint64_t left = 0;
		uint64_t right = rankList.size() - 1;

		BigInt<KeyLenBits> bestMargin = std::numeric_limits<BigInt<KeyLenBits>>::max();
		uint64_t bestIndex = 0;
		while(left < right) {
			uint64_t const middleIndex = std::floor(static_cast<double>(left + right) / 2.0);
			BigInt<KeyLenBits> const middle = rankList[middleIndex];
			BigInt<KeyLenBits> const margin = (middle > depth) ? middle - depth : depth - middle;
			if(margin < bestMargin) {
				bestMargin = margin;
				bestIndex = middleIndex;
			}
			if(middle < depth) {
				right = middleIndex - 1;
			} else if(middle > depth) {
				left = middleIndex + 1;
			} else {
				return std::make_pair(rankList.size() - middleIndex, middle);
			}
		}
		BigInt<KeyLenBits> const margin = (rankList[left] > depth) ? rankList[left] - depth : depth - rankList[left];
		if(margin < bestMargin) {
			return std::make_pair(rankList.size() - left, rankList[left]);
		} else {
			return std::make_pair(rankList.size() - bestIndex, rankList[bestIndex]);
		}
	}

	/**
	 *
	 * @return the list of the rank estimates for each unique weight in the weight table.
	 */
	std::vector<BigInt<KeyLenBits>> const & list() const {
		return rankList;
	}
private:
	WeightTable<VecCount, VecLenBits, WeightType> const & weightTable;
	WeightType const maxWeight;
	std::vector<BigInt<KeyLenBits>> const rankList;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_WEIGHTFINDER_HPP_ */

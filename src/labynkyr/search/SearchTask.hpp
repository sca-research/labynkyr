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
 * SearchTask.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHTASK_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHTASK_HPP_

#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

namespace labynkyr {
namespace search {

/**
 *
 * A SearchTask is defined as a open interval over the integer weights [minKeyWeight, maxKeyWeight).
 *
 * Labynkyr will sequentially enumerate and verify the key candidates defined by the weight range.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class SearchTask {
public:
	/**
	 *
	 * @param minKeyWeight the lowest weight at which keys will be enumerated
	 * @param maxKeyWeight the weight up to (but not inclusive) that keys will be enumerated
	 * @param weightTable an integer representation of the distinguishing scores
	 */
	SearchTask(WeightType minKeyWeight, WeightType maxKeyWeight, WeightTable<VecCount, VecLenBits, WeightType> const & weightTable)
	: minKeyWeight(minKeyWeight)
	, maxKeyWeight(maxKeyWeight)
	, weightTable(weightTable)
	{
	}

	SearchTask(SearchTask const & other)
	: minKeyWeight(other.getMinKeyWeight())
	, maxKeyWeight(other.getMaxKeyWeight())
	, weightTable(other.getWeightTable())
	{
	}

	~SearchTask() {}

	/**
	 *
	 * @return the lowest weight at which keys will be enumerated
	 */
	WeightType getMinKeyWeight() const {
		return minKeyWeight;
	}

	/**
	 *
	 * @return keys with weights up to (but not inclusive) of this value will be enumerated
	 */
	WeightType getMaxKeyWeight() const {
		return maxKeyWeight;
	}

	/**
	 *
	 * @return an integer representation of the distinguishing scores
	 */
	WeightTable<VecCount, VecLenBits, WeightType> const & getWeightTable() const {
		return weightTable;
	}

	/**
	 *
	 * @return true if this SearchTask starts with the enumeration and verification of the most likely key candidate, and
	 * false otherwise. This is needed to help with algorithm selection -- the initial task can use the faster Sorted
	 * algorithm.
	 */
	bool isInitialTask() const {
		return (minKeyWeight == weightTable.minimumWeight() || minKeyWeight == 0);
	}
private:
	WeightType const minKeyWeight;
	WeightType const maxKeyWeight;
	WeightTable<VecCount, VecLenBits, WeightType> const & weightTable;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHTASK_HPP_ */

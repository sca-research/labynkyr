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
 * SortedEnumeration.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_SORTEDENUMERATION_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_SORTEDENUMERATION_HPP_

#include "labynkyr/search/verify/FullKeyBuilder.hpp"
#include "labynkyr/search/verify/KeyVerifier.hpp"
#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <stdexcept>
#include <vector>

namespace labynkyr {
namespace search {

/**
 *
 * This class represents an instance of the Sorted enumeration algorithm as described in:
 *
 * How low can you go? Using side-channel data to enhance brute-force key recovery
 * Jake Longo and Daniel P. Martin and Luke Mather and Elisabeth Oswald and Benjamin Sach and Martijn Stam
 * http://eprint.iacr.org/2016/609
 *
 * The Sorted algorithm requires the weight table to be sorted in ascending order.  You need to copy the table before it does this
 * if you wish to re-use the original form of the table elsewhere.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, int32_t VecLenBits, typename WeightType, typename SubkeyType>
class SortedEnumeration {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits,
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * @param keyVerifier
	 * @param weightTable
	 */
	SortedEnumeration(KeyVerifier<KeyLenBits> & keyVerifier, WeightTable<VecCount, VecLenBits, WeightType> & weightTable)
	: keyVerifier(keyVerifier)
	, weightTable(weightTable)
	, indexes(VecCount * VectorSize)
	, partialSums(VecCount)
	, keyValue(VecCount)
	{
		// Sort weights and track indexes
		weightTable.template sortAscendingAndTrackIndexes<SubkeyType>(indexes);
		// Partial sums
		for(uint64_t vectorIndex = VecCount - 1 ; vectorIndex > 0 ; vectorIndex--) {
			uint64_t const relVectorIndex = vectorIndex - 1;
			partialSums[relVectorIndex] = weightTable.weight(relVectorIndex + 1, 0);
			partialSums[relVectorIndex] += partialSums[relVectorIndex + 1];
		}
		// Create key value and key byte objects to store working keys
		uint32_t const byteCount = (KeyLenBits % 8 != 0) ? (KeyLenBits / 8) + 1 : KeyLenBits / 8;
		keyBytes.resize(byteCount);
	}

	~SortedEnumeration() {}

	void enumerate(WeightType maxKeyWeight) {
		recurse(0, 0, maxKeyWeight);
	}
private:
	KeyVerifier<KeyLenBits> & keyVerifier;
	WeightTable<VecCount, VecLenBits, WeightType> & weightTable;
	std::vector<SubkeyType> indexes;
	std::vector<WeightType> partialSums;
	std::vector<SubkeyType> keyValue;
	std::vector<uint8_t> keyBytes;

	void recurse(uint32_t vectorIndex, WeightType weight, WeightType maxKeyWeight) {
		for(SubkeyType subkeyIndex = 0 ; subkeyIndex < VectorSize ; subkeyIndex++) {
			keyValue[vectorIndex] = indexes[vectorIndex * VectorSize + subkeyIndex];
			WeightType const contrib = weightTable.weight(vectorIndex, subkeyIndex);
			if(weight + contrib + partialSums[vectorIndex] >= maxKeyWeight || keyVerifier.success()) {
				break;
			} else if(vectorIndex == VecCount - 1) {
				FullKeyBuilder<VecCount, VecLenBits, SubkeyType>::fullKey(keyValue, keyBytes);
				keyVerifier.checkKey(keyBytes);
			} else {
				recurse(vectorIndex + 1, weight + contrib, maxKeyWeight);
			}
		}
	}
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_SORTEDENUMERATION_HPP_ */

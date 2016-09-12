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
 * WeightTable.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_WEIGHTTABLE_HPP_
#define LABYNKYR_SRC_LABYNKYR_WEIGHTTABLE_HPP_

#include "labynkyr/Key.hpp"

#include <stdint.h>

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace labynkyr {

/**
 *
 * A WeightTable contains the positive integer representation of the distinguishing scores associated with all the DPA attacks with the recovery
 * of a full key.  In this implementation we assume each attack targets the same size portion of the key (e.g. 16 8-bit SubBytes attack
 * on AES, and not 12 8-bit SubBytes and one 32-bit MixColumns attack)
 *
 * A WeightTable can be created using the DistinguishingTable#mapToWeight(uint32_t precisionBits) method.
 *
 * The weights are stored internally in a single vector, with the weights for the first distinguishing vector stored first in the buffer,
 * the second distinguishing vector in the next portion of the buffer, and so on.
 *
 * This class is templated on the integer type itself; if less than 32-bits or 16-bits of precision are required by the
 * ranking algorithm, then it makes sense to use uint16_t or uint32_t here.
 *
 * Higher distinguishing scores should have lower integer weights! The scores which correspond to more likely key candidates should
 * be mapped to smaller integer weights
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store the weights (e.g. uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class WeightTable {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits,
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * @param weights construct a weights table by passing in a vector of all the weights
	 * @throws std::length_error
	 */
	WeightTable(std::vector<WeightType> weights)
	: weights(weights)
	{
		if(weights.size() != VectorSize * VecCount) {
			std::stringstream error;
			error << "Attack result consists of " << VecCount << "distinguishing vectors each of size ";
			error << VecLenBits << " bits. The weight table must contain " << (VectorSize * VecCount) << " elements, ";
			error << "but provided table contains " << weights.size() << " elements";
			throw std::length_error(error.str().c_str());
		}
	}

	// Copy ctor
	WeightTable(WeightTable<VecCount, VecLenBits, WeightType> const & other)
	: weights(other.allWeights())
	{
	}

	~WeightTable() {}

	/**
	 *
	 * @param vectorIndex
	 * @param subkeyIndex
	 * @return the integer weight associated with the subkeyIndex subkey in the vectorIndex distinguishing vector
	 */
	WeightType weight(uint32_t vectorIndex, uint64_t subkeyIndex) const {
		return weights[vectorIndex * VectorSize + subkeyIndex];
	}

	/**
	 *
	 * @param key a key candidate
	 * @return the integer weight associated with this key candidate (the sum of the weights for each subkey)
	 */
	WeightType weightForKey(Key<KeyLenBits> const & key) const {
		WeightType sum = 0;
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			uint32_t const bitOffset = vectorIndex * VecLenBits;
			uint64_t const keyPortionValue = key.subkeyValue(BitWindow(bitOffset, VecLenBits));
			sum += weight(vectorIndex, keyPortionValue);
		}
		return sum;
	}

	/**
	 *
	 * The run-time of rank and enumeration algorithms is typically some function of the weight of the key.  Often, mapToWeight
	 * will produce weights where the minimum is > 1.  There is a considerable efficiency gain from translating the weights
	 * such that the minimum weight is 1.
	 *
	 * @param newMinimumWeight the minimum weight for any subkey will be set to be this value, by shifting the weights.
	 * The newMinimumWeight must be >= 1, scores of 0 or below are not allowed.
	 */
	void rebase(WeightType newMinimumWeight) {
		WeightType const minValue = *std::min_element(weights.begin(), weights.end());
		if(minValue >= newMinimumWeight) {
			WeightType const shiftAmount = minValue - newMinimumWeight;
			std::transform(
					weights.begin(), weights.end(), weights.begin(),
				[&shiftAmount](WeightType const weight) {
					return weight - shiftAmount;
				}
			);
		} else {
			WeightType const shiftAmount = newMinimumWeight - minValue;
			std::transform(
					weights.begin(), weights.end(), weights.begin(),
				[&shiftAmount](WeightType const weight) {
					return weight + shiftAmount;
				}
			);
		}
	}

	/**
	 *
	 * Sorts the elements (per vector) in ascending order and keeps track of the indexes of each element after
	 * sorting.
	 *
	 * @param indexes will be filled to contain the 'previous' indexes of the sorted elements.  E.g. if the second
	 * element in vector 0 is the largest value, then element 0 of indexes will be set to 1.
	 * @throws std::length_error
	 * @tparam IndexType the type used to store the indexes themselves
	 */
	template<typename IndexType>
	void sortAscendingAndTrackIndexes(std::vector<IndexType> & indexes) {
		if(indexes.size() != VectorSize * VecCount) {
			std::stringstream error;
			error << "Provided indexes vector has length " << indexes.size() << " but is required to have length ";
			error << (VectorSize * VecCount) << " elements";
			throw std::length_error(error.str().c_str());
		}
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			auto const indexesBegin = indexes.begin() + vectorIndex * VectorSize;
			auto const indexesEnd = indexes.begin() + vectorIndex * VectorSize + VectorSize;
			std::iota(indexesBegin, indexesEnd, 0);

			auto const weightsBegin = weights.begin() + vectorIndex * VectorSize;
			// Sort once to track indexes
			std::sort(indexesBegin, indexesEnd,
				[&weightsBegin](IndexType const i1, IndexType const i2) {
					return weightsBegin[i1] < weightsBegin[i2];
				}
			);
		}
		// Sort again to actual sort
		sortAscending();
	}

	/**
	 *
	 * Sorts the weights (per vector) in ascending order
	 */
	void sortAscending() {
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			std::sort(
				weights.begin() + vectorIndex * VectorSize,
				weights.begin() + vectorIndex * VectorSize + VectorSize,
				std::less<WeightType>()
			);
		}
	}

	/**
	 *
	 * Sorts the weights (per vector) in descending order
	 */
	void sortDescending() {
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			std::sort(
				weights.begin() + vectorIndex * VectorSize,
				weights.begin() + vectorIndex * VectorSize + VectorSize,
				std::greater<WeightType>()
			);
		}
	}

	/**
	 *
	 * @return the weight of the minimum (most likely) key candidate
	 */
	WeightType minimumWeight() const {
		WeightType minWeight = 0;
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			minWeight += *std::min_element(
				weights.begin() + vectorIndex * VectorSize,
				weights.begin() + vectorIndex * VectorSize + VectorSize
			);
		}
		return minWeight;
	}

	/**
	 *
	 * @return the weight of the maximum (least likely) key candidate
	 */
	WeightType maximumWeight() const {
		WeightType maxWeight = 0;
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			maxWeight += *std::max_element(
				weights.begin() + vectorIndex * VectorSize,
				weights.begin() + vectorIndex * VectorSize + VectorSize
			);
		}
		return maxWeight;
	}

	/**
	 *
	 * @return access to the raw weights buffer
	 */
	std::vector<WeightType> const & allWeights() const {
		return weights;
	}
private:
	std::vector<WeightType> weights;
};

} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_WEIGHTTABLE_HPP_ */

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
 * DistinguishingTable.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_DISTINGUISHINGTABLE_HPP_
#define LABYNKYR_SRC_LABYNKYR_DISTINGUISHINGTABLE_HPP_

#include "labynkyr/VectorTransformations.hpp"
#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace labynkyr {

/**
 *
 * A DistinguishingTable contains the distinguishing scores associated with all the DPA attacks associated with the recovery of a full
 * key.  In this implementation we assume each attack targets the same size portion of the key (e.g. 16 8-bit SubBytes attack on AES,
 * and not 12 8-bit SubBytes and one 32-bit MixColumns attack)
 *
 * The scores are stored internally in a single vector, with the scores for the first distinguishing vector stored first in the buffer,
 * the second distinguishing vector in the next portion of the buffer, and so on.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam ScoresType the floating-point type used to store distinguishing scores (e.g float or double)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename ScoresType>
class DistinguishingTable {
public:
	enum {
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * @param scores construct a distinguishing table by passing in a vector of all the distinguishing scores
	 * @throws std::length_error
	 */
	DistinguishingTable(std::vector<ScoresType> scores)
	: scores(scores)
	{
		if(scores.size() != VectorSize * VecCount) {
			std::stringstream error;
			error << "Attack result consists of " << VecCount << "distinguishing vectors each of size ";
			error << VecLenBits << " bits. The distinguishing table must contain " << (VectorSize * VecCount) << " elements, ";
			error << "but provided table contains " << scores.size() << " elements";
			throw std::length_error(error.str().c_str());
		}
	}

	// Copy c-tor
	DistinguishingTable(DistinguishingTable<VecCount, VecLenBits, ScoresType> const & other)
	: scores(other.readAllScores())
	{
	}

	~DistinguishingTable() {}

	/**
	 *
	 * @param vectorIndex
	 * @param subkeyIndex
	 * @return the distinguishing score associated with the subkeyIndex subkey in the vectorIndex distinguishing vector
	 */
	ScoresType score(uint32_t vectorIndex, uint32_t subkeyIndex) const {
		return scores[vectorIndex * VectorSize + subkeyIndex];
	}

	/**
	 *
	 * Normalise each distinguishing vector in the table such that each vector (not the whole table) sums to 1.0.  This
	 * method assumes that each distinguishing score is already positive.
	 */
	void normaliseDistinguishingVectors() {
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			typename std::vector<ScoresType>::iterator scoresBegin = scores.begin() + vectorIndex * VectorSize;
			typename std::vector<ScoresType>::iterator scoresEnd = scoresBegin + VectorSize;
			VectorTransformations<ScoresType>::normalise(scoresBegin, scoresEnd);
		}
	}

	// Apply std::fabs() to every element in the table
	void applyAbsoluteValue() {
		VectorTransformations<ScoresType>::absoluteValue(scores.begin(), scores.end());
	}

	/**
	 *
	 * Shifts the entire distinguishing table such that the minimum score in the entire table is slightly above zero.
	 */
	void translateVectorsToPositive() {
		// Find the minimum value
		ScoresType const minValue = *std::min_element(scores.cbegin(), scores.cend());
		// Minimum value is 0.0 then the vector elements are already all positive and we don't need to do anything
		if(minValue <= static_cast<ScoresType>(0.0)) {
			// If we need to shift the scores, then add a small epsilon as a fudge to ensure that no score is 0.0 after translation
			ScoresType const epsilon = 0.000001;
			std::transform(
				scores.begin(), scores.end(), scores.begin(),
				[&minValue,&epsilon](ScoresType const & score) {
					return score - (minValue - epsilon);
				}
			);
		}
	}

	/**
	 *
	 * Replace every distinguishing score with log2(score)
	 */
	void takeLogarithm() {
		takeLogarithm(2.0);
	}

	/**
	 *
	 * Replace every distinguishing score with log_base(score)
	 *
	 * @param logBase take the logs to this base
	 */
	void takeLogarithm(ScoresType logBase) {
		VectorTransformations<ScoresType>::logarithm(scores.begin(), scores.end(), logBase);
	}

	/**
	 *
	 * Copies the distinguishing table and creates a weight table.
	 *
	 * @param precisionBits the bits of precision retained when converting distinguishing scores to integer values.  ~16 is a
	 * reasonable number.  Larger values produce more accurate rank & enumeration calculations but at the cost of speed.
	 * @throws std::invalid_argument
	 * @throws std::logic_error
	 * @tparam WeightType the integer type used to store the weights (e.g. uint32_t)
	 */
	template<typename WeightType>
	std::unique_ptr<WeightTable<VecCount, VecLenBits, WeightType>> mapToWeight(uint32_t precisionBits) const {
		if(precisionBits <= 1) {
			throw std::invalid_argument("Cannot run mapToWeight at less than 2 bits of precision");
		}

		// Find the maximum distinguishing score and compute the multiplier to each score
		ScoresType const maxScore = *std::max_element(scores.begin(), scores.end());
		ScoresType alpha = std::log(maxScore) / std::log(2.0);
		if(std::isinf(alpha)) {
			throw std::logic_error("Maximum score is 0.0; cannot apply mapToWeight");
		}
		ScoresType const precisionMultiplier = std::pow(2.0, static_cast<ScoresType>(precisionBits) - alpha);

		// Go back through the vectors, finding and setting the mapped weights
		std::vector<WeightType> weights(VecCount * VectorSize);
		std::transform(
			scores.begin(),
			scores.end(),
			weights.begin(),
			[&precisionMultiplier](ScoresType const score) {
				return static_cast<WeightType>(score * precisionMultiplier);
			}
		);

		auto * weightTable = new WeightTable<VecCount, VecLenBits, WeightType>(weights);
		// There's a considerable speed improvement from translating the weights such that the most likely key has weight 1
		weightTable->rebase(1);
		return std::unique_ptr<WeightTable<VecCount, VecLenBits, WeightType>>(weightTable);
	}

	/**
	 *
	 * @return access to the raw scores buffer
	 */
	std::vector<ScoresType> & allScores() {
		return scores;
	}

	/**
	 *
	 * @return read access to the raw scores buffer
	 */
	std::vector<ScoresType> const & readAllScores() const {
		return scores;
	}
private:
	std::vector<ScoresType> scores;
};

} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_DISTINGUISHINGTABLE_HPP_ */

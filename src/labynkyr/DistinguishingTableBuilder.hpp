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
 * DistinguishingTableBuilder.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_DISTINGUISHINGTABLEBUILDER_HPP_
#define LABYNKYR_SRC_LABYNKYR_DISTINGUISHINGTABLEBUILDER_HPP_

#include "labynkyr/BitWindow.hpp"
#include "labynkyr/DistinguishingTable.hpp"

#include <stdint.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace labynkyr {

/**
 *
 * Helper class for building a distinguishing table
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam ScoresType the floating-point type used to store distinguishing scores (e.g float or double)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename ScoresType>
class DistinguishingTableBuilder {
public:
	using ScoresIter = typename std::vector<ScoresType>::const_iterator;

	enum {
		KeyLenBits = VecCount * VecLenBits,
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	DistinguishingTableBuilder()
	: scoresTable(VecCount * VectorSize)
	{
	}

	/**
	 *
	 * Add distinguishing scores for a particular distinguishing vector
	 *
	 * @param keyPortion the portion of the global key the scores are associated with
	 * @param scores
	 * @throws std::length_error
	 * @throws std::invalid_argument
	 */
	void addDistinguishingScores(BitWindow keyPortion, std::vector<ScoresType> const & scores) {
		addDistinguishingScores(keyPortion, scores.begin(), scores.end());
	}

	void addDistinguishingScores(BitWindow keyPortion, ScoresIter scoresBegin, ScoresIter scoresEnd) {
		// Check keyPortion describes a complete distinguishing vector
		if(keyPortion.getBitCount() != VecLenBits || keyPortion.getBitStart() % VecLenBits != 0) {
			std::stringstream error;
			error << "BitWindow of [" << keyPortion.getBitStart() << ", " << keyPortion.getBitEnd() << "] does not define a full ";
			error << "distinguishing vector for a key partitioned into " << VecCount << " distinguishing vectors each of size ";
			error << VecLenBits << " bits.";
			throw std::invalid_argument(error.str().c_str());
		}
		if(keyPortion.getBitEnd() >= KeyLenBits) {
			std::stringstream error;
			error << "BitWindow of [" << keyPortion.getBitStart() << ", " << keyPortion.getBitEnd() << "] is not valid for a ";
			error << "key of length " << KeyLenBits << " bits.";
			throw std::invalid_argument(error.str().c_str());
		}
		// Check size of scores matches the specified key portion
		uint64_t const scoresCount = std::distance(scoresBegin, scoresEnd);
		uint64_t const requiredScoresCount = keyPortion.totalPossibleValues();
		if(scoresCount != requiredScoresCount) {
			std::stringstream error;
			error << "BitWindow of [" << keyPortion.getBitStart() << ", " << keyPortion.getBitEnd() << "] defines distinguishing scores ";
			error << "for " << requiredScoresCount << " key candidates, but supplied vector contains " << scoresCount << " elements.";
			throw std::length_error(error.str().c_str());
		}
		// Determine vector offset
		uint32_t const vectorIndex = keyPortion.getBitStart() / VecLenBits;
		uint64_t const offset = vectorIndex * VectorSize;
		// Copy in scores
		std::copy(scoresBegin, scoresEnd, scoresTable.begin() + offset);
	}

	/**
	 *
	 * @return a new distinguishing table
	 */
	std::unique_ptr<DistinguishingTable<VecCount, VecLenBits, ScoresType>> createTable() const {
		// TODO would much prefer to use std::make_unique here
		auto * table = new DistinguishingTable<VecCount, VecLenBits, ScoresType>(scoresTable);
		return std::unique_ptr<DistinguishingTable<VecCount, VecLenBits, ScoresType>>(table);
	}
private:
	std::vector<ScoresType> scoresTable;
};

} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_DISTINGUISHINGTABLEBUILDER_HPP_ */

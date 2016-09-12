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
 * FullKeyBuilder.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_FULLKEYBUILDER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_FULLKEYBUILDER_HPP_

#include <stdint.h>

#include <vector>

namespace labynkyr {
namespace search {

/**
 *
 * Static methods for building byte-array representations of keys from not necessarily byte sized subkeys.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to score weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename SubkeyType>
class FullKeyBuilder {
public:

	/**
	 *
	 * @param input a vector of length VecCount
	 * @param output a vector of length VecCount * VecLenBits / 8; the number of bytes in the key.  Output will be modified to
	 * contain the byte representation of the key specified by the sub-key representation stored in input.
	 */
	static void fullKey(std::vector<SubkeyType> const & input, std::vector<uint8_t> & output) {
		uint32_t offset = 0;
		for(auto value : input) {
			uint32_t const bitEnd =  offset + VecLenBits - 1;

			for(uint32_t byteIndex = 0 ; byteIndex < output.size() ; byteIndex++) {
				uint32_t byteValue = 0;
				for(uint32_t bitIndex = 0 ; bitIndex < 8 ; bitIndex++) {
					bool bitValue = 0;
					if(bitIndex + (byteIndex * 8) < offset || bitIndex + (byteIndex * 8) > bitEnd) {
						bitValue = output[byteIndex] & (1 << bitIndex);
					} else {
						bitValue = value & (1 << (bitIndex + (byteIndex * 8) - offset));
					}
					byteValue |= (bitValue << bitIndex);
				}
				output[byteIndex] = static_cast<uint8_t>(byteValue);
			}
			offset += VecLenBits;
		}
	}
};

// Specialisation for 8-bit attacks
template<uint32_t VecCount>
class FullKeyBuilder<VecCount, 8U, uint8_t> {
public:
	static void fullKey(std::vector<uint8_t> const & input, std::vector<uint8_t> & output) {
		std::copy(input.begin(), input.end(), output.begin());
	}
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_FULLKEYBUILDER_HPP_ */

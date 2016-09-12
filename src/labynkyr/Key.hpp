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
 * Key.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_KEY_HPP_
#define LABYNKYR_SRC_LABYNKYR_KEY_HPP_

#include "labynkyr/BigInt.hpp"
#include "labynkyr/BitWindow.hpp"

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace labynkyr {

/**
 *
 * Class for storing and manipulating key values for use in rank calculation algorithms.
 *
 * @tparam KeyLenBits the length of the key in bits
 */
template<uint32_t KeyLenBits>
class Key {
public:
	/**
	 *
	 * Constructs a zero-value Key
	 */
	Key() {
		// Compute byte representation
		uint32_t const bitRemainder = KeyLenBits % 8;
		uint32_t const byteCount = (bitRemainder == 0) ? KeyLenBits / 8 : (KeyLenBits / 8) + 1;
		bytes.resize(byteCount);
	}

	/**
	 *
	 * @param hexString the key as a hex string.  The string is assumed to be little-endian.
	 * @throws std::invalid_argument if the string is not of the required length specified by the key size
	 */
	Key(std::string const & hexString)
	{
		// Compute byte representation
		uint32_t const bitRemainder = KeyLenBits % 8;
		uint32_t const byteCount = (bitRemainder == 0) ? KeyLenBits / 8 : (KeyLenBits / 8) + 1;
		if(hexString.length() != byteCount * 2) {
			std::stringstream errorStr;
			errorStr << "Hex string needs to be of length " << (byteCount * 2) << " for a key of size " << KeyLenBits << " bits";
			throw std::invalid_argument(errorStr.str().c_str());
		}
		bytes.resize(byteCount);
		hexStringToByteArray(hexString, bytes);
	}

	/**
	 *
	 * @param byteArray constructs an array using the supplied array of byte values.  The array is taken to be little-endian,
	 * e.g {0x01, 0x02} creates a key with integer value 256
	 * @throws std::length_error if the byte array isn't the correct size
	 */
	Key(std::vector<uint8_t> const & byteArray)
	: bytes(byteArray)
	{
		uint32_t const bitRemainder = KeyLenBits % 8;
		uint32_t const byteCount = (bitRemainder == 0) ? KeyLenBits / 8 : (KeyLenBits / 8) + 1;
		if(byteArray.size() != byteCount) {
			std::stringstream errorStr;
			errorStr << "Key is of size " << KeyLenBits << " bits, provided byte array has length of " << byteArray.size();
			throw std::length_error(errorStr.str().c_str());
		}
	}

	// Copy-constructor
	Key(Key<KeyLenBits> const & other)
	{
		bytes.resize(other.asBytes().size());
		std::copy(other.asBytes().begin(), other.asBytes().end(), bytes.begin());
	}

	~Key() {}

	/**
	 *
	 * @return a little-endian byte representation of the key
	 */
	std::vector<uint8_t> const & asBytes() const {
		return bytes;
	}

	/**
	 *
	 * @param subkeyPortion the portion of the key to extract the subkey value from
	 * @return the integer value of the portion of the key specified by the subkeyPortion
	 * @throws std::length_error
	 */
	uint64_t subkeyValue(BitWindow const & subkeyPortion) const {
		if(subkeyPortion.getBitEnd() >= KeyLenBits) {
			std::stringstream error;
			error << "Cannot extract subkey value for invalid bit window [" << subkeyPortion.getBitStart() << ", ";
			error << subkeyPortion.getBitEnd() << "] given a key of length " << KeyLenBits << " bits";
			throw std::length_error(error.str().c_str());
		}
		uint64_t value = 0;
		uint64_t stateBitIndex = 0;
		for(uint32_t bit = subkeyPortion.getBitStart() ; bit <= subkeyPortion.getBitEnd() ; bit++) {
			uint32_t const byteIndex = bit / 8;
			uint32_t const bitOffset = bit % 8;
			uint32_t const bitValue = (bytes[byteIndex] & (1 << bitOffset)) >> bitOffset;
			value |= (static_cast<uint64_t>(bitValue) << stateBitIndex);
			stateBitIndex++;
		}
		return value;
	}

	/**
	 *
	 * @return the integer value of the key
	 */
	BigInt<KeyLenBits> asIntegerValue() const {
		BigInt<KeyLenBits> integerValue(0);
		// Compute integer value
		for(uint32_t byteIndex = 0 ; byteIndex < bytes.size() ; byteIndex++) {
			BigInt<KeyLenBits> byteValue(bytes[byteIndex]);
			integerValue += byteValue << (byteIndex * 8);
		}
		return integerValue;
	}
private:
	std::vector<uint8_t> bytes;

	/**
	 * Converts a hex string to a byte array
	 *
	 * @param hexString the input hex string
	 * @param array the output array
	 */
	static void hexStringToByteArray(std::string const & s, std::vector<uint8_t> & array) {
		char const * text = s.c_str();
		uint32_t temp;
		for(uint32_t i = 0; i < array.size(); ++i ) {
			std::sscanf(text + 2 * i, "%2x", &temp);
			array[i] = static_cast<uint8_t>(temp);
		}
	}
};

} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_KEY_HPP_ */

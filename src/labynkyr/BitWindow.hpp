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
 * BitWindow.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_BITWINDOW_HPP_
#define LABYNKYR_SRC_LABYNKYR_BITWINDOW_HPP_

#include <stdint.h>

#include <sstream>
#include <stdexcept>
#include <vector>

namespace labynkyr {

/**
 *
 * BitWindow is used to define a continuous window of bits within an encryption key or some cipher material.  For instance,
 * if one was to try a DPA attack on the first byte of a key, then the BitWindow would be defined as the bit index 0
 * and then a further offset of 8 bits.
 */
class BitWindow {
public:
	/**
	 *
	 * @param bitStart the starting bit index of the window.  Indexes are taken left-to-right over an array; bit index 0 is the
	 * left-most bit of array[0].
	 * @param bitCount the size of the window beginning at bitStart.  Must be > 0.
	 * @throws std::invalid_argument
	 */
	BitWindow(uint32_t const bitStart, uint32_t const bitCount)
	: bitStart(bitStart)
	, bitCount(bitCount)
	{
		if(bitCount == 0) {
			throw std::invalid_argument("Cannot construct a BitLocation with a window size of 0.");
		}
		bitEnd = bitStart + bitCount - 1;
	}

	/**
	 *
	 * Default constructor
	 */
	BitWindow()
	: bitStart(0)
	, bitCount(1)
	, bitEnd(0)
	{
	}

	// Copy c-tor
	BitWindow(BitWindow const & other)
	: bitStart(other.getBitStart())
	, bitCount(other.getBitCount())
	{
		bitEnd = bitStart + bitCount - 1;
	}

	~BitWindow() {}

	/**
	 *
	 * @return the starting bit index of this window
	 */
	uint32_t getBitStart() const {
		return bitStart;
	}

	/**
	 *
	 * @return the size (number of bits) of the window
	 */
	uint32_t getBitCount() const {
		return bitCount;
	}

	/**
	 *
	 * @return the final bit index associated with this window
	 */
	uint32_t getBitEnd() const {
		return bitEnd;
	}

	/**
	 *
	 * @return the number of possible unsigned integer values defined by this window
	 */
	uint64_t totalPossibleValues() const {
		return 1UL << static_cast<uint64_t>(bitCount);
	}

	/**
	 *
	 * @param other
	 * @return true if this BitWindow encompasses the supplied window other.
	 * Examples:
	 * 	this -> [0,...,16]			location -> [0, ...., 5] 		-> return true
	 * 	this -> [0,...,16]			location -> [10, ...., 18] 		-> return false
	 */
	bool encapsulates(BitWindow const & other) const {
		if(bitStart <= other.getBitStart() && bitEnd >= other.getBitEnd()) {
			return true;
		} else {
			return false;
		}
	}

	// C++ swap idiom
	friend void swap(BitWindow & first, BitWindow & second) {
		std::swap(first.bitStart, second.bitStart);
		std::swap(first.bitCount, second.bitCount);
		std::swap(first.bitEnd, second.bitEnd);
	}

	// Copy assignment
	BitWindow & operator=(BitWindow other) {
	    swap(*this, other);
	    return *this;
	}
private:
	uint32_t bitStart;
	uint32_t bitCount;
	uint32_t bitEnd;

	friend bool operator==(BitWindow const & lhs, BitWindow const & rhs) {
		if(lhs.getBitStart() == rhs.getBitStart() && lhs.getBitEnd() == rhs.getBitEnd()) {
			return true;
		} else {
			return false;
		}
	}

	friend bool operator!=(BitWindow const & lhs, BitWindow const & rhs)  {
		return !operator==(lhs,rhs);
	}
};

} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_BITWINDOW_HPP_ */

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
 * BigInt.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_BIGINT_HPP_
#define LABYNKYR_SRC_LABYNKYR_BIGINT_HPP_

#include <boost/multiprecision/cpp_int.hpp>

#include <stdint.h>

namespace labynkyr {

/**
 *
 * Big integer class for storing the rank for a given key (needs to be the length
 * of the key in bits to store the rank!).  Uses boost::multiprecision::cpp_int to
 * handle the logic.
 *
 * @tparam LengthBits
 */
template<uint32_t LengthBits>
using BigInt = typename boost::multiprecision::number<
	boost::multiprecision::cpp_int_backend<
		LengthBits,
		LengthBits,
		boost::multiprecision::unsigned_magnitude,
		boost::multiprecision::unchecked,
		void
	>
>;

class BigIntTools {
public:
	/**
	 *
	 * Return the big integer value of 2 ^ x
	 *
	 * @param x
	 * @return 2 ^ x
	 * @tparam LengthBits the number of bits of big integer precision
	 */
	template<uint32_t LengthBits>
	static BigInt<LengthBits> twoX(uint32_t x) {
		BigInt<LengthBits> one(1);
		return one << x;
	}
};

} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_BIGINT_HPP_ */

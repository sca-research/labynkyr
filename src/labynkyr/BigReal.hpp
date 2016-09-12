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
 * BigReal.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_BIGREAL_HPP_
#define LABYNKYR_SRC_LABYNKYR_BIGREAL_HPP_

#include "labynkyr/BigInt.hpp"

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <stdint.h>

namespace labynkyr {

/**
 *
 * Big float class
 *
 * @tparam Digits10 the number of digits of fixed floating-point precision
 */
template<uint32_t Digits10>
using BigReal = typename boost::multiprecision::number<boost::multiprecision::cpp_dec_float<Digits10>>;

class BigRealTools {
public:
	/**
	 *
	 * Return the floating-point log base 2 value of a big integer
	 *
	 * @param integer
	 * @return log2(integer)
	 * @tparam LengthBits the number of bits of big integer precision
	 * @tparam Digits10 the number of digits of fixed floating-point precision
	 */
	template<uint32_t LengthBits, uint32_t Digits10>
	static double log2(BigInt<LengthBits> integer) {
		// Temporary work-around.  See definition for BigIntTemp for explanation.
		BigIntTemp<LengthBits + 1> const tempInt = static_cast<BigIntTemp<LengthBits + 1>>(integer);
		BigReal<Digits10> const floatValue = static_cast<BigReal<Digits10>>(tempInt);
		BigReal<Digits10> const two(2.0);
		BigReal<Digits10> const logTwo = boost::multiprecision::log(two);
		BigReal<Digits10> const logValue = boost::multiprecision::log(floatValue) / logTwo;
		return static_cast<double>(logValue);
	}

	/**
	 *
	 * Return the big integer value of 2 ^ x
	 *
	 * @param x
	 * @return 2 ^ x
	 * @tparam LengthBits the number of bits of big integer precision
	 * @tparam Digits10 the number of digits of fixed floating-point precision
	 */
	template<uint32_t LengthBits, uint32_t Digits10>
	static BigInt<LengthBits> twoX(double x) {
		BigReal<Digits10> const two(2.0);
		BigReal<Digits10> const realResult = boost::multiprecision::round(boost::multiprecision::pow(two, x));
		// TODO the commented out method is preferable, but there's an issue with convert_to in boost 1.54.  So for now, we
		// can just accept the loss of casting down to a double.
		//BigInt<LengthBits> const result = realResult.template convert_to<BigInt<LengthBits>>();
		BigInt<LengthBits> const result = static_cast<BigInt<LengthBits>>(realResult.template convert_to<double>());
		return result;
	}
private:
	/**
	 *
	 * This is needed as some versions of boost can't seem to compile
	 * 		static_cast<boost::multiprecision::cpp_dec_float>(boost::multiprecision::cpp_int);
	 * if the cpp_int uses unsigned_magnitude and a void allocator.
	 *
	 * The workaround is to do an intermediate cast to the integer type defined below.
	 *
	 */
	template<uint32_t LengthBits>
	using BigIntTemp = typename boost::multiprecision::number<
		boost::multiprecision::cpp_int_backend<
			LengthBits,
			LengthBits,
			boost::multiprecision::signed_magnitude,
			boost::multiprecision::unchecked,
			std::allocator<boost::multiprecision::limb_type>
		>
	>;
};

} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_BIGREAL_HPP_ */

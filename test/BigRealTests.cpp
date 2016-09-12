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
 * BigRealTests.cpp
 *
 */

#include "src/labynkyr/BigReal.hpp"

#include "src/labynkyr/BigInt.hpp"

#include <unittest++/UnitTest++.h>

namespace labynkyr {

TEST(BigReal_log2_1) {
	BigInt<128> const one(1);
	BigInt<128> const value = one << 87;
	double const log2 = BigRealTools::log2<128, 100>(value);
	CHECK_EQUAL(87.0, log2);
}

TEST(BigReal_log2_2) {
	BigInt<128> const value = 28652746234123;
	double const log2 = BigRealTools::log2<128, 100>(value);
	CHECK_CLOSE(44.7037, log2, 0.0001);
}

TEST(BigReal_twoX_1) {
	BigInt<128> const expected = 28651978537278;
	BigInt<128> const result = BigRealTools::twoX<128, 50>(44.7037);
	CHECK(expected == result);
}

TEST(BigReal_twoX_2) {
	BigInt<128> const expected("18730198603858796544");
	BigInt<128> const result = BigRealTools::twoX<128, 100>(64.022);
	CHECK(expected == result);
}

} /* namespace labynkyr */



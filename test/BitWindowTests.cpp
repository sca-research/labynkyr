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
 * BitWindowTests.cpp
 *
 */

#include "src/labynkyr/BitWindow.hpp"

#include <unittest++/UnitTest++.h>

#include <stdexcept>
#include <vector>

namespace labynkyr {

TEST(BitWindow_getLengths_vector1) {
	BitWindow const loc(0, 1);
	CHECK_EQUAL(0, loc.getBitStart());
	CHECK_EQUAL(1, loc.getBitCount());
	CHECK_EQUAL(0, loc.getBitEnd());
}

TEST(BitWindow_getLengths_vector2) {
	BitWindow const loc(1, 4);
	CHECK_EQUAL(1, loc.getBitStart());
	CHECK_EQUAL(4, loc.getBitCount());
	CHECK_EQUAL(4, loc.getBitEnd());
}

TEST(BitWindow_getLengths_vector3) {
	BitWindow const loc(7, 16);
	CHECK_EQUAL(7, loc.getBitStart());
	CHECK_EQUAL(16, loc.getBitCount());
	CHECK_EQUAL(22, loc.getBitEnd());
}

TEST(BitWindow_zeroCount) {
	CHECK_THROW(BitWindow loc(0, 0), std::invalid_argument);
}

TEST(BitWindow_totalPossibleValues_v1) {
	BitWindow const loc(2, 1);
	CHECK_EQUAL(2, loc.totalPossibleValues());
}

TEST(BitWindow_totalPossibleValues_v2) {
	BitWindow const loc(2, 2);
	CHECK_EQUAL(4, loc.totalPossibleValues());
}

TEST(BitWindow_totalPossibleValues_v3) {
	BitWindow const loc(0, 8);
	CHECK_EQUAL(256, loc.totalPossibleValues());
}

TEST(BitWindow_totalPossibleValues_v4) {
	BitWindow const loc(12, 16);
	CHECK_EQUAL(65536, loc.totalPossibleValues());
}

TEST(BitWindow_totalPossibleValues_v5) {
	BitWindow const loc(18, 32);
	CHECK_EQUAL(4294967296UL, loc.totalPossibleValues());
}

TEST(BitWindow_encapsulates_v1) {
	BitWindow const container(18, 32);
	BitWindow const loc(18, 36);
	CHECK_EQUAL(false, container.encapsulates(loc));
}

TEST(BitWindow_encapsulates_v2) {
	BitWindow const container(0, 1);
	BitWindow const loc(0, 1);
	CHECK_EQUAL(true, container.encapsulates(loc));
}

TEST(BitWindow_encapsulates_v3) {
	BitWindow const container(5, 5);
	BitWindow const loc(9, 1);
	CHECK_EQUAL(true, container.encapsulates(loc));
}

TEST(BitWindow_encapsulates_v5) {
	BitWindow const container(5, 1);
	BitWindow const loc(10, 1);
	CHECK_EQUAL(false, container.encapsulates(loc));
}

TEST(BitWindow_encapsulates_v6) {
	BitWindow const container(18, 32);
	BitWindow const loc(17, 5);
	CHECK_EQUAL(false, container.encapsulates(loc));
}

TEST(BitWindow_inequality_v1) {
	BitWindow const b1(18, 32);
	BitWindow const b2(17, 5);
	bool const equal = (b1 == b2);
	CHECK_EQUAL(false, equal);
}

TEST(BitWindow_inequality_v2) {
	BitWindow const b1(18, 32);
	BitWindow const b2(18, 31);
	bool const equal = (b1 == b2);
	CHECK_EQUAL(false, equal);
}

TEST(BitWindow_inequality_v3) {
	BitWindow const b1(18, 6);
	BitWindow const b2(17, 6);
	bool const equal = (b1 == b2);
	CHECK_EQUAL(false, equal);
}

TEST(BitWindow_equality) {
	BitWindow const b1(18, 32);
	BitWindow const b2(18, 32);
	bool const equal = (b1 == b2);
	CHECK_EQUAL(true, equal);
}

TEST(BitWindow_getLengths_copyConstructor1) {
	BitWindow const loc(10, 1);
	BitWindow const actual(loc);
	CHECK_EQUAL(10, actual.getBitStart());
	CHECK_EQUAL(1, actual.getBitCount());
	CHECK_EQUAL(10, actual.getBitEnd());
}

TEST(BitWindow_getLengths_copyAssign) {
	BitWindow const loc(10, 1);
	BitWindow const actual = loc;
	CHECK_EQUAL(10, actual.getBitStart());
	CHECK_EQUAL(1, actual.getBitCount());
	CHECK_EQUAL(10, actual.getBitEnd());
}

} /* namespace labynkyr */

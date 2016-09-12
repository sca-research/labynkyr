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
 * KeyTests.cpp
 *
 */

#include "src/labynkyr/Key.hpp"

#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/BitWindow.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace labynkyr {

TEST(Key_zero_value_constructor) {
	Key<40> const key;
	CHECK_EQUAL(5, key.asBytes().size());
}

TEST(Key_arrayConstructor_asBytes1) {
	std::vector<uint8_t> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04};
	Key<40> const key(bytes);
	CHECK_ARRAY_EQUAL(bytes.data(), key.asBytes().data(), bytes.size());
}

TEST(Key_arrayConstructor_asBytes2) {
	std::vector<uint8_t> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	Key<64> const key(bytes);
	CHECK_ARRAY_EQUAL(bytes.data(), key.asBytes().data(), bytes.size());
}

TEST(Key_arrayConstructor_asBytes3) {
	std::vector<uint8_t> const bytes = {0x05};
	Key<4> const key(bytes);
	CHECK_ARRAY_EQUAL(bytes.data(), key.asBytes().data(), bytes.size());
}

TEST(Key_arrayConstructor_tooSmall) {
	std::vector<uint8_t> const bytes = {0x00, 0x01, 0x02, 0x03};
	// 39 bit key requires 5 bytes
	CHECK_THROW(Key<39> const key(bytes), std::length_error);
}

TEST(Key_arrayConstructor_tooBig) {
	std::vector<uint8_t> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04, 0x5};
	// 39 bit key requires 5 bytes
	CHECK_THROW(Key<39> const key(bytes), std::length_error);
}

TEST(Key_hexStringConstructor_asBytes1) {
	std::vector<uint8_t> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04};
	Key<40> const key("0001020304");
	CHECK_ARRAY_EQUAL(bytes.data(), key.asBytes().data(), bytes.size());
}

TEST(Key_hexStringConstructor_asBytes2) {
	std::vector<uint8_t> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	Key<64> const key("0001020304050607");
	CHECK_ARRAY_EQUAL(bytes.data(), key.asBytes().data(), bytes.size());
}

TEST(Key_hexStringConstructor_asBytes3) {
	std::vector<uint8_t> const bytes = {0x05};
	Key<4> const key("05");
	CHECK_ARRAY_EQUAL(bytes.data(), key.asBytes().data(), bytes.size());
}

TEST(Key_hexStringConstructor_invalidStringSize1) {
	CHECK_THROW(Key<40> const key("000102030"), std::invalid_argument);
}

TEST(Key_hexStringConstructor_invalidStringSize2) {
	CHECK_THROW(Key<40> const key("00010203040"), std::invalid_argument);
}

TEST(Key_hexStringConstructor_invalidStringSize3) {
	CHECK_THROW(Key<16> const key("00"), std::invalid_argument);
}

TEST(Key_hexStringConstructor_invalidStringSize4) {
	CHECK_THROW(Key<16> const key("FFFFFF"), std::invalid_argument);
}

TEST(Key_hexStringConstructor_invalidStringSize5) {
	CHECK_THROW(Key<16> const key(""), std::invalid_argument);
}

TEST(Key_zeroConstructor_asBytes) {
	Key<40> const key;
	std::vector<uint8_t> const expected(5);
	CHECK_ARRAY_EQUAL(expected, key.asBytes(), expected.size());
}

TEST(Key_arrayConstructor_asIntegerValue1) {
	std::vector<unsigned char> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04};
	Key<40> const key(bytes);
	BigInt<40> const expected(17230332160);
	CHECK_EQUAL(expected, key.asIntegerValue());
}

TEST(Key_arrayConstructor_asIntegerValue2) {
	std::vector<unsigned char> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	Key<64> const key(bytes);
	BigInt<64> const expected(506097522914230528UL);
	CHECK_EQUAL(expected, key.asIntegerValue());
}

TEST(Key_arrayConstructor_asIntegerValue3) {
	std::vector<unsigned char> const bytes = {0x05};
	Key<4> const key(bytes);
	BigInt<4> const expected(5);
	CHECK_EQUAL(expected, key.asIntegerValue());
}

TEST(Key_hexStringConstructor_asIntegerValue1) {
	Key<40> const key("0001020304");
	BigInt<40> const expected(17230332160);
	CHECK_EQUAL(expected, key.asIntegerValue());
}

TEST(Key_hexStringConstructor_asIntegerValue2) {
	Key<64> const key("0001020304050607");
	BigInt<64> const expected(506097522914230528UL);
	CHECK_EQUAL(expected, key.asIntegerValue());
}

TEST(Key_hexStringConstructor_asIntegerValue3) {
	Key<4> const key("05");
	BigInt<4> const expected(5);
	CHECK_EQUAL(expected, key.asIntegerValue());
}

TEST(Key_copyConstructor) {
	std::vector<uint8_t> const bytes = {0x00, 0x01, 0x02, 0x03, 0x04};
	Key<40> const other(bytes);
	Key<40> const key(other);
	CHECK_ARRAY_EQUAL(bytes.data(), key.asBytes().data(), bytes.size());
}

TEST(Key_subkeyValue_endBytes) {
	std::vector<uint8_t> const keyBytes = {0x07, 0x07, 0x00, 0x00, 0x00, 0x9F};
	uint64_t const expected = 2667577344;
	Key<48> const key(keyBytes);
	uint64_t const actual = key.subkeyValue(BitWindow(16, 32));
	CHECK_EQUAL(expected, actual);
}

TEST(Key_subkeyValue_1) {
	std::vector<uint8_t> const keyBytes = {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07};
	uint64_t const expected = 7;
	Key<128> const key(keyBytes);
	uint64_t const actual = key.subkeyValue(BitWindow(0, 8));
	CHECK_EQUAL(expected, actual);
}

TEST(Key_subkeyValue_twoBytes) {
	std::vector<uint8_t> const keyBytes = {0x01, 0x02, 0x03, 0x04};
	Key<32> const key(keyBytes);
	uint64_t const actual = key.subkeyValue(BitWindow(8, 16));
	uint64_t const expected = 770;
	CHECK_EQUAL(expected, actual);
}

TEST(Key_subkeyValue_smallBits) {
	std::vector<uint8_t> const keyBytes = {0x01, 0x02, 0x03, 0x04};
	Key<32> const key(keyBytes);
	uint64_t const actual = key.subkeyValue(BitWindow(0, 1));
	uint64_t const expected = 1;
	CHECK_EQUAL(expected, actual);
}

TEST(Key_subkeyValue_truncated) {
	Key<11> const key("6502");
	// Bits 6, 7, 8 and 9 (so last 2 from the first byte, and the first 2 from the second byte) = 0b1001
	uint64_t const actual = key.subkeyValue(BitWindow(6, 4));
	uint64_t const expected = 9;
	CHECK_EQUAL(expected, actual);
}

TEST(Key_subkeyValue_invalidLengths1) {
	Key<32> const key("01020304");
	CHECK_THROW(key.subkeyValue(BitWindow(32, 1)), std::length_error);
}

TEST(Key_subkeyValue_invalidLengths2) {
	Key<32> const key("01020304");
	CHECK_THROW(key.subkeyValue(BitWindow(31, 2)), std::length_error);
}

TEST(Key_subkeyValue_invalidLengths3) {
	Key<32> const key("01020304");
	CHECK_THROW(key.subkeyValue(BitWindow(0, 33)), std::length_error);
}

} /* namespace labynkyr */



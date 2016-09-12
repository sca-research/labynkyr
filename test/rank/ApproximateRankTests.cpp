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
 * ApproximateRankTests.cpp
 *
 */

#include "src/labynkyr/rank/ApproximateRank.hpp"

#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/DistinguishingTableBuilder.hpp"
#include "src/labynkyr/DistinguishingTable.hpp"
#include "src/labynkyr/Key.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <functional>
#include <vector>

namespace labynkyr {
namespace rank {

TEST(ApproximateRank_twoVectors_greater_double) {
	std::vector<uint8_t> const correctKeyBytes = {0x00, 0x01};
	Key<16> const correctKey(correctKeyBytes);

	std::vector<double> vector1Data(256);
	std::fill(vector1Data.begin(), vector1Data.end(), 5.0);
	vector1Data[0] = 6.0;
	vector1Data[1] = 7.0;
	vector1Data[2] = 8.0; // Subkey rank of 2 (3 when indexed from 1)

	std::vector<double> vector2Data(256);
	std::fill(vector2Data.begin(), vector2Data.end(), 3.0);
	vector2Data[0] = 16.0;
	vector2Data[1] = 7.0;
	vector2Data[2] = 16.0;
	vector2Data[4] = 18.0; // Subkey rank of 3 (4 when indexed from 1)

	DistinguishingTableBuilder<2, 8, double> builder;
	builder.addDistinguishingScores(BitWindow(0, 8), vector1Data);
	builder.addDistinguishingScores(BitWindow(8, 8), vector2Data);
	auto table = builder.createTable();

	BigInt<16> const expected = 12;
	BigInt<16> const actual = ApproximateRank<2, 8, double>::rank<std::greater<double>>(
			*table.get(),
			correctKey,
			std::greater<double>()
	);
	CHECK_EQUAL(expected, actual);
}

TEST(ApproximateRank_twoVectors_greater_withTies_double) {
	std::vector<uint8_t> const correctKeyBytes = {0x00, 0x01};
	Key<16> const correctKey(correctKeyBytes);

	std::vector<double> vector1Data(256);
	std::fill(vector1Data.begin(), vector1Data.end(), 5.0);
	vector1Data[0] = 6.0;
	vector1Data[1] = 6.0;
	vector1Data[2] = 8.0; // Subkey rank of 1 (2 when indexed from 1) as ties are not counted

	std::vector<double> vector2Data(256);
	std::fill(vector2Data.begin(), vector2Data.end(), 3.0);
	vector2Data[0] = 16.0;
	vector2Data[1] = 7.0;
	vector2Data[2] = 16.0;
	vector2Data[4] = 18.0; // Subkey rank of 3 (4 when indexed from 1)

	DistinguishingTableBuilder<2, 8, double> builder;
	builder.addDistinguishingScores(BitWindow(0, 8), vector1Data);
	builder.addDistinguishingScores(BitWindow(8, 8), vector2Data);
	auto table = builder.createTable();

	BigInt<16> const expected = 8;
	BigInt<16> const actual = ApproximateRank<2, 8, double>::rank<std::greater<double>>(
			*table.get(),
			correctKey,
			std::greater<double>()
	);
	CHECK_EQUAL(expected, actual);
}

TEST(ApproximateRank_twoVectors_less_double) {
	std::vector<uint8_t> const correctKeyBytes = {0x00, 0x01};
	Key<16> const correctKey(correctKeyBytes);

	std::vector<double> vector1Data(256);
	std::fill(vector1Data.begin(), vector1Data.end(), 5.0);
	vector1Data[0] = 4.0;
	vector1Data[1] = 3.0;
	vector1Data[2] = 5.0; // Subkey rank of 1 (2 when indexed from 1)

	std::vector<double> vector2Data(256);
	std::fill(vector2Data.begin(), vector2Data.end(), 3.0);
	vector2Data[0] = 1.1;
	vector2Data[1] = 1.4;
	vector2Data[2] = 1.2;
	vector2Data[4] = 1.45; // Subkey rank of 2 (3 when indexed from 1)

	DistinguishingTableBuilder<2, 8, double> builder;
	builder.addDistinguishingScores(BitWindow(0, 8), vector1Data);
	builder.addDistinguishingScores(BitWindow(8, 8), vector2Data);
	auto table = builder.createTable();

	BigInt<16> const expected = 6;
	BigInt<16> const actual = ApproximateRank<2, 8, double>::rank<std::less<double>>(
			*table.get(),
			correctKey,
			std::less<double>()
	);
	CHECK_EQUAL(expected, actual);
}

TEST(ApproximateRank_twoVectors_less_withTies_double) {
	std::vector<uint8_t> const correctKeyBytes = {0x00, 0x01};
	Key<16> const correctKey(correctKeyBytes);

	std::vector<double> vector1Data(256);
	std::fill(vector1Data.begin(), vector1Data.end(), 5.0);
	vector1Data[0] = 3.0;
	vector1Data[1] = 3.0;
	vector1Data[2] = 5.0; // Subkey rank of 0 (1 when indexed from 1)

	std::vector<double> vector2Data(256);
	std::fill(vector2Data.begin(), vector2Data.end(), 3.0);
	vector2Data[0] = 1.0;
	vector2Data[1] = 1.0;
	vector2Data[2] = 1.2;
	vector2Data[4] = 1.45; // Subkey rank of 0 (1 when indexed from 1)

	DistinguishingTableBuilder<2, 8, double> builder;
	builder.addDistinguishingScores(BitWindow(0, 8), vector1Data);
	builder.addDistinguishingScores(BitWindow(8, 8), vector2Data);
	auto table = builder.createTable();

	BigInt<16> const expected = 1;
	BigInt<16> const actual = ApproximateRank<2, 8, double>::rank<std::less<double>>(
			*table.get(),
			correctKey,
			std::less<double>()
	);
	CHECK_EQUAL(expected, actual);
}

TEST(ApproximateRank_twoVectors_greater_float) {
	std::vector<uint8_t> const correctKeyBytes = {0x00, 0x01};
	Key<16> const correctKey(correctKeyBytes);

	std::vector<float> vector1Data(256);
	std::fill(vector1Data.begin(), vector1Data.end(), 5.0);
	vector1Data[0] = 6.0;
	vector1Data[1] = 7.0;
	vector1Data[2] = 8.0; // Subkey rank of 2 (3 when indexed from 1)

	std::vector<float> vector2Data(256);
	std::fill(vector2Data.begin(), vector2Data.end(), 3.0);
	vector2Data[0] = 16.0;
	vector2Data[1] = 7.0;
	vector2Data[2] = 16.0;
	vector2Data[4] = 18.0; // Subkey rank of 3 (4 when indexed from 1)

	DistinguishingTableBuilder<2, 8, float> builder;
	builder.addDistinguishingScores(BitWindow(0, 8), vector1Data);
	builder.addDistinguishingScores(BitWindow(8, 8), vector2Data);
	auto table = builder.createTable();

	BigInt<16> const expected = 12;
	BigInt<16> const actual = ApproximateRank<2, 8, float>::rank<std::greater<float>>(
			*table.get(),
			correctKey,
			std::greater<float>()
	);
	CHECK_EQUAL(expected, actual);
}

TEST(ApproximateRank_twoVectors_less_float) {
	std::vector<uint8_t> const correctKeyBytes = {0x00, 0x01};
	Key<16> const correctKey(correctKeyBytes);

	std::vector<float> vector1Data(256);
	std::fill(vector1Data.begin(), vector1Data.end(), 5.0);
	vector1Data[0] = 4.0;
	vector1Data[1] = 3.0;
	vector1Data[2] = 5.0; // Subkey rank of 1 (2 when indexed from 1)

	std::vector<float> vector2Data(256);
	std::fill(vector2Data.begin(), vector2Data.end(), 3.0);
	vector2Data[0] = 1.1;
	vector2Data[1] = 1.4;
	vector2Data[2] = 1.2;
	vector2Data[4] = 1.45; // Subkey rank of 2 (3 when indexed from 1)

	DistinguishingTableBuilder<2, 8, float> builder;
	builder.addDistinguishingScores(BitWindow(0, 8), vector1Data);
	builder.addDistinguishingScores(BitWindow(8, 8), vector2Data);
	auto table = builder.createTable();

	BigInt<16> const expected = 6;
	BigInt<16> const actual = ApproximateRank<2, 8, float>::rank<std::less<float>>(
			*table.get(),
			correctKey,
			std::less<float>()
	);
	CHECK_EQUAL(expected, actual);
}

} /* namespace rank */
} /* namespace labynkyr */



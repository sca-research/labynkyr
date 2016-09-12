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
 * DistinguishingTableBuilderTests.cpp
 *
 */

#include "src/labynkyr/DistinguishingTableBuilder.hpp"

#include "src/labynkyr/BitWindow.hpp"
#include "src/labynkyr/DistinguishingTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <stdexcept>
#include <vector>

namespace labynkyr {

TEST(DistinguishingTableBuilder_createTable_double) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};

	DistinguishingTableBuilder<2, 2, double> builder;
	builder.addDistinguishingScores(BitWindow(0, 2), scores.begin(), scores.begin() + 4);
	builder.addDistinguishingScores(BitWindow(2, 2), scores.begin() + 4, scores.end());

	auto table = builder.createTable();
	CHECK_ARRAY_EQUAL(scores, table->allScores(), scores.size());
}

TEST(DistinguishingTableBuilder_createTable_entireVectorArg_double) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	std::vector<double> const scores1 = {1.1, 2.2, 3.3, 4.4};
	std::vector<double> const scores2 = {5.5, 6.6, 7.7, 8.8};

	DistinguishingTableBuilder<2, 2, double> builder;
	builder.addDistinguishingScores(BitWindow(2, 2), scores2);
	builder.addDistinguishingScores(BitWindow(0, 2), scores1);

	auto table = builder.createTable();
	CHECK_ARRAY_EQUAL(scores, table->allScores(), scores.size());
}

TEST(DistinguishingTableBuilder_createTable_overwrite_double) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};

	DistinguishingTableBuilder<2, 2, double> builder;
	builder.addDistinguishingScores(BitWindow(0, 2), scores.begin(), scores.begin() + 4);
	builder.addDistinguishingScores(BitWindow(2, 2), scores.begin(), scores.begin() + 4);
	builder.addDistinguishingScores(BitWindow(2, 2), scores.begin() + 4, scores.end());

	auto table = builder.createTable();
	CHECK_ARRAY_EQUAL(scores, table->allScores(), scores.size());
}

TEST(DistinguishingTableBuilder_addDistinguishingScores_invalidBitWindow1) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	DistinguishingTableBuilder<2, 2, double> builder;
	CHECK_THROW(builder.addDistinguishingScores(BitWindow(0, 1), scores.begin(), scores.begin() + 4), std::invalid_argument);
}

TEST(DistinguishingTableBuilder_addDistinguishingScores_invalidBitWindow2) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	DistinguishingTableBuilder<2, 2, double> builder;
	CHECK_THROW(builder.addDistinguishingScores(BitWindow(1, 2), scores.begin(), scores.begin() + 4), std::invalid_argument);
}

TEST(DistinguishingTableBuilder_addDistinguishingScores_invalidBitWindow3) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	DistinguishingTableBuilder<2, 2, double> builder;
	CHECK_THROW(builder.addDistinguishingScores(BitWindow(4, 2), scores.begin(), scores.begin() + 4), std::invalid_argument);
}

TEST(DistinguishingTableBuilder_addDistinguishingScores_invalidScoresCount_small) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	DistinguishingTableBuilder<2, 2, double> builder;
	CHECK_THROW(builder.addDistinguishingScores(BitWindow(0, 2), scores.begin(), scores.begin() + 3), std::length_error);
}

TEST(DistinguishingTableBuilder_addDistinguishingScores_invalidScoresCount_big) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	DistinguishingTableBuilder<2, 2, double> builder;
	CHECK_THROW(builder.addDistinguishingScores(BitWindow(0, 2), scores.begin(), scores.begin() + 5), std::length_error);
}


} /* namespace labynkyr */



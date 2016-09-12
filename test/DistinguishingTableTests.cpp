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
 * DistinguishingTableTests.cpp
 *
 */

#include "src/labynkyr/DistinguishingTable.hpp"

#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

namespace labynkyr {

TEST(DistinguishingTable_mapToWeight_4bits_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(2 * vectorSize);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(-5.0, 5.0);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, double> table(scores);
	table.translateVectorsToPositive();
	table.normaliseDistinguishingVectors();
	table.takeLogarithm(2.0);
	table.applyAbsoluteValue();

	uint32_t const precisionBits = 4;
	auto weightTable = table.mapToWeight<uint32_t>(precisionBits);
	std::vector<uint32_t> const & weights = weightTable->allWeights();

	uint32_t const maxScore = *std::max_element(weights.begin(), weights.end());
	CHECK(maxScore < 16);
}

TEST(DistinguishingTable_mapToWeight_12bits_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(2 * vectorSize);

	// Generate random data
	std::mt19937 generator(521);
	std::uniform_real_distribution<double> distribution(-15.0, 2.0);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, double> table(scores);
	table.translateVectorsToPositive();
	table.normaliseDistinguishingVectors();
	table.takeLogarithm(2.0);
	table.applyAbsoluteValue();

	uint32_t const precisionBits = 12;
	auto weightTable = table.mapToWeight<uint32_t>(precisionBits);
	std::vector<uint32_t> const & weights = weightTable->allWeights();

	uint32_t const maxScore = *std::max_element(weights.begin(), weights.end());
	CHECK(maxScore < 4096);
}

TEST(DistinguishingTable_mapToWeight_4bits_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(2 * vectorSize);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, float> table(scores);
	table.translateVectorsToPositive();
	table.normaliseDistinguishingVectors();
	table.takeLogarithm(2.0f);
	table.applyAbsoluteValue();

	uint32_t const precisionBits = 4;
	auto weightTable = table.mapToWeight<uint32_t>(precisionBits);
	std::vector<uint32_t> const & weights = weightTable->allWeights();

	uint32_t const maxScore = *std::max_element(weights.begin(), weights.end());
	CHECK(maxScore < 16);
}

TEST(DistinguishingTable_mapToWeight_12bits_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(2 * vectorSize);

	// Generate random data
	std::mt19937 generator(521);
	std::uniform_real_distribution<float> distribution(-15.0f, 2.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, float> table(scores);
	table.translateVectorsToPositive();
	table.normaliseDistinguishingVectors();
	table.takeLogarithm(2.0f);
	table.applyAbsoluteValue();

	uint32_t const precisionBits = 12;
	auto weightTable = table.mapToWeight<uint32_t>(precisionBits);
	std::vector<uint32_t> const & weights = weightTable->allWeights();

	uint32_t const maxScore = *std::max_element(weights.begin(), weights.end());
	CHECK(maxScore < 4096);
}

TEST(DistinguishingTable_normaliseDistinguishingVectors_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(-5.0, 5.0);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, double> table(scores);
	table.normaliseDistinguishingVectors();

	// Actual
	double const sum1 = std::accumulate(table.allScores().begin(), table.allScores().begin() + vectorSize, 0.0);
	double const sum2 = std::accumulate(table.allScores().begin()  + vectorSize, table.allScores().end(), 0.0);
	CHECK_CLOSE(1.0, sum1, 0.0001);
	CHECK_CLOSE(1.0, sum2, 0.0001);
}

TEST(DistinguishingTable_normaliseDistinguishingVectors_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, float> table(scores);
	table.normaliseDistinguishingVectors();

	// Actual
	float const sum1 = std::accumulate(table.allScores().begin(), table.allScores().begin() + vectorSize, 0.0f);
	float const sum2 = std::accumulate(table.allScores().begin()  + vectorSize, table.allScores().end(), 0.0f);
	CHECK_CLOSE(1.0f, sum1, 0.0001f);
	CHECK_CLOSE(1.0f, sum2, 0.0001f);
}

TEST(DistinguishingTable_applyAbsoluteValue_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(-5.0, 5.0);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, double> table(scores);
	table.applyAbsoluteValue();

	double const minElement = *std::min_element(table.allScores().begin(), table.allScores().end());
	double const maxElement = *std::max_element(table.allScores().begin(), table.allScores().end());

	CHECK(minElement >= 0.0);
	CHECK(maxElement <= 5.0);
}

TEST(DistinguishingTable_applyAbsoluteValue_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, float> table(scores);
	table.applyAbsoluteValue();

	float const minElement = *std::min_element(table.allScores().begin(), table.allScores().end());
	float const maxElement = *std::max_element(table.allScores().begin(), table.allScores().end());

	CHECK(minElement >= 0.0f);
	CHECK(maxElement <= 5.0f);
}

TEST(DistinguishingTable_translateVectorsToPositive_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(-5.0, 5.0);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, double> table(scores);
	table.translateVectorsToPositive();

	double const minElement = *std::min_element(table.allScores().begin(), table.allScores().end());
	double const maxElement = *std::max_element(table.allScores().begin(), table.allScores().end());

	CHECK_CLOSE(0.0, minElement, 0.00001);
	CHECK(maxElement > 9.0);
}

TEST(DistinguishingTable_translateVectorsToPositive_alreadyPositive_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(1.0, 5.0);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, double> table(scores);
	table.translateVectorsToPositive();

	double const minElement = *std::min_element(table.allScores().begin(), table.allScores().end());
	double const maxElement = *std::max_element(table.allScores().begin(), table.allScores().end());

	CHECK(minElement >= 1.0);
	CHECK(maxElement <= 5.0);
}

TEST(DistinguishingTable_translateVectorsToPositive_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, float> table(scores);
	table.translateVectorsToPositive();

	float const minElement = *std::min_element(table.allScores().begin(), table.allScores().end());
	float const maxElement = *std::max_element(table.allScores().begin(), table.allScores().end());

	CHECK_CLOSE(0.0f, minElement, 0.00001f);
	CHECK(maxElement > 9.0f);
}

TEST(DistinguishingTable_translateVectorsToPositive_alreadyPositive_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(1.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, float> table(scores);
	table.translateVectorsToPositive();

	float const minElement = *std::min_element(table.allScores().begin(), table.allScores().end());
	float const maxElement = *std::max_element(table.allScores().begin(), table.allScores().end());

	CHECK(minElement >= 1.0f);
	CHECK(maxElement <= 5.0f);
}

TEST(DistinguishingTable_takeLogarithm_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(1.0, 5.0);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	std::vector<double> expected(scores);
	for(uint32_t index = 0 ; index < 2 * vectorSize ; index++) {
		expected[index] = std::log(scores[index]) / std::log(2.0);
	}

	DistinguishingTable<2, 8, double> table(scores);
	table.takeLogarithm(2.0);

	CHECK_ARRAY_CLOSE(expected, table.allScores(), expected.size(), 0.0001);
}

TEST(DistinguishingTable_takeLogarithm_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(1.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	std::vector<float> expected(scores);
	for(uint32_t index = 0 ; index < 2 * vectorSize ; index++) {
		expected[index] = std::log(scores[index]) / std::log(2.0f);
	}

	DistinguishingTable<2, 8, float> table(scores);
	table.takeLogarithm(2.0f);

	CHECK_ARRAY_CLOSE(expected, table.allScores(), expected.size(), 0.0001f);
}

TEST(DistinguishingTable_copyConstructor_double) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<double> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(1.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, double> other(scores);
	DistinguishingTable<2, 8, double> table(other);

	CHECK_ARRAY_CLOSE(scores, table.allScores(), scores.size(), 0.0001f);
}

TEST(DistinguishingTable_copyConstructor_single) {
	uint64_t const vectorSize = 1UL << 8;
	std::vector<float> scores(vectorSize * 2);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(1.0f, 5.0f);
	std::generate(scores.begin(), scores.end(), [&generator, &distribution]{ return distribution(generator); });

	DistinguishingTable<2, 8, float> other(scores);
	DistinguishingTable<2, 8, float> table(other);

	CHECK_ARRAY_CLOSE(scores, table.allScores(), scores.size(), 0.0001f);
}

TEST(DistinguishingTable_score_double) {
	std::vector<double> const scores = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
	DistinguishingTable<2, 2, double> table(scores);

	CHECK_EQUAL(1.1, table.score(0, 0));
	CHECK_EQUAL(2.2, table.score(0, 1));
	CHECK_EQUAL(3.3, table.score(0, 2));
	CHECK_EQUAL(4.4, table.score(0, 3));
	CHECK_EQUAL(5.5, table.score(1, 0));
	CHECK_EQUAL(6.6, table.score(1, 1));
	CHECK_EQUAL(7.7, table.score(1, 2));
	CHECK_EQUAL(8.8, table.score(1, 3));
}

} /* namespace labynkyr */

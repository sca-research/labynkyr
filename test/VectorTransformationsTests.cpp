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
 * VectorTransformationsTests.cpp
 *
 */

#include "src/labynkyr/VectorTransformations.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

namespace labynkyr {

TEST(VectorTransformations_kahanSummation_double) {
	uint64_t const size = 65536;
	std::vector<double> data(size);
	std::fill(data.begin(), data.end(), 1.1);

	double const actual = VectorTransformations<double>::kahanSummation(data.cbegin(), data.cend());
	CHECK_CLOSE(72089.6, actual, 0.0001);
}

TEST(VectorTransformations_kahanSummation_float) {
	uint64_t const size = 65536;
	std::vector<float> data(size);
	std::fill(data.begin(), data.end(), 1.1f);

	float const actual = VectorTransformations<float>::kahanSummation(data.cbegin(), data.cend());
	CHECK_CLOSE(72089.6f, actual, 0.0001f);
}

TEST(VectorTransformations_normalise_double) {
	uint64_t const size = 65536;
	std::vector<double> data(size);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(0.0, 1.0);
	std::generate(data.begin(), data.end(), [&generator, &distribution]{ return distribution(generator); });

	// Normalise
	VectorTransformations<double>::normalise(data.begin(), data.end());

	// Actual
	double sum = 0.0;
	double sumC = 0.0;
	std::for_each(
		data.begin(), data.end(),
		[&sum, &sumC] (double const score) {
			double const sumY = score - sumC;
			double const sumT = sum + sumY;
			sumC = (sumT - sum) - sumY;
			sum = sumT;
		}
	);
	CHECK_CLOSE(1.0, sum, 0.0001);
}

TEST(VectorTransformations_normalise_float) {
	uint64_t const size = 65536;
	std::vector<float> data(size);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	std::generate(data.begin(), data.end(), [&generator, &distribution]{ return distribution(generator); });

	// Normalise
	VectorTransformations<float>::normalise(data.begin(), data.end());

	// Actual
	float sum = 0.0f;
	float sumC = 0.0f;
	std::for_each(
		data.begin(), data.end(),
		[&sum, &sumC] (float const score) {
			float const sumY = score - sumC;
			float const sumT = sum + sumY;
			sumC = (sumT - sum) - sumY;
			sum = sumT;
		}
	);
	CHECK_CLOSE(1.0f, sum, 0.0001f);
}

TEST(VectorTransformations_absoluteValue_double) {
	uint64_t const size = 65536;
	std::vector<double> data(size);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(-1.0, 1.0);
	std::generate(data.begin(), data.end(), [&generator, &distribution]{ return distribution(generator); });

	// Expected
	std::vector<double> expected(data);
	std::transform(data.begin(), data.end(), expected.begin(), [](double const & value) { return std::fabs(value); });

	VectorTransformations<double>::absoluteValue(data.begin(), data.end());
	CHECK_ARRAY_CLOSE(expected, data, expected.size(), 0.001);
}

TEST(VectorTransformations_absoluteValue_float) {
	uint64_t const size = 65536;
	std::vector<float> data(size);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::generate(data.begin(), data.end(), [&generator, &distribution]{ return distribution(generator); });

	// Expected
	std::vector<float> expected(data);
	std::transform(data.begin(), data.end(), expected.begin(), [](float const & value) { return std::fabs(value); });

	VectorTransformations<float>::absoluteValue(data.begin(), data.end());
	CHECK_ARRAY_CLOSE(expected, data, expected.size(), 0.001f);
}

TEST(VectorTransformations_logarithm_double) {
	uint64_t const size = 65536;
	std::vector<double> data(size);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<double> distribution(0.01, 1.0);
	std::generate(data.begin(), data.end(), [&generator, &distribution]{ return distribution(generator); });

	// Expected
	double const base = 2.0;
	std::vector<double> expected(data);
	for(uint32_t index = 0 ; index < size ; index++) {
		expected[index] = std::log(data[index]) / std::log(base);
	}

	VectorTransformations<double>::logarithm(data.begin(), data.end(), base);
	CHECK_ARRAY_CLOSE(expected, data, expected.size(), 0.001);
}

TEST(VectorTransformations_logarithm_float) {
	uint64_t const size = 65536;
	std::vector<float> data(size);

	// Generate random data
	std::mt19937 generator(5);
	std::uniform_real_distribution<float> distribution(0.01f, 1.0f);
	std::generate(data.begin(), data.end(), [&generator, &distribution]{ return distribution(generator); });

	// Expected
	float const base = 2.0;
	std::vector<float> expected(data);
	for(uint32_t index = 0 ; index < size ; index++) {
		expected[index] = std::log(data[index]) / std::log(base);
	}

	VectorTransformations<float>::logarithm(data.begin(), data.end(), base);
	CHECK_ARRAY_CLOSE(expected, data, expected.size(), 0.001f);
}

} /* namespace labynkyr */



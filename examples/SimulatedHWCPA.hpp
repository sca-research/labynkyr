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
 * SimulatedHWCPA.hpp
 *
 */

#ifndef LABYNKYR_EXAMPLES_SIMULATEDHWCPA_HPP_
#define LABYNKYR_EXAMPLES_SIMULATEDHWCPA_HPP_

#include "labynkyr/DistinguishingTable.hpp"

#include <math.h>
#include <stdint.h>

#include <algorithm>
#include <memory>
#include <random>
#include <vector>

namespace labynkyr {

/**
 *
 * SimulatedHWCPA can be used to generate correlation DPA attack results for synthetic simulated information leakage on the output
 * of the first 8-bit AES SubBytes operation.
 *
 * Leakage is simulated under the `standard' DPA model as in:
 *
 * 		S. Mangard, E. Oswald, and F.-X. Standaert.
 * 		One for All – All for One: Unifying Standard DPA Attacks.
 * 		IET Information Security, 5(2):100–110, 2011
 *
 * The simulated information leakage takes the form:
 *
 * 		leakage = HammingWeight(SubBytes(k XOR p)) + noise,
 *
 * where k is a byte of a fixed 128-bit AES key, p is a byte of a randomly generated plaintext, SubBytes is the application of the 8-bit
 * AES SBox, and noise is sampled from a Gaussian distribution with zero mean and a specified standard deviation.
 *
 * In this scenario we generate a set of $n$ 16 byte plaintexts uniformly at random. We then define an attacker that uses the Hamming-weight
 * power model to do a divide-and-conquer style DPA attack on each of the 16 bytes of the key.  The attacker uses the absolute value of the
 * Pearson's correlation coefficient as the distinguisher.  This produces a distinguishing table containing 16x256 distinguishing scores.
 *
 * This is an idealised scenario in which each of the 16 DPA attacks is independent of the others, and where the attacker has a perfect power
 * model.
 */
class SimulatedHWCPA {
public:
	/**
	 *
	 * Construct a new simulator
	 *
	 * @param key the bytes of the fixed AES-128 encryption key
	 * @param traceCount the number of traces to simulate data for in the DPA attack
	 * @param snr the signal-to-noise ratio is defined as SNR = Var(signal) / Var(Noise).  In this 8-bit Hamming-weight leakage
	 * setting, the signal variance is 2.0 under a uniform plaintext distribution given a fixed key.  The variance of the additive
	 * Gaussian noise is derived by re-arranging this equation.  Reporting SNRs is much more meaningful than reporting variance of
	 * the Gaussian distribution itself!
	 * @param rngSeed used to generate the random plaintexts and noise. If you supply the same value here, you'll get the same set
	 * of simulated traces.
	 */
	SimulatedHWCPA(std::vector<uint8_t> const & key, uint64_t traceCount, double snr, uint64_t rngSeed)
	: key(key)
	, traceCount(traceCount)
	, snr(snr)
	, generator(rngSeed)
	, normalDistribution(0.0, std::sqrt(2.0 / snr))
	, uniformDistribution(0, 255)
	{
	}

	/**
	 *
	 * Simulate new traces and run a new attack
	 *
	 * @return the distinguishing table produced
	 */
	std::unique_ptr<DistinguishingTable<16, 8, double>> nextRandomAttack() {
		// Generate next random plaintexts
		std::vector<uint8_t> allPlaintextBytes(traceCount * 16);
		std::generate(
			allPlaintextBytes.begin(),
			allPlaintextBytes.end(),
			[&]() {
				return uniformDistribution(generator);
			}
		);

		// Generate next trace values
		// traces_for_byte_0||traces_for_byte_1||...||traces_for_byte_15
		std::vector<double> allTraceSamples(traceCount * 16);
		for(uint32_t byteIndex = 0 ; byteIndex < 16 ; byteIndex++) {
			for(uint32_t traceIndex = 0 ; traceIndex < traceCount ; traceIndex++) {
				uint8_t const plaintextByte = allPlaintextBytes[byteIndex * traceCount + traceIndex];
				uint8_t const intermediateValue = sBox(plaintextByte ^ key[byteIndex]);
				double const leakage = hammingWeight(intermediateValue);
				double const noise = normalDistribution(generator);
				allTraceSamples[byteIndex * traceCount + traceIndex] = leakage + noise;
			}
		}

		// Prepare distinguishing table scores
		std::vector<double> scores(256 * 16);
		for(uint32_t byteIndex = 0 ; byteIndex < 16 ; byteIndex++) {
			// Generate hypothetical values for this byte
			for(uint32_t subkey = 0 ; subkey < 256 ; subkey++) {
				std::vector<double> hypValues(traceCount);
				for(uint32_t traceIndex = 0 ; traceIndex < traceCount ; traceIndex++) {
					uint8_t const plaintextByte = allPlaintextBytes[byteIndex * traceCount + traceIndex];
					uint64_t const intermediateValue = sBox(plaintextByte ^ static_cast<uint8_t>(subkey));
					hypValues[traceIndex] = hammingWeight(intermediateValue);
				}
				// Now correlate
				double const corr = pearsonsCorrelation(
					allTraceSamples.begin() + (byteIndex * traceCount),
					allTraceSamples.begin() + (byteIndex * traceCount) + traceCount,
					hypValues.begin(),
					hypValues.end()
				);
				// Store abs(correlation) as we don't care about directionality in DPA
				scores[byteIndex * 256 + subkey] = std::fabs(corr);
			}
		}

		DistinguishingTable<16, 8, double> * table = new DistinguishingTable<16, 8, double>(scores);
		return std::unique_ptr<DistinguishingTable<16, 8, double>>(table);
	}

	/**
	 *
	 * @return the bytes of the fixed key
	 */
	std::vector<uint8_t> const & keyBytes() const {
		return key;
	}

	uint64_t getTraceCount() const {
		return traceCount;
	}

	double getSNR() const {
		return snr;
	}
private:
	std::vector<uint8_t> const & key;
	uint64_t const traceCount;
	double const snr;
	std::mt19937 generator;
	std::normal_distribution<double> normalDistribution;
	std::uniform_int_distribution<uint8_t> uniformDistribution;

	/**
	 *
	 * @param xBegin
	 * @param xEnd
	 * @param yBegin
	 * @param yEnd
	 * @return the Pearson's correlation coefficient evaluated using the samples defined by the x and y iterators
	 */
	double pearsonsCorrelation(
			std::vector<double>::const_iterator xBegin,
			std::vector<double>::const_iterator xEnd,
			std::vector<double>::const_iterator yBegin,
			std::vector<double>::const_iterator yEnd
		) {
		uint64_t const size = (uint64_t) std::distance(xBegin, xEnd);
		double x = 0.0;
		double x2 = 0.0;
		double y = 0.0;
		double y2 = 0.0;
		double xy = 0.0;
		for(uint64_t index = 0 ; index < size ; index++) {
			x += xBegin[index];
			x2 += (xBegin[index] * xBegin[index]);
			y += yBegin[index];
			y2 += (yBegin[index] * yBegin[index]);
			xy += (xBegin[index] * yBegin[index]);
		}
		double const n = static_cast<double>(size);
		double const xMean = x / n;
		double const yMean = y / n;
		double const rNumerator = xy - (n * xMean * yMean);
		double const rDenominator = std::sqrt(x2 - (n * xMean * xMean)) * std::sqrt(y2 - (n * yMean * yMean));
		return rNumerator / rDenominator;
	}

	/**
	 *
	 * @return the Hamming weight of a 64 bit number
	 */
	double hammingWeight(uint64_t value) {
		uint64_t hw = value;
		hw -= (hw >> 1) & 0x5555555555555555;
		hw = (hw & 0x3333333333333333) + ((hw >> 2) & 0x3333333333333333);
		hw = (hw + (hw >> 4)) & 0x0f0f0f0f0f0f0f0f;
		hw = (hw * 0x0101010101010101) >> 56;
		return static_cast<double>(hw);
	}

	// AES SBox lookup table
	static const uint8_t sBox(uint8_t index) {
		static const uint8_t a[] = {
			0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
			0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
			0xCA, 0x82,	0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
			0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
			0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
			0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
			0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
			0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27,	0xB2, 0x75,
			0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
			0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
			0x53, 0xD1, 0x00, 0xED,	0x20, 0xFC, 0xB1, 0x5B,
			0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
			0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
			0x45, 0xF9,	0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
			0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
			0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
			0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
			0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
			0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A,	0x90, 0x88,
			0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
			0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
			0xC2, 0xD3, 0xAC, 0x62,	0x91, 0x95, 0xE4, 0x79,
			0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
			0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
			0xBA, 0x78,	0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
			0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
			0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
			0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
			0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
			0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55,	0x28, 0xDF,
			0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
			0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
		};
		return a[index];
	}
};

} /* namespace labynkyr */

#endif /* LABYNKYR_EXAMPLES_SIMULATEDHWCPA_HPP_ */

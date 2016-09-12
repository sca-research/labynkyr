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
 * RankExamples.hpp
 *
 */

#ifndef LABYNKYR_LABYNKYR_EXAMPLES_RANKEXAMPLES_HPP_
#define LABYNKYR_LABYNKYR_EXAMPLES_RANKEXAMPLES_HPP_

#include "src/labynkyr/rank/PathCountRank.hpp"
#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/BigReal.hpp"
#include "src/labynkyr/DistinguishingTable.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include "examples/SampleDistinguishingTables.hpp"

#include <stdint.h>

#include <iostream>

namespace labynkyr {

template<typename RealType>
class RankExamples {
public:
	RankExamples() {}

	~RankExamples() {}

	/**
	 *
	 * Simulated HW-CPA on AES-128 encryption.
	 *
	 * Correct key : 0x000102030405060708090A0B0C0D0E0F
	 * Estimated rank at 15 bits of precision: 2^43.796
	 *
	 * @param precisionBits the bits of precision to retain during the conversion of correlation coefficients to integer weights
	 */
	void runExample1(uint32_t precisionBits) const {
		DistinguishingTable<16, 8, double> dt = SampleDistinguishingTables::scores_example_1();
		// Transformations necessary prior to conversion to integer weights
		dt.takeLogarithm();
		dt.applyAbsoluteValue();

		auto weights = dt.mapToWeight<uint32_t>(precisionBits);
		Key<128> const key("000102030405060708090A0B0C0D0E0F");
		BigInt<128> const rank = rank::PathCountRank<16, 8, uint32_t>::rank(key, *weights.get());
		auto const log2Rank = BigRealTools::log2<128, 100>(rank);

		printRank(precisionBits, rank, log2Rank);
	}

	/**
	 *
	 * Simulated HW-CPA on AES-128 encryption.
	 *
	 * Correct key : 0x000102030405060708090A0B0C0D0E0F
	 * Estimated rank at 15 bits of precision: 2^106.687
	 *
	 * @param precisionBits the bits of precision to retain during the conversion of correlation coefficients to integer weights
	 */
	void runExample2(uint32_t precisionBits) const {
		auto dt = SampleDistinguishingTables::scores_example_2();
		// Transformations necessary prior to conversion to integer weights
		dt.takeLogarithm();
		dt.applyAbsoluteValue();

		auto weights = dt.mapToWeight<uint32_t>(precisionBits);
		Key<128> const key("000102030405060708090A0B0C0D0E0F");
		BigInt<128> const rank = rank::PathCountRank<16, 8, uint32_t>::rank(key, *weights.get());
		auto const log2Rank = BigRealTools::log2<128, 100>(rank);

		printRank(precisionBits, rank, log2Rank);
	}

	/**
	 *
	 * Simulated HW-CPA on AES-128 encryption.
	 *
	 * Correct key : 0x000102030405060708090A0B0C0D0E0F
	 * Estimated rank at 15 bits of precision: 2^5.08735
	 *
	 * @param precisionBits the bits of precision to retain during the conversion of correlation coefficients to integer weights
	 */
	void runExample3(uint32_t precisionBits) const {
		auto dt = SampleDistinguishingTables::scores_example_3();
		// Transformations necessary prior to conversion to integer weights
		dt.takeLogarithm();
		dt.applyAbsoluteValue();

		auto weights = dt.mapToWeight<uint32_t>(precisionBits);
		Key<128> const key("000102030405060708090A0B0C0D0E0F");
		BigInt<128> const rank = rank::PathCountRank<16, 8, uint32_t>::rank(key, *weights.get());
		auto const log2Rank = BigRealTools::log2<128, 100>(rank);

		printRank(precisionBits, rank, log2Rank);
	}
private:
	static const uint32_t logRankDP = 6;

	void printRank(uint32_t precisionBits, BigInt<128> rank, double logRank) const {
		std::cout << "Estimated rank at " << precisionBits << " bits of precision = 2^"
			<< std::fixed << std::setprecision(logRankDP) << logRank << " (" << std::dec << rank << ")" << std::endl;
	}
};

} /* namespace labynkyr */

#endif /* LABYNKYR_EXAMPLES_RANKEXAMPLES_HPP_ */

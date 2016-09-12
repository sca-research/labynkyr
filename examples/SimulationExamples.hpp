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
 * SimulationExamples.hpp
 *
 */

#ifndef LABYNKYR_EXAMPLES_SIMULATIONEXAMPLES_HPP_
#define LABYNKYR_EXAMPLES_SIMULATIONEXAMPLES_HPP_

#include "examples/SimulatedHWCPA.hpp"

#include "labynkyr/rank/ApproximateRank.hpp"
#include "labynkyr/rank/PathCountRank.hpp"
#include "labynkyr/search/parallel/PEUPool.hpp"
#include "labynkyr/search/parallel/WorkScheduler.hpp"
#include "labynkyr/search/verify/AES128NIEncryptUnrolledKeyVerifier.hpp"
#include "labynkyr/search/EffortAllocation.hpp"
#include "labynkyr/search/SearchSpec.hpp"
#include "labynkyr/BigInt.hpp"
#include "labynkyr/BigReal.hpp"
#include "labynkyr/DistinguishingTable.hpp"
#include "labynkyr/Key.hpp"
#include "labynkyr/VectorTransformations.hpp"
#include "labynkyr/WeightTable.hpp"

#include <math.h>
#include <stdint.h>

#include <chrono>
#include <iostream>
#include <functional>
#include <memory>
#include <vector>

namespace labynkyr {

class SimulationExamples {
public:
	/**
	 *
	 * @param simulatedCPA an oracle for simulating CPA attacks
	 */
	SimulationExamples(SimulatedHWCPA & simulatedCPA)
	: simulatedCPA(simulatedCPA)
	{
		// Create an initial attack
		runNewAttack();
	}

	/**
	 *
	 * Call to simulate a new set of distinguishing vectors
	 */
	void runNewAttack() {
		scoresTable = simulatedCPA.nextRandomAttack();
	}

	/**
	 *
	 * Prints an approximation of the key rank using the "old-style" method of multiplying through the individual subkey ranks.
	 * This method was the best available prior to the first rank estimation algorithm described in:
	 * 		Veyrat-Charvillon, B. Gérard, and F.-X. Standaert.
	 * 		Security Evaluations beyond Computing Power.
	 * 		In T. Johansson and P. Q. Nguyen, editors, EUROCRYPT, volume 7881 of LNCS, pages 126–141. Springer, 2013
	 */
	void approximateRank() {
		Key<128> const key(simulatedCPA.keyBytes());
		BigInt<128> const approxRank = rank::ApproximateRank<16, 8, double>::rank<std::greater<double>>(
			*scoresTable.get(),
			key,
			std::greater<double>()
		);
		double const logApproxRank = BigRealTools::log2<128, 100>(approxRank);
		std::cout << "Old-style multiplication of subkey ranks estimated rank = 2^"
			<< std::fixed << std::setprecision(logRankDP) << logApproxRank << " (" << std::dec << approxRank << ")" << std::endl;
	}

	/**
	 *
	 * Estimate the rank of the current DPA attack using the path count rank algorithm.
	 *
	 * @param precision the bits of precision to retain in the conversion of distinguishing scores to integer weights
	 * @tparam WeightType the integer type used to store the integer weight values
	 */
	template<typename WeightType>
	void rank(uint32_t precision) {
		// Copy scores table and prepare for weight conversion
		DistinguishingTable<16, 8, double> table(*scoresTable.get());
		table.takeLogarithm(2.0);
		table.applyAbsoluteValue();

		// Convert weights
		auto weightTable = table.template mapToWeight<uint32_t>(precision);

		// Rank
		Key<128> const key(simulatedCPA.keyBytes());
		BigInt<128> const rank = rank::PathCountRank<16, 8, WeightType>::rank(key, *weightTable.get());
		double const logRank = BigRealTools::log2<128, 100>(rank);

		std::cout << "Estimated rank at " << precision << " bits of precision = 2^"
			<< std::fixed << std::setprecision(logRankDP) << logRank << " (" << std::dec << rank << ")" << std::endl;
	}

	/**
	 *
	 * Search for the correct key using the current DPA attack results and the path count search algorithm.
	 *
	 * @param precision the bits of precision to retain in the conversion of distinguishing scores to integer weights
	 * @param peuCount use this many parallel execution units
	 * @param budgetBits the total number of the most likely keys that will be searched is 2^budgetBits.  If this is less than the
	 * estimated rank of the key, the key is not likely to be found
	 * @param preferredTaskSizeBits the preferred number of keys to search in each sequential task will be 2^preferredTaskSizeBits.
	 * @tparam WeightType the integer type used to store the integer weight values
	 */
	template<typename WeightType>
	void search(uint32_t precision, uint32_t peuCount, uint32_t totalEffortBits, uint32_t preferredJobSizeBits) {
		// Copy scores table and prepare for weight conversion
		DistinguishingTable<16, 8, double> table(*scoresTable.get());
		table.takeLogarithm(2.0);
		table.applyAbsoluteValue();

		// Convert weights at the specified precision
		auto weightTable = table.template mapToWeight<uint32_t>(precision);

		// Setup verifiers -- use AES-NI
		std::vector<uint8_t> const plaintext = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
		std::vector<uint8_t> const ciphertext = {0x0a, 0x94, 0x0b, 0xb5, 0x41, 0x6e, 0xf0, 0x45, 0xf1, 0xc3, 0x94, 0x58, 0xc6, 0x53, 0xea, 0x5a};
		search::AES128NIEncryptUnrolledKeyVerifierFactory verifierFactory(plaintext, ciphertext);

		search::PEUPool<16, 8, WeightType, uint8_t> peuPool(peuCount, verifierFactory, peuCount, 1000UL);

		search::SearchSpecBuilder<128> searchSpecBuilder(totalEffortBits);
		auto const searchSpec = searchSpecBuilder.createSpec();
		search::EffortAllocation<16, 8, WeightType> effort(searchSpec, *weightTable.get(), preferredJobSizeBits);

		search::WorkScheduler<16, 8, WeightType, uint8_t> scheduler(1000UL);;
		scheduler.runSearch(peuPool, effort);

		// Print results
		bool const keyFound = peuPool.isKeyFound();
		if(keyFound) {
			std::cout << "Found key = 0x";
			Key<128> const correctKey = peuPool.correctKey();
			for(auto & keyByte : correctKey.asBytes()) {
				std::cout << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<uint32_t>(keyByte);
			}
			auto const keyFoundDuration = scheduler.getLastTimeTakenToFindKey();
			std::cout << " in " << std::chrono::duration<double>(keyFoundDuration).count() << " seconds." << std::endl;
		} else {
			auto const overallDuration = scheduler.getLastTotalTimeTaken();
			std::cout << "Key not found.  Total time = " << std::chrono::duration<double>(overallDuration).count() << " seconds." << std::endl;
		}
		uint64_t const keysChecked = peuPool.keysVerified();
		double const keysCheckedLog2 = std::log(static_cast<double>(keysChecked)) / std::log(2.0);
		std::cout << "Actual keys checked: "
			<< " 2^" << std::fixed << std::setprecision(logRankDP) << keysCheckedLog2 << " (" << std::dec << keysChecked << ")" << std::endl;
	}
private:
	SimulatedHWCPA & simulatedCPA;
	std::unique_ptr<DistinguishingTable<16, 8, double>> scoresTable;

	static const uint32_t logRankDP = 3;
};

} /* namespace labynkyr */

#endif /* LABYNKYR_EXAMPLES_SIMULATIONEXAMPLES_HPP_ */

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
 * SearchExamples.hpp
 *
 */

#ifndef LABYNKYR_EXAMPLES_SEARCHEXAMPLES_HPP_
#define LABYNKYR_EXAMPLES_SEARCHEXAMPLES_HPP_

#include "src/labynkyr/search/parallel/PEUPool.hpp"
#include "src/labynkyr/search/parallel/WorkScheduler.hpp"
#include "src/labynkyr/search/verify/AES128NIEncryptUnrolledKeyVerifier.hpp"
#include "src/labynkyr/search/verify/ComparisonKeyVerifier.hpp"
#include "src/labynkyr/search/EffortAllocation.hpp"
#include "src/labynkyr/search/SearchSpec.hpp"
#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include "examples/SampleWeightTables.hpp"

#include <stdint.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

namespace labynkyr {

/**
 *
 * The three example enumerations described below use pre-constructed weight tables (defined in SampleWeightTables.hpp) constructed
 * using real distinguishing scores produced by a correlation DPA attack on an AES-128 key used by hardware in a Beaglebone Black device.
 * Full details of the attack can be found in the original CHES 2015 paper:
 *
 * 		Jake Longo, Elke De Mulder, Dan Page, Michael Tunstall
 *		SoC It to EM: ElectroMagnetic Side-Channel Attacks on a Complex System-on-Chip.
 *		CHES 2015: 620-640
 *
 * The examples only differ in the number of traces used.  Each attack is imperfect, and requires enumeration/search to find the correct
 * key.  In each case the correct AES-128 key is 0xde3aa9d70fc25dfcd4dddec804ae5d1c
 *
 * [Example 1]
 * 			  Traces : 55,000
 * 	Est. rank of key : 2^18.0598 or 273,236
 *
 * [Example 2]
 * 			  Traces : 49,000
 * 	Est. rank of key : 2^30.0733 or 1,129,728,524
 *
 * [Example 3]
 * 			  Traces : 45,750
 * 	Est. rank of key : 2^34.5170 or 24,584,138,996
 */
template<typename WeightType>
class SearchExamples {
public:
	/**
	 *
	 * @param peuCount use this many parallel execution units
	 * @param budgetBits the total number of the most likely keys that will be searched is 2^budgetBits.  If this is < 2^30, the key
	 * will not be found
	 * @param preferredTaskSizeBits the preferred number of keys to search in each sequential task will be 2^preferredTaskSizeBits.
	 */
	SearchExamples(uint32_t peuCount, uint32_t budgetBits, uint32_t preferredTaskSizeBits)
	: peuCount(peuCount)
	, budgetBits(budgetBits)
	, preferredTaskSizeBits(preferredTaskSizeBits)
	{
	}

	~SearchExamples() {}

	/**
	 *
	 * Run search example #1.
	 *
	 * Traces		 : 55000
	 * Precision	 : 15
	 * Rank (2^x)	 : 18.0598
	 * Rank			 : 273236
	 * Weight of key : 42419
	 */
	void runExample1() const {
		std::cout << "Running search example 1" << std::endl;
		std::cout << " + Traces           : 55000" << std::endl;
		std::cout << " + Precision (bits) : 15" << std::endl;
		std::cout << " + Rank (2^x)       : 18.0598" << std::endl;
		std::cout << " + Rank             : 273236" << std::endl;
		std::cout << " + Weight of key    : 42419 " << std::endl;
		std::cout << "----------------------------" << std::endl;
		auto const weightTable = SampleWeightTables::rank_2_18<WeightType>();
		searchUsingWeightTable(weightTable);
	}

	/**
	 *
	 * Run search example #2.
	 *
	 * Traces		 : 49000
	 * Precision	 : 15
	 * Rank (2^x)	 : 30.0733
	 * Rank			 : 1129728524
	 * Weight of key : 42082
	 */
	void runExample2() const {
		std::cout << "Running search example 2" << std::endl;
		std::cout << " + Traces           : 49000" << std::endl;
		std::cout << " + Precision (bits) : 15" << std::endl;
		std::cout << " + Rank (2^x)       : 30.0733" << std::endl;
		std::cout << " + Rank             : 1129728524" << std::endl;
		std::cout << " + Weight of key    : 42082 " << std::endl;
		std::cout << "--------------------------------" << std::endl;
		auto const weightTable = SampleWeightTables::rank_2_30<WeightType>();
		searchUsingWeightTable(weightTable);
	}

	/**
	 *
	 * Run search example #3.
	 *
	 * Traces		 : 45750
	 * Precision	 : 15
	 * Rank (2^x)	 : 34.5170
	 * Rank			 : 24584138996
	 * Weight of key : 37184
	 */
	void runExample3() const {
		std::cout << "Running search example 3" << std::endl;
		std::cout << " + Traces           : 45750" << std::endl;
		std::cout << " + Precision (bits) : 15" << std::endl;
		std::cout << " + Rank (2^x)       : 34.5170" << std::endl;
		std::cout << " + Rank             : 24584138996" << std::endl;
		std::cout << " + Weight of key    : 37184 " << std::endl;
		std::cout << "---------------------------------" << std::endl;
		auto const weightTable = SampleWeightTables::rank_2_34<WeightType>();
		searchUsingWeightTable(weightTable);
	}
private:
	uint32_t const peuCount;
	uint32_t const budgetBits;
	uint32_t const preferredTaskSizeBits;

	void searchUsingWeightTable(WeightTable<16, 8, WeightType> const & weightTable) const {
		using namespace search;

		// Use AES-NI to verify the key candidates
		std::vector<uint8_t> const plaintext = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
		std::vector<uint8_t> const ciphertext = {0xc5, 0x11, 0xb3, 0xb8, 0xe8, 0x2e, 0x57, 0xac, 0x0a, 0xd3, 0x03, 0x19, 0xa7, 0x44, 0x63, 0xa6};
		AES128NIEncryptUnrolledKeyVerifierFactory verifierFactory(plaintext, ciphertext);

		// 1:1 mapping between enumerator and verifier instances
		uint32_t const verifierCount = peuCount;
		PEUPool<16, 8, WeightType, uint8_t> peuPool(peuCount, verifierFactory, verifierCount, 100UL);

		// Aim to search the first 2^budgetBits key candidates
		SearchSpecBuilder<128> const searchSpecBuilder(budgetBits);
		auto const searchSpec = searchSpecBuilder.createSpec();

		// Allocate the required effort for the preferred task sizes of 2^preferredTaskSizeBits
		EffortAllocation<16, 8, WeightType> effort(searchSpec, weightTable, preferredTaskSizeBits);

		// Run the enumeration
		WorkScheduler<16, 8, WeightType, uint8_t> scheduler(100UL);
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
		std::cout << "Actual keys checked: " << " 2 ^ " << keysCheckedLog2 << " (" << std::dec << keysChecked << ")" << std::endl;
	}
};

} /*namespace labynkyr */

#endif /* LABYNKYR_EXAMPLES_SEARCHEXAMPLES_HPP_ */

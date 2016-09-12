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
 * Examples.cpp
 *
 */

#include "examples/RankExamples.hpp"
#include "examples/SearchExamples.hpp"
#include "examples/SimulationExamples.hpp"
#include "examples/SimulatedHWCPA.hpp"

#include <stdint.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

void help() {
	std::cout << "Usage:" << std::endl;
	std::cout << "  1) ./examples rank <precisionBits>" << std::endl;
	std::cout << "  2) ./examples search <example_to_run (1, 2, 3)> <peuCount> <budgetBits> <preferredTaskSizeBits>" << std::endl;
	std::cout << "          e.g ./examples search 1 4 28 22, where" << std::endl;
	std::cout << "            [Example #1] correct key rank is 2^18.0598" << std::endl;
	std::cout << "            [Example #2] correct key rank is 2^30.0733" << std::endl;
	std::cout << "            [Example #3] correct key rank is 2^34.5170" << std::endl;
	std::cout << "  3) ./examples simulate-rank <traceCount> <snr> <rngSeed> <precisionBits>" << std::endl;
	std::cout << "  4) ./examples simulate-search <traceCount> <snr> <rngSeed> <precisionBits> <peuCount> <budgetBits> <preferredTaskSizeBits>" << std::endl;
}

void logParallelSearchConfig(uint32_t peuCount, uint32_t budgetBits, uint32_t preferredTaskSizeBits) {
	std::cout << "Searching using " << peuCount << " PEUs.  Will search up to the 2^" << budgetBits << " most likely key candidates.";
	std::cout << " Each search task will contain at least 2^" << preferredTaskSizeBits << " key candidates." << std::endl;
}

void logSimulatedCPAConfig(labynkyr::SimulatedHWCPA const & simulator) {
	std::cout << "Simulating a Hamming-weight CPA attack on the first SubBytes operation in AES-128 using:" << std::endl;
	std::cout << "   Traces : " << simulator.getTraceCount() << std::endl;
	std::cout << "      SNR : " << simulator.getSNR() << std::endl;
	std::cout << "Traces are simulated using Hamming-weight leakage and additive Gaussian noise."
			<< " Attacker uses CPA and a Hamming-weight power model." << std::endl;
}

/**
 *
 * RANK EXAMPLES
 * ============================================================================================================================
 * See examples/RankExamples.hpp for a basic example of how to construct the functionality necessary to run a rank calculation.
 *
 * There are three example distinguishing tables.  Each contain distinguishing scores produced by a correlation DPA attack
 * on an AES-128 key.
 *
 * Each example uses the same correlation DPA with a Hamming-weight power model.  The information leakage has an SNR of 0.25.
 * The only difference is in the number of traces used:
 * 		(1) Uses 35 traces
 * 		(2) Uses 15 traces
 * 		(3) Uses 90 traces
 *
 * SEARCH EXAMPLES
 * ============================================================================================================================
 * See examples/SearchExamples.hpp for a basic example of how to construct the functionality necessary to run a parallel search
 * calculation.
 *
 * There are three example weight tables available.  Each contains the integer conversion of real distinguishing scores at 15 bits
 * of precision, produced by a correlation DPA attack on an AES-128 key used by hardware in a Beaglebone Black device.  Full
 * details of the attack can be found in the original CHES 2015 paper:
 * 		Jake Longo, Elke De Mulder, Dan Page, Michael Tunstall
 *		SoC It to EM: ElectroMagnetic Side-Channel Attacks on a Complex System-on-Chip.
 *		CHES 2015: 620-640
 *
 * Each example uses the same correlation DPA with a Hamming distance power model, attacking the input and output of the final
 * round of encryption.  The only difference is in the number of traces used:
 * 		(1) Uses 55,000 traces
 * 		(2) Uses 49,000 traces
 * 		(3) Uses 45,750 traces
 *
 * Through program arguments you will provide:
 * 			       peuCount - the number of parallel execution units used.  You can experiment with the effect this has on overall
 * 			       			  performance, but typically selecting the number of physical cores on the host provides the best results.
 * 		         budgetBits - the total number of the most likely keys to aim to search will be taken to be 2^budgetBits
 *    preferredTaskSizeBits - the total number of keys to search within each sequential search task will be taken to be 2^preferredTaskSizeBits
 *
 * Note that the fairly simple concurrency design of labynkyr means that the processing of individual search tasks cannot be interrupted,
 * so if one task finds the key, the time taken to find that key can be reported, but the program must wait for the other search tasks
 * to complete before it can exit.  Consequently, using the total program execution time can be misleading.
 *
 * SIMULATED EXAMPLES
 * ============================================================================================================================
 * See examples/SimulationExamples.hpp for examples of both rank and search calculations using simulated correlation power analysis
 * (CPA) attack results produced by attacking synthetic information leakage.
 *
 * The specifics of the simulated information leakage and attack are described in examples/SimulatedHWCPA.hpp
 *
 * There are two modes available:
 * 		1) ./examples simulate-rank <traceCount> <snr> <rngSeed> <precisionBits>
 * 		2) ./examples simulate-search <traceCount> <snr> <rngSeed> <precisionBits> <peuCount> <budgetBits> <preferredTaskSizeBits>
 *
 * simulate-rank will produce a simulated DPA attack using traceCount traces, where the information leakage will have an SNR of SNR.
 * It will then run an old-style "multiply through subkey ranks" approximation to the actual rank, and the estimated rank using the
 * path count rank algorithm at precisionBits of precision.  The rng seed is supplied to allow the reproduction of the same set of
 * simulated traces over multiple program runs.
 *
 * simulate-search can simulate the same set of information leakage, and will use the DPA attack results to search for keys.  It will
 * use peuCount parallel execution units to search up to the 2^budgetBits most likely key candidates.  Each sequential search task will
 * contain at least 2^preferredTaskSizeBits candidates.
 */
int main(int argc, char* argv[]) {
	if(argc == 3 && (std::string(argv[1])).compare("rank") == 0) {
		std::string const precisionStr(argv[2]);
		uint32_t const precisionBits = std::stoi(precisionStr);

		labynkyr::RankExamples<double> examples;

		std::cout << "[Example 1] SubBytes CPA on AES-128, 35 traces" << std::endl;
		auto const rank1Begin = std::chrono::high_resolution_clock::now();
		examples.runExample1(precisionBits);
		auto const rank1End = std::chrono::high_resolution_clock::now();
		auto const rank1TimeNanos = std::chrono::duration<uint64_t, std::nano>(rank1End - rank1Begin);
		std::cout << "Computation took " << std::fixed << std::setprecision(4) <<
				std::chrono::duration<double>(rank1TimeNanos).count() << " seconds" << std::endl;
		std::cout << std::endl;

		std::cout << "[Example 2] SubBytes CPA on AES-128, 15 traces" << std::endl;
		auto const rank2Begin = std::chrono::high_resolution_clock::now();
		examples.runExample2(precisionBits);
		auto const rank2End = std::chrono::high_resolution_clock::now();
		auto const rank2TimeNanos = std::chrono::duration<uint64_t, std::nano>(rank2End - rank2Begin);
		std::cout << "Computation took " << std::fixed << std::setprecision(4) <<
				std::chrono::duration<double>(rank2TimeNanos).count() << " seconds" << std::endl;
		std::cout << std::endl;

		std::cout << "[Example 3] SubBytes CPA on AES-128, 90 traces" << std::endl;
		auto const rank3Begin = std::chrono::high_resolution_clock::now();
		examples.runExample3(precisionBits);
		auto const rank3End = std::chrono::high_resolution_clock::now();
		auto const rank3TimeNanos = std::chrono::duration<uint64_t, std::nano>(rank3End - rank3Begin);
		std::cout << "Computation took "  << std::fixed << std::setprecision(4) <<
				std::chrono::duration<double>(rank3TimeNanos).count() << " seconds" << std::endl;
	} else if(argc == 6 && (std::string(argv[1])).compare("search") == 0) {
		uint32_t const exampleIndex = std::stoi(std::string(argv[2]));
		uint32_t const peuCount = std::stoi(std::string(argv[3]));
		uint32_t const budgetBits = std::stoi(std::string(argv[4]));
		uint32_t const preferredTaskSizeBits = std::stoi(std::string(argv[5]));

		logParallelSearchConfig(peuCount, budgetBits, preferredTaskSizeBits);
		std::cout << "----------------------" << std::endl;

		labynkyr::SearchExamples<uint32_t> examples(peuCount, budgetBits, preferredTaskSizeBits);
		if(exampleIndex == 1) {
			examples.runExample1();
		} else if(exampleIndex == 2) {
			examples.runExample2();
		} else if(exampleIndex == 3) {
			examples.runExample3();
		} else {
			help();
		}
	} else if(argc == 6 && (std::string(argv[1])).compare("simulate-rank") == 0) {
		// Simulated CPA attack data, with rank calculations
		uint32_t const traceCount = std::stoi(std::string(argv[2]));
		double const snr = std::stod(std::string(argv[3]));
		uint64_t const rngSeed = std::stoi(std::string(argv[4]));
		uint32_t const precision = std::stoi(std::string(argv[5]));

		// A fixed AES-128 key
		std::vector<uint8_t> const key = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
		labynkyr::SimulatedHWCPA simulatedCpa(key, traceCount, snr, rngSeed);
		logSimulatedCPAConfig(simulatedCpa);
		std::cout << "----------------------" << std::endl;

		// Simulate rank calculations
		labynkyr::SimulationExamples simulator(simulatedCpa);
		simulator.approximateRank();
		simulator.rank<uint32_t>(precision);
	} else if(argc == 9 && (std::string(argv[1])).compare("simulate-search") == 0) {
		// Simulated CPA attack data, with search calculations
		uint32_t const traceCount = std::stoi(std::string(argv[2]));
		double const snr = std::stod(std::string(argv[3]));
		uint64_t const rngSeed = std::stoi(std::string(argv[4]));
		uint32_t const precision = std::stoi(std::string(argv[5]));
		uint32_t const peuCount = std::stoi(std::string(argv[6]));
		uint32_t const budgetBits = std::stoi(std::string(argv[7]));
		uint32_t const preferredTaskSizeBits = std::stoi(std::string(argv[8]));

		// A fixed AES-128 key
		std::vector<uint8_t> const key = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
		labynkyr::SimulatedHWCPA simulatedCpa(key, traceCount, snr, rngSeed);
		logSimulatedCPAConfig(simulatedCpa);

		// Run a parallel search calculation
		logParallelSearchConfig(peuCount, budgetBits, preferredTaskSizeBits);
		std::cout << "----------------------" << std::endl;
		labynkyr::SimulationExamples simulator(simulatedCpa);
		simulator.search<uint32_t>(precision, peuCount, budgetBits, preferredTaskSizeBits);
	} else {
		help();
	}
}



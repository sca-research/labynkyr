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
 * EnvironmentManager.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_ENVIRONMENTMANAGER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_ENVIRONMENTMANAGER_HPP_

#include "labynkyr/BigInt.hpp"
#include "labynkyr/BigReal.hpp"

#include <stdint.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

namespace labynkyr {
namespace search {

/**
 *
 * Global singleton instance designed to handle logging and management of parallel search efforts
 */
class EnvironmentManager {
public:
	static uint32_t const sizeDecPlaces = 3;
	static uint32_t const timeDecPlaces = 4;

	/**
	 *
	 * Log the completion of a SearchTaskRunner instance.
	 *
	 * @param taskSize the number of keys verified in the task
	 * @param duration the length of time taken to execute the task
	 * @param methodUsed the enumeration method used
	 * @tparam KeyLenBits
	 */
	template<uint32_t KeyLenBits>
	void logTaskCompletion(BigInt<KeyLenBits> taskSize, std::chrono::duration<uint64_t, std::nano> duration, std::string const & methodUsed) {
		tasksCompleted++;
		if(!suppressLogging) {
			std::stringstream log;
			log << "[INFO] Task " << tasksCompleted << " completed using ";
			log << std::right << std::setw(10) << methodUsed << ". Task size = ";
			log << "2^" << std::fixed << std::setprecision(sizeDecPlaces) << BigRealTools::log2<KeyLenBits, 100>(taskSize);
			log << " (" << taskSize << ")";
			double const seconds = std::chrono::duration<double>(duration).count();
			log << " in " << std::right << std::setw(timeDecPlaces + 2) << std::fixed << std::setprecision(timeDecPlaces) << seconds << " seconds." << std::endl;
			std::cout << log.str();
		}
	}

	/**
	 *
	 * Log that the correct key has been found
	 *
	 * @param keyBytes
	 * @param timeSpentSearching the time taken from the search execution starting up until the key being found
	 */
	void logKeyFound(std::vector<uint8_t> const & keyBytes, std::chrono::duration<uint64_t, std::nano> timeSpentSearching) {
		if(!suppressLogging) {
			std::stringstream log;
			log << "[INFO] Task " << tasksCompleted << " **KEY FOUND**. Key is 0x";
			for(auto & keyByte : keyBytes) {
				log << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<uint32_t>(keyByte);
			}
			log << ". Time to find key = " << std::chrono::duration<double>(timeSpentSearching).count() << " seconds" << std::endl;
			std::cout << log.str();
		}
	}

	/**
	 *
	 * @param suppressLogging if set to true, logging information will not be printed to stdout
	 */
	void setSuppressLogging(bool suppressLogging) {
		this->suppressLogging = suppressLogging;
	}

	/**
	 *
	 * Sets the number of recorded tasks completed to be 0.
	 */
	void resetLogging() {
		tasksCompleted = 0;
	}

	/**
	 *
	 * Attempt to determine the number of logical threads available on the system.  If an answer
	 * can't be found, the value fallbackValue will be returned
	 *
	 * @param fallbackValue
	 * @return
	 */
	uint32_t guessLogicalThreadCount(uint32_t fallbackValue) const {
		uint32_t const reportedCount = logicalThreadCount();
		if(reportedCount == 0) {
			return fallbackValue;
		} else {
			return reportedCount;
		}
	}

	/**
	 *
	 * Attempt to determine the number of logical threads available on the system.  If an answer
	 * can't be found, the value zero will be returned
	 *
	 * @return
	 */
	uint32_t logicalThreadCount() const {
		return std::thread::hardware_concurrency();
	}

	/**
	 *
	 * @return the singleton EnvironmentManager instance
	 */
	static EnvironmentManager & getInstance() {
		static EnvironmentManager instance;
		return instance;
	}

	virtual ~EnvironmentManager() {}
private:
	bool suppressLogging;
	uint32_t tasksCompleted;

	/**
	 * Singleton-pattern constructor
	 */
	EnvironmentManager()
	: suppressLogging(false)
	, tasksCompleted(0)
	{
	}

	/**
	 * Overriden copy constructor
	 */
	EnvironmentManager(EnvironmentManager const &);

	/**
	 * Overriden assignment operator
	 */
	EnvironmentManager & operator=(EnvironmentManager const &);
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_ENVIRONMENTMANAGER_HPP_ */

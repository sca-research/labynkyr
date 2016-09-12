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
 * PEUPool.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_PEUPOOL_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_PEUPOOL_HPP_

#include "labynkyr/search/parallel/PEU.hpp"
#include "labynkyr/search/parallel/Queue.hpp"
#include "labynkyr/search/parallel/SearchTaskRunner.hpp"
#include "labynkyr/search/verify/KeyVerifier.hpp"

#include <stdint.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace labynkyr {
namespace search {

/**
 *
 * A PEUPool encapsulates a group of PEUs
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType, typename SubkeyType>
class PEUPool {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	using QueueType = Queue<SearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>>;

	/**
	 *
	 * @param peuCount the number of PEUs to instantiate.  This should typically correspond to the number of physical cores on the host system.
	 * @param verifierFactory a factory for building KeyVerifier instances
	 * @param verifierCount the number of KeyVerifiers to instance.  If this is the same as peuCount, each PEU will get its own verifier.  This
	 * is the fastest approach we've tested so far.  You can also specify a number that divides peuCount -- e.g. given 4 PEUs, you can specify
	 * 2 verifiers, and each PEU will share a verifier instance with one other PEU.
	 * @param peuSleepNanoseconds when not processing a SearchTaskRunner, the PEU will check the read queue every peuSleepNanoseconds
	 * @throws std::invalid_argument
	 */
	PEUPool(uint32_t peuCount, KeyVerifierFactory<KeyLenBits> & verifierFactory, uint32_t verifierCount, uint64_t peuSleepNanoseconds)
	: verifiers(verifierCount)
	, peus(peuCount)
	{
		// Check parameters are ok
		if(peuCount % verifierCount != 0) {
			std::stringstream error;
			error << "Number of verifiers must divide the number of PEUs (verifiers must be distributed evenly).  Requested ";
			error << peuCount << " PEUs, requested " << verifierCount << " verifiers";
			throw std::invalid_argument(error.str().c_str());
		}
		// Instantiate the set of verifiers
		for(uint32_t verifierIndex = 0 ; verifierIndex < verifierCount ; verifierIndex++) {
			auto verifier = verifierFactory.newVerifier();
			verifiers[verifierIndex] = std::move(verifier);
		}
		// Instantiate the set of PEUs
		uint32_t const peusPerVerifier = peuCount / verifierCount;
		uint32_t nextVerifierIndex = 0;
		uint32_t currentVerifierAssignedCount = 0;
		for(uint32_t peuIndex = 0 ; peuIndex < peuCount ; peuIndex++) {
			// Check if new verifier needed
			nextVerifierIndex = (currentVerifierAssignedCount == peusPerVerifier) ? nextVerifierIndex + 1 : nextVerifierIndex;
			currentVerifierAssignedCount = currentVerifierAssignedCount % peusPerVerifier;
			auto & verifier = *verifiers[nextVerifierIndex].get();
			auto * peu = new PEU<VecCount, VecLenBits, WeightType, SubkeyType>(peuIndex, verifier, readQueue, writeQueue, peuSleepNanoseconds);
			std::unique_ptr<PEU<VecCount, VecLenBits, WeightType, SubkeyType>> peuPtr(peu);
			peus[peuIndex] = std::move(peuPtr);
			// Update counters
			currentVerifierAssignedCount++;
		}
	}

	virtual ~PEUPool() {}

	/**
	 *
	 * Start all PEUs listening for work
	 */
	void processAllPEUsAsynchronously() {
		for(auto & peu : peus) {
			peu->processAsynchronously();
		}
	}

	/**
	 *
	 * @param task place a new SearchTaskRunner on the read queue for the PEUs
	 */
	void addTasking(std::unique_ptr<SearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>> task) {
		readQueue.put(std::move(task));
	}

	/**
	 *
	 * @return the write queue completed SearchTaskRunner instances will be placed on
	 */
	Queue<SearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>> & getWriteQueue() {
		return writeQueue;
	}

	/**
	 *
	 * Stop all PEUs from listening for work
	 */
	void stopAllPEUs() {
		for(auto & peu : peus) {
			peu->stop();
		}
	}

	/**
	 *
	 * @throws std::exception if any PEU has caught an exception.  This exception will be re-thrown in this function call.
	 */
	void checkForThrownExceptions() {
		for(auto & peu : peus) {
			if(peu->isExceptionThrown()) {
				std::rethrow_exception(peu->getExceptionPtr());
			}
		}
	}

	/**
	 *
	 * @return the set of KeyVerifier instances instantiated for the group of PEUs
	 */
	std::vector<std::unique_ptr<KeyVerifier<KeyLenBits>>> & getVerifiers() {
		return verifiers;
	}

	/**
	 *
	 * @return true if the correct key has been found, false otherwise
	 */
	bool isKeyFound() const {
		bool found = false;
		for(auto & verifier : verifiers) {
			found |= verifier->success();
		}
		return found;
	}

	/**
	 *
	 * @return the total number of keys verified.  Stored in a 64-bit int as I'm pretty sure no-one will want to run experiments
	 * beyond this limit!  The rest of the codebase will work correctly at values larger than this, however.
	 */
	uint64_t keysVerified() const {
		return std::accumulate(
			verifiers.begin(),
			verifiers.end(),
			0,
			[](uint64_t a, std::unique_ptr<KeyVerifier<KeyLenBits>> const & b) {
				return a + b->keysChecked();
			}
		);
	}

	/**
	 *
	 * @return the value of the correct key, if found
	 * @throws std::logic_error
	 */
	Key<KeyLenBits> correctKey() const {
		for(auto & verifier : verifiers) {
			if(verifier->success()) {
				return verifier->correctKey();
			}
		}
		throw std::logic_error("The PEUs in this pool did not find the correct key");
	}

	uint32_t peuCount() const {
		return peus.size();
	}

	std::vector<std::unique_ptr<PEU<VecCount, VecLenBits, WeightType, SubkeyType>>> & getPEUs() {
		return peus;
	}
private:
	QueueType readQueue;
	QueueType writeQueue;
	std::vector<std::unique_ptr<KeyVerifier<KeyLenBits>>> verifiers;
	std::vector<std::unique_ptr<PEU<VecCount, VecLenBits, WeightType, SubkeyType>>> peus;
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_PEUPOOL_HPP_ */

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
 * KeyVerifier.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_KEYVERIFIER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_KEYVERIFIER_HPP_

#include "labynkyr/Key.hpp"

#include <stdint.h>

#include <memory>
#include <vector>

namespace labynkyr {
namespace search {

/**
 *
 * KeyVerifier describes an interface for checking candidate keys.
 *
 * @tparam KeyLenBits the length of the key in bits
 */
template<uint32_t KeyLenBits>
class KeyVerifier {
public:

	virtual ~KeyVerifier() {}

	/**
	 *
	 * @param candidateKeyBytes a vector of the bytes comprising the key candidate.  Implementations are not required to check
	 * keys as they are provided in this function, and may buffer keys for future verification.
	 */
	virtual void checkKey(std::vector<uint8_t> const & candidateKeyBytes) = 0;

	/**
	 *
	 * @return the number of keys this verifier has checked
	 */
	virtual uint64_t keysChecked() const = 0;

	/**
	 *
	 * @return true if the key has been found, false otherwise
	 */
	virtual bool success() const = 0;

	/**
	 *
	 * @return the verified Key, if it has been found
	 * @throws std::logic_error. In the absence of a Maybe-style type, it's simplest if implementing classes throw a std::logic_error
	 */
	virtual Key<KeyLenBits> correctKey() = 0;

	/**
	 *
	 * Implementations are not required to verify candidate keys as they arrive --- some may store keys in a buffer to check in groups.
	 * In this case, its necessary to force implementations to check any buffered keys.  All implementing classes must check all unverified
	 * keys when a call to this function is made.
	 */
	virtual void flush() = 0;
};

/**
 *
 * The parallelism model requires that each enumeration algorithm instance is paired with a unique KeyVerifier instance.  The parallel
 * search code is provided with a KeyVerifierFactory to allow it to construct as many KeyVerifiers as are needed.
 *
 * @tparam KeyLenBits the length of the key in bits
 */
template<uint32_t KeyLenBits>
class KeyVerifierFactory {
public:
	virtual ~KeyVerifierFactory() {}

	/**
	 *
	 * @return a new key verifier instance
	 */
	virtual std::unique_ptr<KeyVerifier<KeyLenBits>> newVerifier() const = 0;
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_KEYVERIFIER_HPP_ */

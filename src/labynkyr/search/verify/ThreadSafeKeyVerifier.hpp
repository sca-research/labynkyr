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
 * ThreadSafeKeyVerifier.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_THREADSAFEKEYVERIFIER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_THREADSAFEKEYVERIFIER_HPP_

#include "labynkyr/search/verify/KeyVerifier.hpp"

#include <mutex>

namespace labynkyr {
namespace search {

/**
 *
 * A very naive method for creating a thread-safe KeyVerifier.  This is achieved by wrapping an internal KeyVerifier instance and associating
 * the calls to checkKey() and flush() with a mutex.
 *
 * @tparam KeyLenBits
 */
template<uint32_t KeyLenBits>
class ThreadSafeKeyVerifier : public KeyVerifier<KeyLenBits> {
public:
	ThreadSafeKeyVerifier(std::unique_ptr<KeyVerifier<KeyLenBits>> internal)
	: internal(std::move(internal))
	{
	}

	~ThreadSafeKeyVerifier() {}

	void checkKey(std::vector<uint8_t> const & candidateKeyBytes) override {
		std::unique_lock<std::mutex> lock(mutex);
		internal->checkKey(candidateKeyBytes);
	}

	uint64_t keysChecked() const override {
		return internal->keysChecked();
	}

	bool success() const override {
		return internal->success();
	}

	Key<KeyLenBits> correctKey() override {
		return internal->correctKey();
	}

	void flush() override {
		std::unique_lock<std::mutex> lock(mutex);
		internal->flush();
	}
private:
	std::unique_ptr<KeyVerifier<KeyLenBits>> internal;
	mutable std::mutex mutex;
};

template<uint32_t KeyLenBits>
class ThreadSafeKeyVerifierFactory : public KeyVerifierFactory<KeyLenBits> {
public:
	ThreadSafeKeyVerifierFactory(KeyVerifierFactory<KeyLenBits> & internal)
	: internal(internal)
	{
	}

	~ThreadSafeKeyVerifierFactory() {}

	std::unique_ptr<KeyVerifier<KeyLenBits>> newVerifier() const override {
		auto internalVerifier = internal.newVerifier();
		auto * threadSafeVerifier = new ThreadSafeKeyVerifier<KeyLenBits>(std::move(internalVerifier));
		return std::unique_ptr<KeyVerifier<KeyLenBits>>(threadSafeVerifier);
	}
private:
	KeyVerifierFactory<KeyLenBits> & internal;
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_THREADSAFEKEYVERIFIER_HPP_ */

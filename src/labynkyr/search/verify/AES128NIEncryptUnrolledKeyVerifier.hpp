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
 * AES128NIEncryptUnrolledKeyVerifier.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_AES128NIENCRYPTUNROLLEDKEYVERIFIER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_AES128NIENCRYPTUNROLLEDKEYVERIFIER_HPP_

#include "labynkyr/search/verify/KeyVerifier.hpp"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cstring>
#include <wmmintrin.h>
#include <x86intrin.h>

#include <stdexcept>

namespace labynkyr {
namespace search {

/**
 *
 * Implementation of the KeyVerifier interface for verifying AES-128 keys given a known plaintext and a ciphertext pair.  The
 * implementation uses the AES-NI instruction set and buffers 4 candidate keys at a time to maximise pipeline occupancy.
 */
class AES128NIEncryptUnrolledKeyVerifier : public KeyVerifier<128> {
public:
	AES128NIEncryptUnrolledKeyVerifier(std::vector<uint8_t> const & plaintext, std::vector<uint8_t> const & ciphertext)
	: KeyVerifier<128>()
	, count(0)
	, currentBatchSize(0)
	, found(false)
	, foundKeyBytes(16)
	, output(16)
	{
		#ifdef __APPLE__
		posix_memalign((void**)&keysBuffer, 16, 16 * 4);
		#else
		keysBuffer = (uint8_t*) aligned_alloc(16, 16 * 4);
		#endif

		std::copy(plaintext.begin(), plaintext.end(), this->plaintext);
		std::copy(ciphertext.begin(), ciphertext.end(), expectedCiphertext);
	}

	~AES128NIEncryptUnrolledKeyVerifier() {
		free(keysBuffer);
	}

	void checkKey(std::vector<uint8_t> const & candidateKeyBytes) override {
		// Copy the incoming key in
		std::copy(candidateKeyBytes.begin(), candidateKeyBytes.end(), keysBuffer + currentBatchSize * 16);
		count++;
		currentBatchSize++;
		if(currentBatchSize == 4) {
			unrolledKeys(keysBuffer, plaintext, ciphertextsBuffer);
			runCheck();
		}
	}

	uint64_t keysChecked() const override {
		return count;
	}

	bool success() const override {
		return found;
	}

	Key<128> correctKey() override {
		if(found) {
			Key<128> const key(foundKeyBytes);
			return key;
		}
		throw std::logic_error("Key has not been found");
	}

	void flush() override {
		unrolledKeys(keysBuffer, plaintext, ciphertextsBuffer);
		runCheck();
	}
private:
	uint64_t count;
	uint64_t currentBatchSize;
	bool found;
	std::vector<uint8_t> foundKeyBytes;
	std::vector<uint8_t> output;

	uint8_t plaintext[16] 								__attribute__((aligned(16)));
	uint8_t expectedCiphertext[16] 						__attribute__((aligned(16)));
	//uint8_t keysBuffer[64] 								__attribute__((aligned(64)));
	uint8_t * keysBuffer;
	uint8_t ciphertextsBuffer[64] 						__attribute__((aligned(64)));

	uint32_t const rcon[16] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a };

	/**
	 *
	 * Compare all four ciphertexts against the expected one.
	 */
	void runCheck() {
		if(!found) {
			if(0 == memcmp(&(expectedCiphertext[00]), &(ciphertextsBuffer[00]), 16)) {
				found = true;
				std::copy(keysBuffer, keysBuffer + 16, foundKeyBytes.begin());
			}
			if(0 == memcmp(&(expectedCiphertext[00]), &(ciphertextsBuffer[16]), 16)) {
				found = true;
				std::copy(keysBuffer + 16, keysBuffer + 32, foundKeyBytes.begin());
			}
			if(0 == memcmp(&(expectedCiphertext[00]), &(ciphertextsBuffer[32]), 16)) {
				found = true;
				std::copy(keysBuffer + 32, keysBuffer + 48, foundKeyBytes.begin());
			}
			if(0 == memcmp(&(expectedCiphertext[00]), &(ciphertextsBuffer[48]), 16)) {
				found = true;
				std::copy(keysBuffer + 48, keysBuffer + 64, foundKeyBytes.begin());
			}
			currentBatchSize = 0;
		}
	}

	void unrolledKeys(uint8_t * keys, uint8_t * plaintext, uint8_t *ciphertexts) {

	  __m128i const mask = _mm_set_epi8(0x0C, 0x03, 0x06, 0x09, 0x08, 0x0F, 0x02, 0x05,
	                              0x04, 0x0B, 0x0E, 0x01, 0x00, 0x07, 0x0A, 0x0D);

	  __m128i tmp0;
	  __m128i zero = _mm_xor_si128(tmp0, tmp0);

	  __m128i key0  = _mm_load_si128((__m128i*)&(keys[ 0]));
	  __m128i key1  = _mm_load_si128((__m128i*)&(keys[16]));
	  __m128i key2  = _mm_load_si128((__m128i*)&(keys[32]));
	  __m128i key3  = _mm_load_si128((__m128i*)&(keys[48]));

	  __m128i data0  = _mm_load_si128((__m128i*) plaintext);
	  __m128i data1  = data0;
	  __m128i data2  = data0;
	  __m128i data3  = data0;

	  __m128i rk0;
	  __m128i rk1;
	  __m128i rk2;
	  __m128i rk3;
	  __m128i rki;
	  __m128i rkj;

	  __m128i mmrcon;

	  data0   = _mm_xor_si128(data0, key0);
	  data1   = _mm_xor_si128(data1, key1);
	  data2   = _mm_xor_si128(data2, key2);
	  data3   = _mm_xor_si128(data3, key3);

	  rki     = _mm_unpacklo_epi32(key0, key1);
	  rkj     = _mm_unpacklo_epi32(key2, key3);
	  rk0     = _mm_unpacklo_epi64(rki, rkj);
	  rk1     = _mm_unpackhi_epi64(rki, rkj);

	  rki     = _mm_unpackhi_epi32(key0, key1);
	  rkj     = _mm_unpackhi_epi32(key2, key3);
	  rk2     = _mm_unpacklo_epi64(rki, rkj);
	  rk3     = _mm_unpackhi_epi64(rki, rkj);

	  tmp0    = _mm_aesenclast_si128(rk3, zero);
	  tmp0    = _mm_shuffle_epi8(tmp0, mask);
	  mmrcon  = _mm_set1_epi32(rcon[1]);

	  tmp0   = _mm_xor_si128(tmp0, mmrcon);
	  rk0    = _mm_xor_si128(rk0, tmp0);
	  rk1    = _mm_xor_si128(rk1, rk0);
	  rk2    = _mm_xor_si128(rk2, rk1);
	  rk3    = _mm_xor_si128(rk3, rk2);

	  rki    = _mm_unpacklo_epi32(rk0, rk1);
	  rkj    = _mm_unpacklo_epi32(rk2, rk3);
	  key0   = _mm_unpacklo_epi64(rki, rkj);
	  key1   = _mm_unpackhi_epi64(rki, rkj);

	  rki    = _mm_unpackhi_epi32(rk0, rk1);
	  rkj    = _mm_unpackhi_epi32(rk2, rk3);
	  key2   = _mm_unpacklo_epi64(rki, rkj);
	  key3   = _mm_unpackhi_epi64(rki, rkj);

	  for (uint8_t roundCounter = 1; roundCounter < 10; roundCounter++) {
		  tmp0    = _mm_aesenclast_si128(rk3, zero);
		  tmp0    = _mm_shuffle_epi8(tmp0, mask);
		  mmrcon  = _mm_set1_epi32(rcon[roundCounter+1]);

		  data0   = _mm_aesenc_si128(data0, key0);
		  data1   = _mm_aesenc_si128(data1, key1);
		  data2   = _mm_aesenc_si128(data2, key2);
		  data3   = _mm_aesenc_si128(data3, key3);

		  tmp0   = _mm_xor_si128(tmp0, mmrcon);
		  rk0    = _mm_xor_si128(rk0, tmp0);
		  rk1    = _mm_xor_si128(rk1, rk0);
		  rk2    = _mm_xor_si128(rk2, rk1);
		  rk3    = _mm_xor_si128(rk3, rk2);

		  rki    = _mm_unpacklo_epi32(rk0, rk1);
		  rkj    = _mm_unpacklo_epi32(rk2, rk3);
		  key0   = _mm_unpacklo_epi64(rki, rkj);
		  key1   = _mm_unpackhi_epi64(rki, rkj);

		  rki    = _mm_unpackhi_epi32(rk0, rk1);
		  rkj    = _mm_unpackhi_epi32(rk2, rk3);
		  key2   = _mm_unpacklo_epi64(rki, rkj);
		  key3   = _mm_unpackhi_epi64(rki, rkj);
	  }

	  data0   = _mm_aesenclast_si128(data0, key0);
	  data1   = _mm_aesenclast_si128(data1, key1);
	  data2   = _mm_aesenclast_si128(data2, key2);
	  data3   = _mm_aesenclast_si128(data3, key3);

	  _mm_store_si128((__m128i*)&(ciphertexts[00]), data0);
	  _mm_store_si128((__m128i*)&(ciphertexts[16]), data1);
	  _mm_store_si128((__m128i*)&(ciphertexts[32]), data2);
	  _mm_store_si128((__m128i*)&(ciphertexts[48]), data3);
	}
};

class AES128NIEncryptUnrolledKeyVerifierFactory : public KeyVerifierFactory<128> {
public:
	AES128NIEncryptUnrolledKeyVerifierFactory(std::vector<uint8_t> const & plaintext, std::vector<uint8_t> const & ciphertext)
	: plaintext(plaintext)
	, ciphertext(ciphertext)
	{
	}

	~AES128NIEncryptUnrolledKeyVerifierFactory() {}

	std::unique_ptr<KeyVerifier<128>> newVerifier() const override {
		auto * verifier = new AES128NIEncryptUnrolledKeyVerifier(plaintext, ciphertext);
		return std::unique_ptr<AES128NIEncryptUnrolledKeyVerifier>(verifier);
	}
private:
	std::vector<uint8_t> const & plaintext;
	std::vector<uint8_t> const & ciphertext;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_VERIFY_AES128NIENCRYPTUNROLLEDKEYVERIFIER_HPP_ */

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
 * CandidateKeyForestTests.cpp
 *
 */

#include "src/labynkyr/search/enumerate/CandidateKeyForest.hpp"

#include "src/labynkyr/search/verify/ListKeyVerifier.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <vector>

namespace labynkyr {
namespace search {

TEST(CandidateKeyForest_empty) {
	uint32_t const vectorSizeBits = 8;
	uint32_t const vectorCount = 3;

	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> const forest(0);
	CHECK_EQUAL(0, forest.size());
	CHECK_EQUAL(0, std::distance(forest.getForest()->begin(), forest.getForest()->end()));
}

TEST(CandidateKeyForest_sizeOne) {
	uint32_t const vectorSizeBits = 8;
	uint32_t const vectorCount = 3;

	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> const forest(1);
	CHECK_EQUAL(1, forest.size());
	CHECK_EQUAL(0, std::distance(forest.getForest()->begin(), forest.getForest()->end()));
}

TEST(CandidateKeyForest_merge_empty_one) {
	uint32_t const vectorSizeBits = 8;
	uint32_t const vectorCount = 3;
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest1(0);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest2(1);
	forest1.merge(forest2, 2);
	// Should now have a single key
	CHECK_EQUAL(1, forest1.size());
	CHECK_EQUAL(1, std::distance(forest1.getForest()->begin(), forest1.getForest()->end()));
}

TEST(CandidateKeyForest_merge_one_one) {
	uint32_t const vectorSizeBits = 8;
	uint32_t const vectorCount = 3;
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest1(1);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest2(1);
	forest1.merge(forest2, 3);
	CHECK_EQUAL(2, forest1.size());
	CHECK_EQUAL(1, std::distance(forest1.getForest()->begin(), forest1.getForest()->end()));
}

TEST(CandidateKeyForest_merge_verifyCandidates) {
	uint32_t const vectorSizeBits = 8;
	uint32_t const vectorCount = 3;
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest1(1);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest2(0);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest3(0);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest4(0);

	ListKeyVerifier<24> verifier;
	forest2.merge(forest1, 3);
	forest3.merge(forest2, 5);
	forest4.merge(forest3, 4);

	forest4.verifyKeys(verifier);

	// Only 1 key should be verified
	CHECK_EQUAL(1, verifier.keysChecked());
	std::vector<uint8_t> const expectedKey1Bytes = {0x04, 0x05, 0x03};
	CHECK_ARRAY_EQUAL(expectedKey1Bytes, verifier.keys().at(0), expectedKey1Bytes.size());
}

TEST(CandidateKeyForest_merge_verifyCandidates_2) {
	uint32_t const vectorSizeBits = 8;
	uint32_t const vectorCount = 3;
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest1(1);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest2(0);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest3(0);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest4(0);

	ListKeyVerifier<24> verifier;
	forest2.merge(forest1, 3);
	forest3.merge(forest2, 5);
	forest4.merge(forest3, 4);
	forest4.merge(forest3, 7);

	forest4.verifyKeys(verifier);

	// Only 1 key should be verified
	CHECK_EQUAL(2, verifier.keysChecked());
	std::vector<uint8_t> const expectedKey1Bytes = {0x04, 0x05, 0x03};
	CHECK_ARRAY_EQUAL(expectedKey1Bytes, verifier.keys().at(0), expectedKey1Bytes.size());
	std::vector<uint8_t> const expectedKey2Bytes = {0x07, 0x05, 0x03};
	CHECK_ARRAY_EQUAL(expectedKey2Bytes, verifier.keys().at(1), expectedKey2Bytes.size());
}

TEST(CandidateKeyForest_merge_verifyMergeCandidates) {
	uint32_t const vectorSizeBits = 8;
	uint32_t const vectorCount = 3;
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest1(1);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest2(0);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest3(0);
	CandidateKeyForest<vectorCount, vectorSizeBits, uint8_t> forest4(0);

	ListKeyVerifier<24> verifier;
	forest2.merge(forest1, 3);
	forest3.merge(forest2, 5);
	forest4.verifyMergeCandidates(verifier, forest3, 4);

	// Only 1 key should be verified
	CHECK_EQUAL(1, verifier.keysChecked());
	std::vector<uint8_t> const expectedKey1Bytes = {0x04, 0x05, 0x03};
	CHECK_ARRAY_EQUAL(expectedKey1Bytes, verifier.keys().at(0), expectedKey1Bytes.size());

	// Forest 4 should not be modified
	CHECK_EQUAL(0, forest4.size());
	CHECK_EQUAL(0, std::distance(forest4.getForest()->begin(), forest4.getForest()->end()));
}

} /* namespace search */
} /* namespace labynkyr */

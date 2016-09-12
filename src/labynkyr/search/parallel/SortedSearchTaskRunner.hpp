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
 * SortedSearchTaskRunner.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_SORTEDSEARCHTASKRUNNER_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_SORTEDSEARCHTASKRUNNER_HPP_

#include "labynkyr/search/parallel/SearchTaskRunner.hpp"

#include "labynkyr/search/PathCountSearch.hpp"
#include "labynkyr/WeightTable.hpp"

namespace labynkyr {
namespace search {

/**
 *
 * SortedSearchTaskRunner implements SearchTaskRunner and wraps an instance of the Sorted enumeration algorithm.
 *
 * The Sorted algorithm requires the weight table to be sorted in ascending order.  This class will copy the weight table first.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType, typename SubkeyType>
class SortedSearchTaskRunner : public SearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType> {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	SortedSearchTaskRunner(SearchTask<VecCount, VecLenBits, WeightType> const & task, BigInt<KeyLenBits> expectedTaskSize)
	: SearchTaskRunner<VecCount, VecLenBits, WeightType, SubkeyType>(task, expectedTaskSize)
	, weightTableCopy(task.getWeightTable())
	, maxKeyWeight(task.getMaxKeyWeight())
	{
	}

	~SortedSearchTaskRunner() {}

	void processSequentially(KeyVerifier<KeyLenBits> & keyVerifier) override {
		PathCountSearch<VecCount, VecLenBits, WeightType, SubkeyType> pathCountSearch(keyVerifier);
		auto const start = std::chrono::high_resolution_clock::now();
		pathCountSearch.searchWithSorted(maxKeyWeight, weightTableCopy);
		auto const end = std::chrono::high_resolution_clock::now();
		this->duration = std::chrono::duration<uint64_t, std::nano>(end - start);
		// Check whether found the key
		keyVerifier.flush();
		this->keyFound = keyVerifier.success();
	}

	std::string methodName() const override {
		return "Sorted";
	}
private:
	WeightTable<VecCount, VecLenBits, WeightType> weightTableCopy;
	WeightType const maxKeyWeight;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_SORTEDSEARCHTASKRUNNER_HPP_ */

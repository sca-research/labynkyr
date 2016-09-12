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
 * CandidateKeyTree.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_CANDIDATEKEYTREE_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_CANDIDATEKEYTREE_HPP_

#include "labynkyr/search/verify/FullKeyBuilder.hpp"
#include "labynkyr/search/verify/KeyVerifier.hpp"

#include <forward_list>
#include <memory>

namespace labynkyr {
namespace search {

/**
 *
 * The ActiveNodeFinder/Forest algorithm stores candidate keys in a tree.  This trades off memory at the cost of some computation when keys are
 * constructed from their subkey parts in the verification phase.
 *
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename SubkeyType>
class CandidateKeyTree {
public:
	enum {
		KeyLenBits = VecLenBits * VecCount
	};

	CandidateKeyTree(SubkeyType value, std::shared_ptr<std::vector<std::unique_ptr<CandidateKeyTree<VecCount, VecLenBits, SubkeyType>>>> children, uint64_t forestSize)
	: value(value)
	, children(children)
	, treeSize(forestSize)
	{
	}

	/**
	 *
	 * @return the subkey value represented by this tree
	 */
	SubkeyType getValue() const {
		return value;
	}

	std::shared_ptr<std::vector<std::unique_ptr<CandidateKeyTree<VecCount, VecLenBits, SubkeyType>>>> getChildren() const {
		return children;
	}

	/**
	 *
	 * Iterates through the tree, constructs all candidate keys, and verifiers using the supplied verifier.
	 *
	 * @param keyValues a vector of length VecCount, to store the candidate keys in subkey form
	 * @param fullKeyBytes a vector of length KeyLenBits / 8, to store the candidate keys in byte format
	 * @param verifier
	 */
	void buildAndVerifyKeys(std::vector<SubkeyType> & keyValues, std::vector<uint8_t> & fullKeyBytes, uint32_t index, KeyVerifier<KeyLenBits> & verifier) {
		keyValues[index] = value;
		if(index == keyValues.size() - 1) {
			FullKeyBuilder<VecCount, VecLenBits, SubkeyType>::fullKey(keyValues, fullKeyBytes);
			verifier.checkKey(fullKeyBytes);
		} else if(size() > 0 && !verifier.success()) {
			for(auto & child : *children.get()) {
				child->buildAndVerifyKeys(keyValues, fullKeyBytes, index + 1, verifier);
			}
		}
	}

	/**
	 *
	 * @return the number of candidate keys stored in this tree
	 */
	uint64_t size() const {
		return treeSize;
	}
private:
	SubkeyType const value;
	std::shared_ptr<std::vector<std::unique_ptr<CandidateKeyTree<VecCount, VecLenBits, SubkeyType>>>> children;
	uint64_t treeSize;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_CANDIDATEKEYTREE_HPP_ */

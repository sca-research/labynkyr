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
 * ApproximateRank.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_RANK_APPROXIMATERANK_HPP_
#define LABYNKYR_SRC_LABYNKYR_RANK_APPROXIMATERANK_HPP_

#include "labynkyr/BigInt.hpp"
#include "labynkyr/BitWindow.hpp"
#include "labynkyr/DistinguishingTable.hpp"
#include "labynkyr/Key.hpp"

#include <stdint.h>

#include <functional>

namespace labynkyr {
namespace rank {

/**
 *
 * Static method for approximating the rank for a global key by multiplying together the rank values of the individual subkeys.
 *
 * IMPORTANT: The notion of rank here is slightly different: in a perfect attack on a subkey (where the correct subkey
 * value has the 'best' score, the rank of the correct subkey in that vector is 1 and NOT 0.  If there are two better subkeys,
 * then the correct subkey has rank 3 and NOT 2. This is necessary to allow for the multiplication of each subkey rank.
 *
 * In the case of ties, the number ties do not count towards the rank.  E.g. if three keys, including the true one, have the
 * same highest(/best) distinguishing score, then the rank of the correct key is still 1, and is not 3.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam ScoresType the floating-point type of the distinguishing scores (e.g float or double)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename ScoresType>
class ApproximateRank {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits,
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * Approximate the rank by multiplying together individual subkey ranks.
	 *
	 * @param table
	 * @param key
	 * @param comparator
	 * @tparam ComparatorFn the type comparator used for computing the subkey rank values.  For example, if the 'best' subkey candidate
	 * is taken to be the one with the highest score (such as with a standard DPA or CPA), then pass in std::greater<> here, and
	 * vice-versa if the best candidate has the lowest score, pass in std::less<>
	 */
	template<typename ComparatorFn>
	static BigInt<KeyLenBits> rank(
			DistinguishingTable<VecCount, VecLenBits, ScoresType> const & table,
			Key<KeyLenBits> const & key,
			ComparatorFn comparator)
		{

		BigInt<KeyLenBits> approximatedRank = 1;
		for(uint32_t vectorIndex = 0 ; vectorIndex < VecCount ; vectorIndex++) {
			BitWindow const subkeyTargeted(vectorIndex * VecLenBits, VecLenBits);
			uint64_t const correctSubkeyIndex = key.subkeyValue(subkeyTargeted);
			ScoresType const correctSubkeyScore = table.score(vectorIndex, correctSubkeyIndex);

			// Find subkey rank
			uint64_t subkeyRank = 0;
			for(uint32_t subkeyIndex = 0 ; subkeyIndex < VectorSize ; subkeyIndex++) {
				ScoresType const thisSubkeyScore = table.score(vectorIndex, subkeyIndex);
				if(subkeyIndex != correctSubkeyIndex) {
					if(comparator(thisSubkeyScore, correctSubkeyScore)) {
						subkeyRank++;
					}
				}
			}
			// Multiply through
			approximatedRank *= BigInt<KeyLenBits>(subkeyRank + 1);
		}
		return approximatedRank;
	}
};

} /*namespace rank */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_RANK_APPROXIMATERANK_HPP_ */

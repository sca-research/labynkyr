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
 * SearchSpec.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHSPEC_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHSPEC_HPP_

#include "labynkyr/WeightTable.hpp"

#include "labynkyr/BigInt.hpp"
#include "labynkyr/BigReal.hpp"

#include <stdint.h>

#include <limits>
#include <sstream>
#include <stdexcept>

namespace labynkyr {
namespace search {

/**
 *
 * SearchSpec defines a global search task.  It is defined by two values:
 * 		- offset
 * 		- budget
 * Labynkyr will enumerate and verify the budget keys following the offset most likely.
 *
 * @tparam KeyLenBits the number of bits in the global key
 */
template<uint32_t KeyLenBits>
class SearchSpec {
public:
	/**
	 *
	 * @param offset the number of keys to skip searching
	 * @param budget the number of keys to search after the specified offset
	 * @throws std::invalid_argument
	 */
	SearchSpec(BigInt<KeyLenBits> offset, BigInt<KeyLenBits> budget)
: offset(offset)
, budget(budget)
{
		BigInt<KeyLenBits> const diff = std::numeric_limits<BigInt<KeyLenBits>>::max() - offset;
		if(budget > diff) {
			std::stringstream error;
			error << "Offset of " << offset << " with a budget of " << budget << " defines too many keys";
			throw std::invalid_argument(error.str().c_str());
		}
}

	SearchSpec(SearchSpec const & other)
	: offset(other.getOffset())
	, budget(other.getBudget())
	{
	}

	~SearchSpec() {}

	/**
	 *
	 * @return the number of the most likely keys that will be skipped
	 */
	BigInt<KeyLenBits> getOffset() const {
		return offset;
	}

	/**
	 *
	 * @return the total number of keys that will be searched
	 */
	BigInt<KeyLenBits> getBudget() const {
		return budget;
	}

	/**
	 *
	 * @return the index of the deepest key that will be searched (indexing is from zero)
	 */
	BigInt<KeyLenBits> deepestKey() const {
		return offset + budget - 1;
	}

	/**
	 *
	 * @return true if an offset is specified, false otherwise
	 */
	bool hasOffset() const {
		return offset != 0;
	}
private:
	BigInt<KeyLenBits> const offset;
	BigInt<KeyLenBits> const budget;
};

/**
 *
 * Builder class for creating SearchSpec objects.
 *
 * @tparam KeyLenBits the number of bits in the global key
 */
template<uint32_t KeyLenBits>
class SearchSpecBuilder {
public:
	/**
	 *
	 * @param budget set the exact value of the budget
	 */
	SearchSpecBuilder(BigInt<KeyLenBits> budget)
	: offset(0)
	, budget(budget)
	{
	}

	/**
	 *
	 * @param budgetBits set the value of the budget to be 2^budgetBits
	 */
	SearchSpecBuilder(uint32_t budgetBits)
	: offset(0)
	, budget(BigIntTools::twoX<KeyLenBits>(budgetBits))
	{
	}

	/**
	 *
	 * @param budgetBits set the value of the budget to be 2^budgetBits
	 */
	SearchSpecBuilder(double budgetBits)
	: offset(0)
	, budget(BigRealTools::twoX<KeyLenBits, 100>(budgetBits))
	{
	}

	~SearchSpecBuilder() {}

	/**
	 *
	 * @param offset set the exact value of the offset
	 */
	void setOffset(BigInt<KeyLenBits> offset) {
		this->offset = offset;
	}

	/**
	 *
	 * @param offsetBits set the value of the offset to be 2^offsetBits
	 */
	void setOffset(uint32_t offsetBits) {
		this->offset = BigIntTools::twoX<KeyLenBits>(offsetBits);
	}

	/**
	 *
	 * @param offsetBits set the value of the offset to be 2^offsetBits
	 */
	void setOffset(double offsetBits) {
		this->offset = BigRealTools::twoX<KeyLenBits, 100>(offsetBits);
	}

	SearchSpec<KeyLenBits> createSpec() const {
		return SearchSpec<KeyLenBits>(offset, budget);
	}
private:
	BigInt<KeyLenBits> offset;
	BigInt<KeyLenBits> budget;
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHSPEC_HPP_ */

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
 * SearchSpecTests.cpp
 *
 */

#include "src/labynkyr/search/SearchSpec.hpp"

#include "src/labynkyr/BigInt.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <stdexcept>

namespace labynkyr {
namespace search {

TEST(SearchSpecBuilder_deepestKey_noOffset) {
	BigInt<128> const budget("346237842384242");
	SearchSpec<128> const spec(0, budget);
	CHECK(346237842384241UL == spec.deepestKey());
}

TEST(SearchSpecBuilder_deepestKey_offset) {
	BigInt<128> const offset("5");
	BigInt<128> const budget("7");
	SearchSpec<128> const spec(offset, budget);
	CHECK(11 == spec.deepestKey());
}

TEST(SearchSpecBuilder_copyConstructor) {
	BigInt<128> const offset("5");
	BigInt<128> const budget("7");
	SearchSpec<128> const other(offset, budget);
	SearchSpec<128> const spec(other);
	CHECK(offset == spec.getOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_noOffset_BigIntBudget) {
	BigInt<128> const budget("346237842384242");
	SearchSpecBuilder<128> builder(budget);
	auto const spec = builder.createSpec();
	CHECK(0 == spec.getOffset());
	CHECK(!spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_noOffset_intBudgetBits) {
	BigInt<128> const budget("4503599627370496");
	SearchSpecBuilder<128> builder(52U);
	auto const spec = builder.createSpec();
	CHECK(0 == spec.getOffset());
	CHECK(!spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_noOffset_doubleBudgetBits) {
	BigInt<128> const budget("28651978537278");
	SearchSpecBuilder<128> builder(44.7037);
	auto const spec = builder.createSpec();
	CHECK(0 == spec.getOffset());
	CHECK(!spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_BigIntOffset_BigIntBudget) {
	BigInt<128> const offset("672523234234243");
	BigInt<128> const budget("346237842384242");
	SearchSpecBuilder<128> builder(budget);
	builder.setOffset(offset);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_BigIntOffset_intBudgetBits) {
	BigInt<128> const offset("672523234234243");
	BigInt<128> const budget("4503599627370496");
	SearchSpecBuilder<128> builder(52U);
	builder.setOffset(offset);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_BigIntOffset_doubleBudgetBits) {
	BigInt<128> const offset("672523234234243");
	BigInt<128> const budget("28651978537278");
	SearchSpecBuilder<128> builder(44.7037);
	builder.setOffset(offset);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_intBitsOffset_BigIntBudget) {
	BigInt<128> const offset("4503599627370496");
	BigInt<128> const budget("346237842384242");
	SearchSpecBuilder<128> builder(budget);
	builder.setOffset(52U);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_intBitsOffset_intBudgetBits) {
	BigInt<128> const offset("4503599627370496");
	BigInt<128> const budget("4503599627370496");
	SearchSpecBuilder<128> builder(52U);
	builder.setOffset(52U);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_intBitsOffset_doubleBudgetBits) {
	BigInt<128> const offset("4503599627370496");
	BigInt<128> const budget("28651978537278");
	SearchSpecBuilder<128> builder(44.7037);
	builder.setOffset(52U);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_doubleBitsOffset_BigIntBudget) {
	BigInt<128> const offset("28651978537278");
	BigInt<128> const budget("346237842384242");
	SearchSpecBuilder<128> builder(budget);
	builder.setOffset(44.7037);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_doubleBitsOffset_intBudgetBits) {
	BigInt<128> const offset("28651978537278");
	BigInt<128> const budget("4503599627370496");
	SearchSpecBuilder<128> builder(52U);
	builder.setOffset(44.7037);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpecBuilder_doubleBitsOffset_doubleBudgetBits) {
	BigInt<128> const offset("28651978537278");
	BigInt<128> const budget("28651978537278");
	SearchSpecBuilder<128> builder(44.7037);
	builder.setOffset(44.7037);
	auto const spec = builder.createSpec();
	CHECK(offset == spec.getOffset());
	CHECK(spec.hasOffset());
	CHECK(budget == spec.getBudget());
}

TEST(SearchSpec_overflow1) {
	BigInt<4> const offset(4);
	BigInt<4> const budget(13);
	CHECK_THROW(SearchSpec<4> spec(offset, budget), std::invalid_argument);
}

TEST(SearchSpec_overflow2) {
	BigInt<4> const offset(4);
	BigInt<4> const budget(12);
	CHECK_THROW(SearchSpec<4> spec(offset, budget), std::invalid_argument);
}

} /* namespace search */
} /* namespace labynkyr */

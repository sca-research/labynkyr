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
 * SearchTaskGeneratorTests.cpp
 *
 */

#include "src/labynkyr/search/SearchTaskGenerator.hpp"

#include "src/labynkyr/search/enumerate/WeightFinder.hpp"
#include "src/labynkyr/search/SearchTask.hpp"
#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdexcept>
#include <vector>

namespace labynkyr {
namespace search {

TEST(SearchTaskGenerator_twoVector_differentBudgets) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget1(1);
	BigInt<4> const idealBudget7(7);
	BigInt<4> const idealBudget6(6);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	auto task1 = generator.nextTask(idealBudget6);
	auto task2 = generator.nextTask(idealBudget1);
	auto task3 = generator.nextTask(idealBudget7);

	CHECK_EQUAL(0, task1.getMinKeyWeight()); CHECK_EQUAL(2, task1.getMaxKeyWeight());
	CHECK_EQUAL(2, task2.getMinKeyWeight()); CHECK_EQUAL(3, task2.getMaxKeyWeight());
	CHECK_EQUAL(3, task3.getMinKeyWeight()); CHECK_EQUAL(6, task3.getMaxKeyWeight());
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_isInitialTask) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget1(1);
	BigInt<4> const idealBudget7(7);
	BigInt<4> const idealBudget6(6);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	auto task1 = generator.nextTask(idealBudget6);
	CHECK_EQUAL(true, task1.isInitialTask());
	auto task2 = generator.nextTask(idealBudget1);
	CHECK_EQUAL(false, task2.isInitialTask());
	auto task3 = generator.nextTask(idealBudget7);
	CHECK_EQUAL(false, task3.isInitialTask());
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_keysAllocatedCount) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget1(1);
	BigInt<4> const idealBudget7(7);
	BigInt<4> const idealBudget6(6);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	auto task1 = generator.nextTask(idealBudget6);
	CHECK_EQUAL(6, generator.keysAllocatedCount());
	auto task2 = generator.nextTask(idealBudget1);
	CHECK_EQUAL(8, generator.keysAllocatedCount());
	auto task3 = generator.nextTask(idealBudget7);
	CHECK_EQUAL(15, generator.keysAllocatedCount());
	CHECK(!generator.isTasksAvailable());
}

TEST(SearchTaskGenerator_twoVector_budget2) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget(2);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	auto task1 = generator.nextTask(idealBudget);
	auto task2 = generator.nextTask(idealBudget);
	auto task3 = generator.nextTask(idealBudget);
	auto task4 = generator.nextTask(idealBudget);
	auto task5 = generator.nextTask(idealBudget);

	CHECK_EQUAL(0, task1.getMinKeyWeight()); CHECK_EQUAL(1, task1.getMaxKeyWeight());
	CHECK_EQUAL(1, task2.getMinKeyWeight()); CHECK_EQUAL(2, task2.getMaxKeyWeight());
	CHECK_EQUAL(2, task3.getMinKeyWeight()); CHECK_EQUAL(3, task3.getMaxKeyWeight());
	CHECK_EQUAL(3, task4.getMinKeyWeight()); CHECK_EQUAL(4, task4.getMaxKeyWeight());
	CHECK_EQUAL(4, task5.getMinKeyWeight()); CHECK_EQUAL(6, task5.getMaxKeyWeight());
}

TEST(SearchTaskGenerator_twoVector_budget1) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget(1);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	auto task1 = generator.nextTask(idealBudget);
	auto task2 = generator.nextTask(idealBudget);
	auto task3 = generator.nextTask(idealBudget);
	auto task4 = generator.nextTask(idealBudget);
	auto task5 = generator.nextTask(idealBudget);
	auto task6 = generator.nextTask(idealBudget);

	CHECK_EQUAL(0, task1.getMinKeyWeight()); CHECK_EQUAL(1, task1.getMaxKeyWeight());
	CHECK_EQUAL(1, task2.getMinKeyWeight()); CHECK_EQUAL(2, task2.getMaxKeyWeight());
	CHECK_EQUAL(2, task3.getMinKeyWeight()); CHECK_EQUAL(3, task3.getMaxKeyWeight());
	CHECK_EQUAL(3, task4.getMinKeyWeight()); CHECK_EQUAL(4, task4.getMaxKeyWeight());
	CHECK_EQUAL(4, task5.getMinKeyWeight()); CHECK_EQUAL(5, task5.getMaxKeyWeight());
	CHECK_EQUAL(5, task6.getMinKeyWeight()); CHECK_EQUAL(6, task6.getMaxKeyWeight());
}

TEST(SearchTaskGenerator_twoVector_budgetBits1) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	auto task1 = generator.nextTask(1U);
	auto task2 = generator.nextTask(1U);
	auto task3 = generator.nextTask(1U);
	auto task4 = generator.nextTask(1U);
	auto task5 = generator.nextTask(1U);

	CHECK_EQUAL(0, task1.getMinKeyWeight()); CHECK_EQUAL(1, task1.getMaxKeyWeight());
	CHECK_EQUAL(1, task2.getMinKeyWeight()); CHECK_EQUAL(2, task2.getMaxKeyWeight());
	CHECK_EQUAL(2, task3.getMinKeyWeight()); CHECK_EQUAL(3, task3.getMaxKeyWeight());
	CHECK_EQUAL(3, task4.getMinKeyWeight()); CHECK_EQUAL(4, task4.getMaxKeyWeight());
	CHECK_EQUAL(4, task5.getMinKeyWeight()); CHECK_EQUAL(6, task5.getMaxKeyWeight());
}

TEST(SearchTaskGenerator_twoVector_budgetBits0) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	auto task1 = generator.nextTask(0U);
	auto task2 = generator.nextTask(0U);
	auto task3 = generator.nextTask(0U);
	auto task4 = generator.nextTask(0U);
	auto task5 = generator.nextTask(0U);
	auto task6 = generator.nextTask(0U);

	CHECK_EQUAL(0, task1.getMinKeyWeight()); CHECK_EQUAL(1, task1.getMaxKeyWeight());
	CHECK_EQUAL(1, task2.getMinKeyWeight()); CHECK_EQUAL(2, task2.getMaxKeyWeight());
	CHECK_EQUAL(2, task3.getMinKeyWeight()); CHECK_EQUAL(3, task3.getMaxKeyWeight());
	CHECK_EQUAL(3, task4.getMinKeyWeight()); CHECK_EQUAL(4, task4.getMaxKeyWeight());
	CHECK_EQUAL(4, task5.getMinKeyWeight()); CHECK_EQUAL(5, task5.getMaxKeyWeight());
	CHECK_EQUAL(5, task6.getMinKeyWeight()); CHECK_EQUAL(6, task6.getMaxKeyWeight());
}

TEST(SearchTaskGenerator_twoVector_noFurtherTasks) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget(2);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable);

	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	CHECK_THROW(generator.nextTask(idealBudget), std::logic_error);
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_maxKeysAllocatableCount) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget1(1);
	BigInt<4> const idealBudget7(7);
	BigInt<4> const idealBudget6(6);
	BigInt<4> const maxKeysAllocatableCount(8);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);

	generator.nextTask(idealBudget6);
	generator.nextTask(idealBudget1);
	CHECK_THROW(generator.nextTask(idealBudget7), std::logic_error);
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_maxKeysAllocatableCount2) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget1(1);
	BigInt<4> const idealBudget6(6);
	BigInt<4> const maxKeysAllocatableCount(7);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);

	generator.nextTask(idealBudget6);
	CHECK_THROW(generator.nextTask(idealBudget1), std::logic_error);
}

TEST(SearchTaskGenerator_twoVector_budget2_maxKeysAllocatableCount_throw) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget(2);
	BigInt<4> const maxKeysAllocatableCount(8);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);

	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	CHECK_THROW(generator.nextTask(idealBudget), std::logic_error);
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_withmaxKeysAllocatableCount_isTasksAvailable) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget1(1);
	BigInt<4> const idealBudget7(7);
	BigInt<4> const idealBudget6(6);
	BigInt<4> const maxKeysAllocatableCount(8);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);

	generator.nextTask(idealBudget6);
	generator.nextTask(idealBudget1);
	CHECK(generator.isTasksAvailable() == false);
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_isTasksAvailable2) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget6(6);
	BigInt<4> const maxKeysAllocatableCount(7);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);

	generator.nextTask(idealBudget6);
	CHECK(generator.isTasksAvailable() == false);
}

TEST(SearchTaskGenerator_twoVector_budget2_isTasksAvailable) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const idealBudget(2);
	BigInt<4> const maxKeysAllocatableCount(8);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);

	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	generator.nextTask(idealBudget);
	CHECK(generator.isTasksAvailable() == false);
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_maxKeysAllocatableCount_tightenmaxKeysAllocatableCount_limit6) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const maxKeysAllocatableCount(6);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);
	CHECK_EQUAL(6, generator.getMaxKeysAllocatableCount());
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_maxKeysAllocatableCount_tightenmaxKeysAllocatableCount_limit9) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const maxKeysAllocatableCount(9);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);
	CHECK_EQUAL(8, generator.getMaxKeysAllocatableCount());
}

TEST(SearchTaskGenerator_twoVector_differentBudgets_maxKeysAllocatableCount_tightenmaxKeysAllocatableCount_limit12) {
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);

	BigInt<4> const maxKeysAllocatableCount(12);
	SearchTaskGenerator<2, 2, uint32_t> generator(weightTable, maxKeysAllocatableCount);
	CHECK_EQUAL(8, generator.getMaxKeysAllocatableCount());
}

} /* namespace search */
} /* namespace labynkyr */



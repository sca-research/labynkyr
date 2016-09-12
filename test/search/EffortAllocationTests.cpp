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
 * EffortAllocationTests.cpp
 *
 */


#include "src/labynkyr/search/EffortAllocation.hpp"

#include "src/labynkyr/search/SearchSpec.hpp"

#include "src/labynkyr/BigInt.hpp"
#include "src/labynkyr/WeightTable.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

#include <vector>

namespace labynkyr {
namespace search {

TEST(EffortAllocation_allocateAllJobs) {
	SearchSpec<4> const searchSpec(0, 15);
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	uint32_t const preferredJobSizeBits = 0;

	EffortAllocation<2, 2, uint32_t> allocation(searchSpec, weightTable, preferredJobSizeBits);
	auto & tasks = allocation.getAllocatedTasks();

	std::vector<BigInt<4>> const expectedBatchSizes = {4, 2, 2, 5, 1, 1};
	std::vector<uint32_t> const expectedMinWeights = {0, 1, 2, 3, 4, 5};
	std::vector<uint32_t> const expectedMaxWeights = {1, 2, 3, 4, 5, 6};
	std::vector<BigInt<4>> batchSizes;
	std::vector<uint32_t> minWeights;
	std::vector<uint32_t> maxWeights;
	for(auto & task : tasks) {
		batchSizes.push_back(task.first);
		minWeights.push_back(task.second.getMinKeyWeight());
		maxWeights.push_back(task.second.getMaxKeyWeight());
	}
	CHECK_ARRAY_EQUAL(expectedBatchSizes, batchSizes, expectedBatchSizes.size());
	CHECK_ARRAY_EQUAL(expectedMinWeights, minWeights, expectedMinWeights.size());
	CHECK_ARRAY_EQUAL(expectedMaxWeights, maxWeights, expectedMaxWeights.size());
}

TEST(EffortAllocation_allocateAllJobs_differentFirstJobSize) {
	SearchSpec<4> const searchSpec(0, 15);
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	uint32_t const preferredFirstJobSizeBits = 3;
	uint32_t const preferredJobSizeBits = 0;

	EffortAllocation<2, 2, uint32_t> allocation(searchSpec, weightTable, preferredFirstJobSizeBits, preferredJobSizeBits);
	auto & tasks = allocation.getAllocatedTasks();

	std::vector<BigInt<4>> const expectedBatchSizes = {8, 5, 1, 1};
	std::vector<uint32_t> const expectedMinWeights = {0, 3, 4, 5};
	std::vector<uint32_t> const expectedMaxWeights = {3, 4, 5, 6};
	std::vector<BigInt<4>> batchSizes;
	std::vector<uint32_t> minWeights;
	std::vector<uint32_t> maxWeights;
	for(auto & task : tasks) {
		batchSizes.push_back(task.first);
		minWeights.push_back(task.second.getMinKeyWeight());
		maxWeights.push_back(task.second.getMaxKeyWeight());
	}
	CHECK_ARRAY_EQUAL(expectedBatchSizes, batchSizes, expectedBatchSizes.size());
	CHECK_ARRAY_EQUAL(expectedMinWeights, minWeights, expectedMinWeights.size());
	CHECK_ARRAY_EQUAL(expectedMaxWeights, maxWeights, expectedMaxWeights.size());
}

TEST(EffortAllocation_offset) {
	SearchSpec<4> const searchSpec(5, 8);
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	uint32_t const preferredJobSizeBits = 0;

	EffortAllocation<2, 2, uint32_t> allocation(searchSpec, weightTable, preferredJobSizeBits);
	auto & tasks = allocation.getAllocatedTasks();

	std::vector<BigInt<4>> const expectedBatchSizes = {2, 2, 5};
	std::vector<uint32_t> const expectedMinWeights = {1, 2, 3};
	std::vector<uint32_t> const expectedMaxWeights = {2, 3, 4};
	std::vector<BigInt<4>> batchSizes;
	std::vector<uint32_t> minWeights;
	std::vector<uint32_t> maxWeights;
	for(auto & task : tasks) {
		batchSizes.push_back(task.first);
		minWeights.push_back(task.second.getMinKeyWeight());
		maxWeights.push_back(task.second.getMaxKeyWeight());
	}
	CHECK_ARRAY_EQUAL(expectedBatchSizes, batchSizes, expectedBatchSizes.size());
	CHECK_ARRAY_EQUAL(expectedMinWeights, minWeights, expectedMinWeights.size());
	CHECK_ARRAY_EQUAL(expectedMaxWeights, maxWeights, expectedMaxWeights.size());
}

TEST(EffortAllocation_offset2) {
	SearchSpec<4> const searchSpec(4, 9);
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	uint32_t const preferredJobSizeBits = 0;

	EffortAllocation<2, 2, uint32_t> allocation(searchSpec, weightTable, preferredJobSizeBits);
	auto & tasks = allocation.getAllocatedTasks();

	std::vector<BigInt<4>> const expectedBatchSizes = {2, 2, 5};
	std::vector<uint32_t> const expectedMinWeights = {1, 2, 3};
	std::vector<uint32_t> const expectedMaxWeights = {2, 3, 4};
	std::vector<BigInt<4>> batchSizes;
	std::vector<uint32_t> minWeights;
	std::vector<uint32_t> maxWeights;
	for(auto & task : tasks) {
		batchSizes.push_back(task.first);
		minWeights.push_back(task.second.getMinKeyWeight());
		maxWeights.push_back(task.second.getMaxKeyWeight());
	}
	CHECK_ARRAY_EQUAL(expectedBatchSizes, batchSizes, expectedBatchSizes.size());
	CHECK_ARRAY_EQUAL(expectedMinWeights, minWeights, expectedMinWeights.size());
	CHECK_ARRAY_EQUAL(expectedMaxWeights, maxWeights, expectedMaxWeights.size());
}

TEST(EffortAllocation_removeNextTask_tasksRemaining) {
	SearchSpec<4> const searchSpec(0, 15);
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	uint32_t const preferredJobSizeBits = 0;

	EffortAllocation<2, 2, uint32_t> allocation(searchSpec, weightTable, preferredJobSizeBits);

	CHECK_EQUAL(6, allocation.tasksRemaining());
	auto task1 = allocation.removeNextTask();
	CHECK_EQUAL(5, allocation.tasksRemaining());
	auto task2 = allocation.removeNextTask();
	CHECK_EQUAL(4, allocation.tasksRemaining());
	auto task3 = allocation.removeNextTask();
	CHECK_EQUAL(3, allocation.tasksRemaining());
	auto task4 = allocation.removeNextTask();
	CHECK_EQUAL(2, allocation.tasksRemaining());
	auto task5 = allocation.removeNextTask();
	CHECK_EQUAL(1, allocation.tasksRemaining());
	auto task6 = allocation.removeNextTask();
	CHECK_EQUAL(0, allocation.tasksRemaining());

	CHECK_EQUAL(4, task1.first);
	CHECK_EQUAL(0, task1.second.getMinKeyWeight());
	CHECK_EQUAL(1, task1.second.getMaxKeyWeight());
	CHECK_EQUAL(2, task2.first);
	CHECK_EQUAL(1, task2.second.getMinKeyWeight());
	CHECK_EQUAL(2, task2.second.getMaxKeyWeight());
	CHECK_EQUAL(2, task3.first);
	CHECK_EQUAL(2, task3.second.getMinKeyWeight());
	CHECK_EQUAL(3, task3.second.getMaxKeyWeight());
	CHECK_EQUAL(5, task4.first);
	CHECK_EQUAL(3, task4.second.getMinKeyWeight());
	CHECK_EQUAL(4, task4.second.getMaxKeyWeight());
	CHECK_EQUAL(1, task5.first);
	CHECK_EQUAL(4, task5.second.getMinKeyWeight());
	CHECK_EQUAL(5, task5.second.getMaxKeyWeight());
	CHECK_EQUAL(1, task6.first);
	CHECK_EQUAL(5, task6.second.getMinKeyWeight());
	CHECK_EQUAL(6, task6.second.getMaxKeyWeight());

	CHECK_EQUAL(0, allocation.getAllocatedTasks().size());
}

TEST(EffortAllocation_preAllocatedConstructor) {
	SearchSpec<4> const searchSpec(0, 15);
	std::vector<uint32_t> const weights = {0, 1, 3, 0, 0, 2, 3, 0};
	WeightTable<2, 2, uint32_t> const weightTable(weights);
	uint32_t const preferredJobSizeBits = 0;

	EffortAllocation<2, 2, uint32_t> preallocation(searchSpec, weightTable, preferredJobSizeBits);
	auto & preallocatedTasks = preallocation.getAllocatedTasks();

	EffortAllocation<2, 2, uint32_t> allocation(preallocatedTasks);
	auto & tasks = allocation.getAllocatedTasks();

	std::vector<BigInt<4>> const expectedBatchSizes = {4, 2, 2, 5, 1, 1};
	std::vector<uint32_t> const expectedMinWeights = {0, 1, 2, 3, 4, 5};
	std::vector<uint32_t> const expectedMaxWeights = {1, 2, 3, 4, 5, 6};
	std::vector<BigInt<4>> batchSizes;
	std::vector<uint32_t> minWeights;
	std::vector<uint32_t> maxWeights;
	for(auto & task : tasks) {
		batchSizes.push_back(task.first);
		minWeights.push_back(task.second.getMinKeyWeight());
		maxWeights.push_back(task.second.getMaxKeyWeight());
	}
	CHECK_ARRAY_EQUAL(expectedBatchSizes, batchSizes, expectedBatchSizes.size());
	CHECK_ARRAY_EQUAL(expectedMinWeights, minWeights, expectedMinWeights.size());
	CHECK_ARRAY_EQUAL(expectedMaxWeights, maxWeights, expectedMaxWeights.size());
}

} /* namespace search */
} /* namespace labynkyr */



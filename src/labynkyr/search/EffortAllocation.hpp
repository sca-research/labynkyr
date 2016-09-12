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
 * EffortAllocation.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_EFFORTALLOCATION_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_EFFORTALLOCATION_HPP_

#include "labynkyr/search/enumerate/WeightFinder.hpp"
#include "labynkyr/search/SearchSpec.hpp"
#include "labynkyr/search/SearchTaskGenerator.hpp"
#include "labynkyr/search/SearchTask.hpp"
#include "labynkyr/BigInt.hpp"

#include <stdint.h>

#include <list>
#include <utility>

namespace labynkyr {
namespace search {

/**
 *
 * Divides a global SearchSpec into smaller SearchTasks.  These SearchTasks are chosen to fulfill a preferred size.  As deeper keys are
 * searched, the minimum task size is almost certain to increase beyond the preferred value. This is due to the number of keys with the
 * same weight increasing.
 *
 * The generated SearchTasks are stored in a list. A parallel search execution will take tasks from the front of this list. Each SearchTask
 * may be executed in parallel.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to score weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class EffortAllocation {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	/**
	 *
	 * Allocate a global search specification into discrete jobs of a fixed size.
	 *
	 * @param totalEffort
	 * @param weightTable
	 * @param preferredJobSizeBits each discrete SearchTask will aim to contain 2^preferredJobSizeBits key candidates
	 * @throws std::logic_error
	 */
	EffortAllocation(SearchSpec<KeyLenBits> const & totalEffort, WeightTable<VecCount, VecLenBits, WeightType> const & weightTable, uint32_t preferredJobSizeBits)
	: weightTable(weightTable)
	{
		allocateWork(totalEffort, preferredJobSizeBits, preferredJobSizeBits);
	}

	/**
	 *
	 * Allocate a global search specification into discrete jobs of a fixed size, where the first job can have a different preferred size.  This
	 * is useful when the Sorted enumeration algorithm can be used, as it is faster than the ANF/Forest algorithm, but can only be used on the
	 * initial batch of keys.
	 *
	 * @param totalEffort
	 * @param weightTable
	 * @param preferredFirstJobSizeBits the first SearchTask will aim to contain 2^preferredFirstJobSizeBits key candidates
	 * @param preferredJobSizeBits each subsequent discrete SearchTask will aim to contain 2^preferredJobSizeBits key candidates
	 * @throws std::logic_error
	 */
	EffortAllocation(SearchSpec<KeyLenBits> const & totalEffort, WeightTable<VecCount, VecLenBits, WeightType> const & weightTable, uint32_t preferredFirstJobSizeBits, uint32_t preferredJobSizeBits)
	: weightTable(weightTable)
	{
		allocateWork(totalEffort, preferredFirstJobSizeBits, preferredJobSizeBits);
	}

	/**
	 *
	 * Copy a pre-allocated list of tasks
	 *
	 * @param preallocatedTasks
	 */
	EffortAllocation(std::list<std::pair<BigInt<KeyLenBits>, SearchTask<VecCount, VecLenBits, WeightType>>> const & preallocatedTasks)
	: weightTable(preallocatedTasks.front().second.getWeightTable())
	{
		// Copy the pre-allocated tasks
		for(auto & task : preallocatedTasks) {
			BigInt<KeyLenBits> const taskKeyCount = task.first;
			SearchTask<VecCount, VecLenBits, WeightType> const cloneTask(task.second);
			allocatedTasks.push_back(std::make_pair(taskKeyCount, cloneTask));
		}
	}

	~EffortAllocation() {}

	WeightTable<VecCount, VecLenBits, WeightType> const & getWeightTable() const {
		return weightTable;
	}

	/**
	 *
	 * @return a list containing the following pairs, where the SearchTasks containing the most likely keys come first in the list:
	 * 		{number_of_keys_in_task, SearchTask}
	 */
	std::list<std::pair<BigInt<KeyLenBits>, SearchTask<VecCount, VecLenBits, WeightType>>> & getAllocatedTasks() {
		return allocatedTasks;
	}

	/**
	 *
	 * @return the number of SearchTasks that have not yet been completed
	 */
	uint32_t tasksRemaining() const {
		return allocatedTasks.size();
	}

	/**
	 *
	 * @return remove the SearchTask containing the next most likely set of key candidates from the internal list. A pair
	 *		{number_of_keys_in_task, SearchTask}
	 * will be returned.
	 */
	std::pair<BigInt<KeyLenBits>, SearchTask<VecCount, VecLenBits, WeightType>> removeNextTask() {
		auto const nextTask = allocatedTasks.front();
		allocatedTasks.pop_front();
		return nextTask;
	}
private:
	WeightTable<VecCount, VecLenBits, WeightType> const & weightTable;
	std::list<std::pair<BigInt<KeyLenBits>, SearchTask<VecCount, VecLenBits, WeightType>>> allocatedTasks;

	void allocateWork(SearchSpec<KeyLenBits> const & totalEffort, uint32_t preferredFirstJobSizeBits, uint32_t preferredJobSizeBits) {
		generateTasks(totalEffort, preferredFirstJobSizeBits, preferredJobSizeBits);
		if(totalEffort.hasOffset()) {
			// If there's a defined enumeration offset, loop through all generated tasks and remove all until the
			// cumulative number of keys tested breaches the specified offset.  Consequently, the first search task
			// retained is likely to start slightly before offset and end slightly after -- the offset boundary will
			// be somewhere in the middle of the first search task.
			BigInt<KeyLenBits> cumulativeKeys(0);
			auto iter = allocatedTasks.begin();
			while (iter != allocatedTasks.end()) {
				cumulativeKeys += iter->first;
				if(cumulativeKeys <= totalEffort.getOffset()) {
					allocatedTasks.erase(iter++);
				} else {
					break;
				}
			}
		}
	}

	void generateTasks(SearchSpec<KeyLenBits> const & totalEffort, uint32_t preferredFirstJobSizeBits, uint32_t preferredJobSizeBits) {
		SearchTaskGenerator<VecCount, VecLenBits, WeightType> taskGenerator(weightTable, totalEffort.deepestKey() + 1);
		while(taskGenerator.isTasksAvailable()) {
			auto const keysAllocatedCount = taskGenerator.keysAllocatedCount();
			uint32_t const preferredBatchSizeBits = (keysAllocatedCount == 0) ? preferredFirstJobSizeBits : preferredJobSizeBits;
			auto const task = taskGenerator.nextTask(preferredBatchSizeBits);
			auto const batchSize = taskGenerator.keysAllocatedCount() - keysAllocatedCount;
			allocatedTasks.push_back(std::make_pair(batchSize, task));
		}
	}
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_EFFORTALLOCATION_HPP_ */

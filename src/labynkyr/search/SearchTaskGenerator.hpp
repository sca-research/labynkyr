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
 * SearchTaskGenerator.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHTASKGENERATOR_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHTASKGENERATOR_HPP_

#include "labynkyr/search/enumerate/WeightFinder.hpp"
#include "labynkyr/search/SearchTask.hpp"
#include "labynkyr/BigInt.hpp"
#include "labynkyr/BigReal.hpp"
#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace labynkyr {
namespace search {

/**
 *
 * Takes as input a weight table, and optionally a maximum number of keys to allocate SearchTasks for.  This class can be used to allocate
 * sequential portions of the overall search space, starting from the most likely key.
 *
 * Note: can only get tasks for all but the deepest key. Not a problem, as if you're going to check every key anyway then a standard brute
 * force is much faster!
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to store weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class SearchTaskGenerator {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits
	};

	/**
	 *
	 * @param weightTable an integer representation of the distinguishing scores
	 * @param maxKeysAllocatableCount the maximum number of keys this generator can allocate (starting from the most likely key).  This
	 * limit is approximate, as large amounts of keys are likely to share the same weight value.
	 */
	SearchTaskGenerator(WeightTable<VecCount, VecLenBits, WeightType> const & weightTable, BigInt<KeyLenBits> maxKeysAllocatableCount)
	: weightTable(weightTable)
	, weightFinder(weightTable)
	, keysAllocated(0)
	, nextMinWeight(0)
	, nextMaxWeight(0)
	, maxKeysAllocatableCount(maxKeysAllocatableCount)
	{
		tightenMaxKeysAllocatableCount();
	}

	/**
	 *
	 * @param weightTable an integer representation of the distinguishing scores
	 */
	SearchTaskGenerator(WeightTable<VecCount, VecLenBits, WeightType> const & weightTable)
	: weightTable(weightTable)
	, weightFinder(weightTable)
	, keysAllocated(0)
	, nextMinWeight(0)
	, nextMaxWeight(0)
	, maxKeysAllocatableCount(weightFinder.list()[0])
	{
		tightenMaxKeysAllocatableCount();
	}

	~SearchTaskGenerator() {}

	/**
	 *
	 * Generate the next SearchTask
	 *
	 * @param idealTaskBudget the preferred number of keys to allocate in the next task.  The actual number of keys allocated may differ,
	 * as this is constrained by the weight representation of the distinguishing scores.
	 * @return the next task
	 * @throws std::logic_error if a task cannot be allocated
	 */
	SearchTask<VecCount, VecLenBits, WeightType> nextTask(BigInt<KeyLenBits> idealTaskBudget) {
		if(maxKeysAllocatableCount <= keysAllocated) {
			std::stringstream output;
			output << "No further search task available.";
			throw std::logic_error(output.str().c_str());
		}

		auto const diffToMax = maxKeysAllocatableCount - keysAllocated;
		BigInt<KeyLenBits> nextTaskBudget = 0;
		// We need to check for overflow here; if the next requested budget overflows the maximum we can provide
		// (either fundamentally there are no more keys, or there's a limit specified) then we just allocate the
		// remainder
		if(idealTaskBudget > diffToMax) {
			nextTaskBudget = keysAllocated + diffToMax;
		} else {
			nextTaskBudget = keysAllocated + idealTaskBudget;
		}
		auto const foundWeightAndCount = weightFinder.findBestWeight(nextTaskBudget);

		if(nextMaxWeight == foundWeightAndCount.first) {
			// We'd be returning the same set of keys as the budget isn't enough to allocate a new weight
			nextMinWeight = nextMaxWeight;
			nextMaxWeight += 1;
			keysAllocated = weightFinder.list()[weightFinder.list().size() - nextMaxWeight];
		} else {
			keysAllocated = foundWeightAndCount.second;
			nextMinWeight = nextMaxWeight;
			nextMaxWeight = foundWeightAndCount.first;
		}
		// Have to check that the new number of keys allocated isn't problematic
		if(maxKeysAllocatableCount < keysAllocated) {
			std::stringstream output;
			output << "No further search task available.";
			throw std::logic_error(output.str().c_str());
		}

		SearchTask<VecCount, VecLenBits, WeightType> task(nextMinWeight, nextMaxWeight, weightTable);
		return task;
	}

	/**
	 *
	 * Generate the next SearchTask
	 *
	 * @param idealTaskBudgetBits the preferred number of keys, in form 2^idealTaskBudgetBits, to allocate in the next task.
	 * The actual number of keys allocated may differ, as this is constrained by the weight representation of the distinguishing scores.
	 * @return the next task
	 * @throws std::logic_error if a task cannot be allocated
	 */
	SearchTask<VecCount, VecLenBits, WeightType> nextTask(uint32_t idealTaskBudgetBits) {
		BigInt<KeyLenBits> budget(1);
		budget = budget << idealTaskBudgetBits;
		return nextTask(budget);
	}

	/**
	 *
	 * Generate the next SearchTask
	 *
	 * @param idealTaskBudgetBits the preferred number of keys, in form 2^idealTaskBudgetBits, to allocate in the next task.
	 * The actual number of keys allocated may differ, as this is constrained by the weight representation of the distinguishing scores.
	 * as this is constrained by the weight representation of the distinguishing scores.
	 * @return the next task
	 * @throws std::logic_error if a task cannot be allocated
	 */
	SearchTask<VecCount, VecLenBits, WeightType> nextTask(double idealTaskBudgetBits) {
		BigInt<KeyLenBits> const budget = BigRealTools::twoX<KeyLenBits, 100>(idealTaskBudgetBits);
		return nextTask(budget);
	}

	/**
	 *
	 * @return the number of keys this generator has currently allocated in tasks
	 */
	BigInt<KeyLenBits> keysAllocatedCount() const {
		return keysAllocated;
	}

	/**
	 *
	 * @return true if all the keys accessible to this task builder have not already been allocated
	 */
	bool isTasksAvailable() const {
		return maxKeysAllocatableCount > keysAllocated;
	}

	/**
	 *
	 * @return the maximum number of keys this generator can allocate, starting from the most likely key candidate
	 */
	BigInt<KeyLenBits> getMaxKeysAllocatableCount() const {
		return maxKeysAllocatableCount;
	}
private:
	WeightTable<VecCount, VecLenBits, WeightType> const & weightTable;
	WeightFinder<VecCount, VecLenBits, WeightType> weightFinder;
	BigInt<KeyLenBits> keysAllocated;
	WeightType nextMinWeight;
	WeightType nextMaxWeight;
	BigInt<KeyLenBits> maxKeysAllocatableCount;

	/**
	 *
	 * If the request is to allocate tasks for e.g. the first 2^50 keys, then we need to determine which integer weight
	 * defines a number of keys that is closest to the preferred maximum, and set the new maximum to be that number
	 * of keys.
	 */
	void tightenMaxKeysAllocatableCount() {
		auto const foundWeightAndCount = weightFinder.findBestWeight(maxKeysAllocatableCount);
		auto const allocation = foundWeightAndCount.second;
		if(maxKeysAllocatableCount > allocation) {
			maxKeysAllocatableCount = allocation;
		} else if(maxKeysAllocatableCount < allocation) {
			auto weight = foundWeightAndCount.first;
			weight--;
			maxKeysAllocatableCount = weightFinder.list()[weightFinder.list().size() - weight];
		}
	}
};

} /*namespace search */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_SEARCHTASKGENERATOR_HPP_ */

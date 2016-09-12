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
 * PathCountEnumerationGraph.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_PATHCOUNTENUMERATIONGRAPH_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_PATHCOUNTENUMERATIONGRAPH_HPP_

#include "labynkyr/search/enumerate/CandidateKeyForest.hpp"
#include "labynkyr/search/SearchTask.hpp"

#include "labynkyr/rank/GraphCoordinate.hpp"

#include <stdint.h>

#include <memory>
#include <vector>

namespace labynkyr {
namespace search {

/**
 *
 * A slightly modified representation of the "path count" graph traversed by the rank estimation algorithm, providing
 * an implementation of the function to calculate the index of the right child node for a given coordinate.
 *
 * In the rank estimation problem, the graph nodes store a count of keys.  In the enumeration/search problem, each node is
 * required to store a (partial) set of key candidates.  This requires more storage space, and a more convoluted method of
 * updating nodes whenever new information is created by the traversal algorithm.
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to score weights (e.g uint32_t)
 * @tparam SubkeyType the integer type used to store a subkey valyue (e.g uint8_t for a typical 8-bit DPA attack)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType, typename SubkeyType>
class PathCountEnumerationGraph {
public:
	enum {
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * Instantiate the graph dimensions given the SearchTask task
	 *
	 * @param task
	 */
	PathCountEnumerationGraph(SearchTask<VecCount, VecLenBits, WeightType> const & task)
	: task(task)
	, rejectStateSet(CandidateKeyForest<VecCount, VecLenBits, SubkeyType>::rejectStateSet())
	, acceptStateSet(CandidateKeyForest<VecCount, VecLenBits, SubkeyType>::acceptStateSet())
	, current(task.getMaxKeyWeight())
	, previous(task.getMaxKeyWeight())
	{
		// Fill current with reject state sets
		for(uint64_t index = 0 ; index < current.size() ; index++) {
			current[index] = CandidateKeyForest<VecCount, VecLenBits, SubkeyType>::emptySet();
		}
	}

	~PathCountEnumerationGraph() {}

	/**
	 *
	 * Sets the forest state at the specified coordinate
	 *
	 * @param coord
	 * @param value
	 */
	void set(rank::GraphCoordinate const & coord, std::unique_ptr<CandidateKeyForest<VecCount, VecLenBits, SubkeyType>> value) {
		current.at(coord.getWeightIndex()) = std::move(value);
	}

	/**
	 *
	 * @return remove and return the forest stored at the first position in the graph.  This should only be called once the
	 * graph traversal is complete. This will contain the set of all keys to be tested.
	 */
	std::unique_ptr<CandidateKeyForest<VecCount, VecLenBits, SubkeyType>> removeFirst() {
		return std::move(current[0]);
	}

	/**
	 *
	 * @param rightChildIndex an index relative to the entire graph
	 * @return the forest stored within the graph at that index
	 */
	CandidateKeyForest<VecCount, VecLenBits, SubkeyType> const & rightChild(rank::GraphCoordinate const & rightChildIndex) {
		if(rightChildIndex.isAccept()) {
			return *acceptStateSet.get();
		} else if(rightChildIndex.isReject()) {
			return *rejectStateSet.get();
		}
		return *previous.at(rightChildIndex.getWeightIndex()).get();
	}

	/**
	 *
	 * @param coord a coordinate on the path count graph
	 * @return the coordinate for the right child of the input coordinate
	 */
	rank::GraphCoordinate rightChildIndex(rank::GraphCoordinate const & coord) const {
		if(coord.isReject()) {
			return rank::GraphCoordinate::rejectCoord();
		}

		uint64_t const vectorIndex = coord.getVectorIndex();
		uint64_t const subkeyIndex = coord.getSubkeyIndex();
		uint64_t const weightIndex = coord.getWeightIndex();

		uint64_t const weight = static_cast<uint64_t>(task.getWeightTable().weight(vectorIndex, subkeyIndex));
		uint64_t const newWeight = weightIndex + weight;

		if(newWeight >= task.getMaxKeyWeight()) {
			return rank::GraphCoordinate::rejectCoord();
		} else if(vectorIndex != VecCount - 1) {
			rank::GraphCoordinate const child(vectorIndex + 1, 0, newWeight);
			return child;
		} else {
			// if wi + w < minWeight reject, else accept
			if(newWeight < task.getMinKeyWeight()) {
				return rank::GraphCoordinate::rejectCoord();
			} else {
				return rank::GraphCoordinate::acceptCoord();
			}
		}
	}

	CandidateKeyForest<VecCount, VecLenBits, SubkeyType> & at(rank::GraphCoordinate const & coord) {
		return *current.at(coord.getWeightIndex()).get();
	}

	/**
	 *
	 * Must be called whenever the traversal begins accessing nodes corresponding to a new distinguishing vector,
	 * except for the case that the new distinguishing vector is the final (zeroth) one.
	 */
	void rotateBuffers() {
		// Copy current into previous
		uint64_t offset = 0;
		for(auto & keySet : current) {
			previous[offset].reset();
			previous[offset] = std::move(keySet);
			offset++;
		}
		// Fill current with empty forests
		for(uint64_t index = 0 ; index < current.size() ; index++) {
			current[index].reset();
			current[index] = std::move(CandidateKeyForest<VecCount, VecLenBits, SubkeyType>::emptySet());
		}
	}

	std::vector<std::unique_ptr<CandidateKeyForest<VecCount, VecLenBits, SubkeyType>>> & previousRow() {
		return previous;
	}
private:
	SearchTask<VecCount, VecLenBits, WeightType> const & task;
	std::unique_ptr<CandidateKeyForest<VecCount, VecLenBits, SubkeyType>> rejectStateSet;
	std::unique_ptr<CandidateKeyForest<VecCount, VecLenBits, SubkeyType>> acceptStateSet;
	std::vector<std::unique_ptr<CandidateKeyForest<VecCount, VecLenBits, SubkeyType>>> current;
	std::vector<std::unique_ptr<CandidateKeyForest<VecCount, VecLenBits, SubkeyType>>> previous;
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_ENUMERATE_PATHCOUNTENUMERATIONGRAPH_HPP_ */

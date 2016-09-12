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
 * PathCountGraph.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_RANK_PATHCOUNTGRAPH_HPP_
#define LABYNKYR_SRC_LABYNKYR_RANK_PATHCOUNTGRAPH_HPP_

#include "labynkyr/rank/GraphCoordinate.hpp"
#include "labynkyr/BigInt.hpp"
#include "labynkyr/WeightTable.hpp"

#include <stdint.h>

#include <vector>

namespace labynkyr {
namespace rank {

/**
 *
 * A representation of the "path count" graph traversed by the rank estimation algorithm, providing
 * implementations of functions to calculate the indexes of the left and right child nodes for a given coordinate.
 *
 * The graph is restricted to be 64 bits in size as uint64_t is used as the indexing type.  This requires that the retained precision
 * from the distinguishing scores is at the very most 60-62 bits.  As the entire graph must be traversed by the algorithm, in practice
 * the retained precision is likely to have to be capped at <= 32 bits for efficiency reasons.
 *
 * This is the fastest implementation of the PathCountRank algorithm, in addition to requiring the least memory.
 *
 * The optimisation stems from being able to automatically capture the role of the leftChild function through summation with
 * the existing value in the graph when the set() function is called (rather than overriding).
 *
 * This typically offers a significant speed-up.
 *
 * Storage: 2*W for W = the weight of the correct key
 *
 * @tparam VecCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
 * @tparam VecLenBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
 * @tparam WeightType the integer type used to score weights (e.g uint32_t)
 */
template<uint32_t VecCount, uint32_t VecLenBits, typename WeightType>
class PathCountGraph {
public:
	enum {
		KeyLenBits = VecCount * VecLenBits,
		// Number of distinguishing scores in each distinguishing vector
		VectorSize = 1UL << VecLenBits
	};

	/**
	 *
	 * @param keyWeight the sum of the integer scores for the known/correct key
	 */
	PathCountGraph(WeightType keyWeight)
	: keyWeight(static_cast<uint64_t>(keyWeight))
	, current(keyWeight)
	, previous(keyWeight)
	, acceptValue(1)
	, rejectValue(0)
	{
	}

	~PathCountGraph() {}

	/**
	 *
	 * @return the value stored at the first position in the graph.  This should only be called once the
	 * graph traversal is complete. This will contain the rank of the key when traversal has completed.
	 */
	BigInt<KeyLenBits> first() {
		return current[0];
	}

	/**
	 *
	 * @param coord a coordinate on the path count graph
	 * @param weightTable
	 * @return the coordinate for the right child of the input coordinate, given the weight table
	 */
	GraphCoordinate rightChildIndex(GraphCoordinate const & coord, WeightTable<VecCount, VecLenBits, WeightType> const & weightTable) const {
		if(coord.isReject()) {
			return GraphCoordinate::rejectCoord();
		}

		uint64_t const vectorIndex = coord.getVectorIndex();
		uint64_t const subkeyIndex = coord.getSubkeyIndex();
		uint64_t const weightIndex = coord.getWeightIndex();

		uint64_t const weight = static_cast<uint64_t>(weightTable.weight(vectorIndex, subkeyIndex));

		if(weightIndex + weight >= keyWeight) {
			return GraphCoordinate::rejectCoord();
		} else if(vectorIndex != VecCount - 1) {
			GraphCoordinate const child(vectorIndex + 1, 0, weightIndex + weight);
			return child;
		} else {
			return GraphCoordinate::acceptCoord();
		}
	}

	/**
	 *
	 * @param rightChildIndex an index relative to the entire graph
	 * @return the value stored within the graph at that index
	 */
	BigInt<KeyLenBits> rightChild(GraphCoordinate const & rightChildIndex) {
		if(rightChildIndex.isAccept()) {
			return acceptValue;
		} else if(rightChildIndex.isReject()) {
			return rejectValue;
		}
		return previous.at(rightChildIndex.getWeightIndex());
	}

	/**
	 *
	 * Sets the value at the specified coordinate
	 *
	 * @param coord
	 * @param value
	 */
	void set(GraphCoordinate const & coord, BigInt<KeyLenBits> value) {
		current.at(coord.getWeightIndex()) += value;
	}

	/**
	 *
	 *
	 * Must be called whenever the traversal begins accessing nodes corresponding to a new distinguishing vector,
	 * except for the case that the new distinguishing vector is the final (zeroth) one.
	 */
	void rotateBuffers() {
		std::copy(current.begin(), current.end(), previous.begin());
		std::fill(current.begin(), current.end(), 0);
	}

	/**
	 *
	 * @return
	 */
	std::vector<BigInt<KeyLenBits>> & previousRow() {
		return previous;
	}
private:
	uint64_t const keyWeight;

	std::vector<BigInt<KeyLenBits>> current;
	std::vector<BigInt<KeyLenBits>> previous;

	BigInt<KeyLenBits> const acceptValue;
	BigInt<KeyLenBits> const rejectValue;
};

} /*namespace rank */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_RANK_PATHCOUNTGRAPH_HPP_ */

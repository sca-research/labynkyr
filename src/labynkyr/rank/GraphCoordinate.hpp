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
 * GraphCoordinate.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_RANK_GRAPHCOORDINATE_HPP_
#define LABYNKYR_SRC_LABYNKYR_RANK_GRAPHCOORDINATE_HPP_

#include <stdint.h>

namespace labynkyr {
namespace rank {

/**
 *
 *
 * Represents a three-dimension coordinate on a path count graph.   The coordinate can take one of three states:
 * 		(1) a tuple (vectorIndex, subkeyIndex, weightIndex), representing a standard point on the graph;
 * 		(2) a reject state, if the coordinate is the reject node
 * 		(3) an accept state, if the coordinate is the accept node
 *
 * 	The coordinate system is zero-indexed.
 *
 * 	Copy-constructable.
 */
class GraphCoordinate {
public:
	/**
	 *
	 * Create an accept or reject node coordinate.
	 *
	 * @param isAccept if true, the coordinate will be set to the accept node, and if false will be set to the reject node.
	 */
	GraphCoordinate(bool isAccept)
	: accept(isAccept)
	, reject(!isAccept)
	, vectorIndex(0)
	, subkeyIndex(0)
	, weightIndex(0)
	{
	}

	/**
	 *
	 * Create a standard coordinate
	 *
	 * @param vectorIndex
	 * @param subkeyIndex
	 * @param weightIndex
	 */
	GraphCoordinate(uint64_t vectorIndex, uint64_t subkeyIndex, uint64_t weightIndex)
	: accept(false)
	, reject(false)
	, vectorIndex(vectorIndex)
	, subkeyIndex(subkeyIndex)
	, weightIndex(weightIndex)
	{
	}

	/**
	 *
	 * Copy constructor
	 *
	 * @param other
	 */
	GraphCoordinate(GraphCoordinate const & other)
	: accept(other.isAccept())
	, reject(other.isReject())
	, vectorIndex(other.getVectorIndex())
	, subkeyIndex(other.getSubkeyIndex())
	, weightIndex(other.getWeightIndex())
	{
	}

	virtual ~GraphCoordinate() {}

	/**
	 *
	 * @return true if the coordinate is the accept node, false otherwise
	 */
	bool isAccept() const {
		return accept;
	}

	/**
	 *
	 * @return true if the coordinate is the reject node, false otherwise
	 */
	bool isReject() const {
		return reject;
	}

	/**
	 *
	 * @return the index over the distinguishing vectors
	 */
	uint64_t getVectorIndex() const {
		return vectorIndex;
	}

	/**
	 *
	 * @return the index over the number of subkeys in each distinguishing vector
	 */
	uint64_t getSubkeyIndex() const {
		return subkeyIndex;
	}

	/**
	 *
	 * @return the index between [0, keyWeight - 1] for the weight
	 */
	uint64_t getWeightIndex() const {
		return weightIndex;
	}

	/**
	 *
	 * @return a coordinate for the accept node
	 */
	static GraphCoordinate acceptCoord() {
		GraphCoordinate coord(true);
		return coord;
	}

	/**
	 *
	 * @return a coordinate for the reject node
	 */
	static GraphCoordinate rejectCoord() {
		GraphCoordinate coord(false);
		return coord;
	}
protected:
	bool const accept;
	bool const reject;
	uint64_t const vectorIndex;
	uint64_t const subkeyIndex;
	uint64_t const weightIndex;
};

} /*namespace rank */
} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_RANK_GRAPHCOORDINATE_HPP_ */

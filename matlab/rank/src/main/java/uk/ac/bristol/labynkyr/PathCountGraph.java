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
package uk.ac.bristol.labynkyr;

import java.math.BigInteger;

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
*/
public class PathCountGraph {
	
	private final int vectorCount;
	private final int keyWeight;
	private	BigInteger[] current;
	private	BigInteger[] previous;
	private	final BigInteger acceptValue;
	private	final BigInteger rejectValue;
	
	/**
	 * 
	 * @param vectorCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
	 * @param keyWeight the sum of the integer scores for the known/correct key
	 */
	public PathCountGraph(int vectorCount, int keyWeight) {
		this.vectorCount = vectorCount;
		this.keyWeight = keyWeight;
		this.current = new BigInteger[keyWeight];
		this.previous = new BigInteger[keyWeight];
		// 
		for(int index = 0 ; index < current.length ; index++) {
			previous[index] = BigInteger.ZERO;
			current[index] = BigInteger.ZERO;
		}
		this.acceptValue = BigInteger.ONE;
		this.rejectValue = BigInteger.ZERO;
	}

	/**
	 *
	 * @return the value stored at the first position in the graph.  This should only be called once the
	 * graph traversal is complete.
	 */
	public BigInteger first() {
		return current[0];
	}

	/**
	 *
	 * @param coord a coordinate on the path count graph
	 * @param weightTable
	 * @return the coordinate for the right child of the input coordinate, given the weight table
	 */
	public GraphCoordinate rightChildIndex(GraphCoordinate coord, WeightTable weightTable) {
		if(coord.isReject()) {
			return GraphCoordinate.rejectCoord();
		}

		final int vectorIndex = coord.getVectorIndex();
		final int subkeyIndex = coord.getSubkeyIndex();
		final int weightIndex = coord.getWeightIndex();

		final int weight = weightTable.weight(vectorIndex, subkeyIndex);

		if(weightIndex + weight >= keyWeight) {
			return GraphCoordinate.rejectCoord();
		} else if(vectorIndex != vectorCount - 1) {
			final GraphCoordinate child = new GraphCoordinate(vectorIndex + 1, 0, weightIndex + weight);
			return child;
		} else {
			return GraphCoordinate.acceptCoord();
		}
	}

	/**
	 *
	 * @param rightChildIndex an index relative to the entire graph
	 * @return the value stored within the graph at that index
	 */
	BigInteger rightChild(GraphCoordinate rightChildIndex) {
		if(rightChildIndex.isAccept()) {
			return acceptValue;
		} else if(rightChildIndex.isReject()) {
			return rejectValue;
		}
		return previous[rightChildIndex.getWeightIndex()];
	}

	/**
	 *
	 * Sets the value at the specified coordinate
	 *
	 * @param coord
	 * @param value
	 */
	void set(GraphCoordinate coord, BigInteger value) {
		current[coord.getWeightIndex()] = current[coord.getWeightIndex()].add(value);
	}

	/**
	 *
	 * Must be called whenever the traversal begins accessing nodes corresponding to a new distinguishing vector,
	 * except for the case that the new distinguishing vector is the final (zeroth) one.
	 */
	void rotateBuffers() {
		for(int index = 0 ; index < current.length ; index++) {
			previous[index] = current[index];
			current[index] = BigInteger.ZERO;
		}
	}

	/**
	 *
	 * @return
	 */
	BigInteger[] previousRow() {
		return previous;
	}
}

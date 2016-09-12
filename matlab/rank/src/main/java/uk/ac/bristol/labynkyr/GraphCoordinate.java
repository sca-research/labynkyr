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
*/
public class GraphCoordinate {

	private final boolean accept;
	private final boolean reject;
	private final int vectorIndex;
	private final int subkeyIndex;
	private final int weightIndex;
	
	/**
	 *
	 * Create a standard coordinate
	 *
	 * @param vectorIndex
	 * @param subkeyIndex
	 * @param weightIndex
	 */
	public GraphCoordinate(int vectorIndex, int subkeyIndex, int weightIndex) {
		this.accept = false;
		this.reject = false;
		this.vectorIndex = vectorIndex;
		this.subkeyIndex = subkeyIndex;
		this.weightIndex = weightIndex;
	}
	
	/**
	 *
	 * Create an accept or reject node coordinate.
	 *
	 * @param isAccept if true, the coordinate will be set to the accept node, and if false will be set to the reject node.
	 */
	public GraphCoordinate(boolean isAccept) {
		this.accept = isAccept;
		this.reject = !isAccept;
		this.vectorIndex = -1;
		this.subkeyIndex = -1;
		this.weightIndex = -1;
	}
	
	/**
	 *
	 * @return true if the coordinate is the accept node, false otherwise
	 */
	public final boolean isAccept() {
		return this.accept;
	}
	
	/**
	 *
	 * @return true if the coordinate is the reject node, false otherwise
	 */
	public final boolean isReject() {
		return this.reject;
	}
	
	/**
	 *
	 * @return the index over the distinguishing vectors
	 */
	public final int getVectorIndex() {
		return vectorIndex;
	}

	/**
	 *
	 * @return the index over the number of subkeys in each distinguishing vector
	 */
	public final int getSubkeyIndex() {
		return subkeyIndex;
	}

	/**
	 *
	 * @return the index between [0, keyWeight - 1] for the weight
	 */
	public final int getWeightIndex() {
		return weightIndex;
	}

	/**
	 *
	 * @return a coordinate for the accept node
	 */
	public static final GraphCoordinate acceptCoord() {
		GraphCoordinate coord = new GraphCoordinate(true);
		return coord;
	}

	/**
	 *
	 * @return a coordinate for the reject node
	 */
	public static final GraphCoordinate rejectCoord() {
		GraphCoordinate coord = new GraphCoordinate(false);
		return coord;
	}
}

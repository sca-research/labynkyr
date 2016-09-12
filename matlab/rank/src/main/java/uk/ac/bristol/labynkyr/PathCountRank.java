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
* Implementation of the path-counting rank estimation algorithm of Martin et al.:
*
* 		How to Enumerate Your Keys Accurately and Efficiently After a Side Channel Attack.
* 		Daniel P. Martin, Jonathan F. O'Connell, Elisabeth Oswald, Martijn Stam
* 		IACR Cryptology ePrint Archive 2015: 689 (2015)
*
* with improvements from:
*
*		Characterisation and Estimation of the Key Rank Distribution in the Context of Side Channel Evaluations.
* 		Daniel P. Martin, Luke Mather, Elisabeth Oswald, Martijn Stam
*		IACR Cryptology ePrint Archive 2016: 491 (2016)
*
* The integer representation of the distinguishing scores should ideally be positive integers.  Technically, as long as the
* sum of the integer scores for the correct key is non-zero, the algorithm is correct, but for safety it is likely easier
* to ensure all values are >= 1.
*
* IMPORTANT - definitions of 'rank':
* 		- The rank is defined as the number of keys with a better distinguishing score.  Thus, a perfect attack is rank 0.
* 		- In the case of ties, the number of ties do not count towards the rank.  E.g. if three keys, including the true one, have the
* 		  same (best) distinguishing score, then the rank of the correct key is still 0, and is not 3.
*
*/
public class PathCountRank {

	/**
	 *
	 * Estimates the rank of a key.
	 *
	 * Both the fastest and smallest memory footprint implementation.
	 *
	 * Optimises by only checking the nodes corresponding to weight = 0 in the final distinguishing vector
	 *
	 * @param key the known key
	 * @param weightTable an integer representation of the distinguishing scores
	 * @return the estimated rank of the key
	 * @throws IllegalArgumentException
	 */
	public static final BigInteger rank(Key key, WeightTable weightTable) {
		final int keyWeight = weightTable.weightForKey(key);
		if(keyWeight == 0) {
			throw new IllegalArgumentException("The weight for the known key must be > 0.");
		}

		return rank(keyWeight, weightTable);
	}
	
	/**
	 *
	 * Estimates the rank of a *weight*, rather than a key.  Finding the rank of a key is equivalent to finding the rank of its
	 * weight.
	 *
	 * Both the fastest and smallest memory footprint implementation.
	 *
	 * Optimises by only checking the nodes corresponding to weight = 0 in the final distinguishing vector
	 *
	 * @param maxWeight the weight to be ranked up to.  In standard 'rank' problems, this value should be equivalent to the weight
	 * of the known key.  In enumeration problems, this may be any bound.
	 * @param weightTable an integer representation of the distinguishing scores
	 * @return the estimated rank of the weight
	 * @throws IllegalArgumentException
	 */
	public static final BigInteger rank(int maxWeight, WeightTable weightTable) {
		if(maxWeight == 0) {
			throw new IllegalArgumentException("The weight to be ranked must be > 0.");
		}

		PathCountGraph graph = new PathCountGraph(weightTable.getVectorCount(), maxWeight);

		for(int vectorIndex = weightTable.getVectorCount() ; vectorIndex > 1 ; vectorIndex--) {
			for(int subkeyIndex = weightTable.getVectorSize() ; subkeyIndex > 0 ; subkeyIndex--) {
				for(int weightIndex = maxWeight ; weightIndex > 0 ; weightIndex--) {
					final GraphCoordinate coord = new GraphCoordinate(vectorIndex - 1, subkeyIndex - 1, weightIndex - 1);
					final GraphCoordinate rightChildIndex = graph.rightChildIndex(coord, weightTable);
					graph.set(coord, graph.rightChild(rightChildIndex));
				}
			}
			graph.rotateBuffers();
		}
		// Can skip all but nodes with weight 0 in the last vector
		for(int subkeyIndex = weightTable.getVectorSize() ; subkeyIndex > 0 ; subkeyIndex--) {
			final GraphCoordinate coord = new GraphCoordinate(0, subkeyIndex - 1, 0);
			final GraphCoordinate rightChildIndex = graph.rightChildIndex(coord, weightTable);
			graph.set(coord, graph.rightChild(rightChildIndex));
		}
		return graph.first();
	}
}

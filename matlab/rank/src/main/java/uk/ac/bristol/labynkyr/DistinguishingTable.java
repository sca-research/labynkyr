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

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

/**
*
* A DistinguishingTable contains the distinguishing scores associated with all the DPA attacks associated with the recovery of a full
* key.  In this implementation we assume each attack targets the same size portion of the key (e.g. 16 8-bit SubBytes attack on AES,
* and not 12 8-bit SubBytes and one 32-bit MixColumns attack)
*
* The scores are stored internally in a single vector, with the scores for the first distinguishing vector stored first in the buffer,
* the second distinguishing vector in the next portion of the buffer, and so on.
* 
*/
public class DistinguishingTable {

	private final int vectorCount;
	private final int vectorSizeBits;
	private final int vectorSize;
	private List<Double> scores;
	
	/**
	 *
	 * @param vectorCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
	 * @param vectorSizeBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
	 * @param scores construct a distinguishing table by passing in a vector of all the distinguishing scores
	 * @throws IllegalArgumentException
	 */
	public DistinguishingTable(int vectorCount, int vectorSizeBits, List<Double> scores) {
		this.vectorCount = vectorCount;
		this.vectorSizeBits = vectorSizeBits;
		this.vectorSize = 1 << vectorSizeBits;
		this.scores = scores;
		final int requiredSpace = vectorSize * vectorCount;
		if(requiredSpace != scores.size()) {
			throw new IllegalArgumentException("Required to supply scores array of length " + requiredSpace + " elements." +
				" Supplied length was " + scores.size() + " elements");
		}
	}
	
	/**
	 *
	 * @param vectorIndex
	 * @param subkeyIndex
	 * @return the distinguishing score associated with the subkeyIndex subkey in the vectorIndex distinguishing vector
	 */
	public final double score(int vectorIndex, int subkeyIndex) {
		return scores.get(vectorIndex * vectorSize + subkeyIndex);
	}
	
	/**
	 *
	 * Normalise each distinguishing vector in the table such that each vector (not the whole table) sums to 1.0.  This
	 * method assumes that each distinguishing score is already positive.
	 */
	public void normaliseDistinguishingVectors() {
		for(int vectorIndex = 0 ; vectorIndex < vectorCount ; vectorIndex++) {
			// Compute sum
			double sum = 0.0;
			for(int subkeyIndex = 0 ; subkeyIndex < vectorSize ; subkeyIndex++) {
				sum += score(vectorIndex, subkeyIndex);
			}
			// Divide through
			for(int subkeyIndex = 0 ; subkeyIndex < vectorSize ; subkeyIndex++) {
				scores.set(vectorIndex * vectorSize + subkeyIndex, score(vectorIndex, subkeyIndex) / sum);
			}
		}
	}

	/**
	 * 
	 *  Apply Math.abs() to every element in the table
	 */
	public void applyAbsoluteValue() {
		for(int index = 0 ; index < scores.size() ; index++) {
			scores.set(index, Math.abs(scores.get(index)));
		}
	}

	/**
	 *
	 * Shifts the entire distinguishing table such that the minimum score in the entire table is slightly above zero.
	 */
	public void translateVectorsToPositive() {
		// Find the minimum value
		final double minValue = Collections.min(scores);
		// Minimum value is 0.0 then the vector elements are already all positive and we don't need to do anything
		if(minValue <= 0.0) {
			// If we need to shift the scores, then add a small epsilon as a fudge to ensure that no score is 0.0 after translation
			final double epsilon = 0.000001;
			for(int index = 0 ; index < scores.size() ; index++) {
				scores.set(index, scores.get(index) - (minValue - epsilon));
			}
		}
	}

	/**
	 *
	 * Replace every distinguishing score with log2(score)
	 */
	public void takeLogarithm() {
		takeLogarithm(2.0);
	}

	/**
	 *
	 * Replace every distinguishing score with log_base(score)
	 *
	 * @param logBase take the logs to this base
	 */
	public void takeLogarithm(double logBase) {
		for(int index = 0 ; index < scores.size() ; index++) {
			scores.set(index, Math.log(scores.get(index)) / Math.log(logBase));
		}
	}

	/**
	 *
	 * Copies the distinguishing table and creates a weight table.
	 *
	 * @param precisionBits the bits of precision retained when converting distinguishing scores to integer values.  ~16 is a
	 * reasonable number.  Larger values produce more accurate rank & enumeration calculations but at the cost of speed.
	 * @throws IllegalArgumentException
	 */
	public final WeightTable mapToWeight(int precisionBits) {
		if(precisionBits <= 1) {
			throw new IllegalArgumentException("Cannot run mapToWeight at less than 2 bits of precision");
		}
		
		// Find the maximum distinguishing score and compute the multiplier to each score
		final double maxScore = Collections.max(scores);
		final double alpha = Math.log(maxScore) / Math.log(2.0);
		if(Double.isInfinite(alpha) || Double.isNaN(alpha)) {
			throw new IllegalArgumentException("Maximum score is 0.0; cannot apply mapToWeight");
		}
		final double precisionMultiplier = Math.pow(2.0, (double) precisionBits - alpha);

		// Go back through the vectors, finding and setting the mapped weights
		Integer[] weights = new Integer[vectorCount * vectorSize];
		for(int index = 0 ; index < weights.length ; index++) {
			weights[index] = (int) (scores.get(index) * precisionMultiplier);
		}
		
		//
		WeightTable weightTable = new WeightTable(vectorCount, vectorSizeBits, Arrays.asList(weights));
		// There's a considerable speed improvement from translating the weights such that the most likely key has weight 1
		weightTable.rebase(1);
		return weightTable;
	}
	
	
	/**
	 *
	 * @return the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
	 */
	public final int getVectorCount() {
		return vectorCount;
	}
	
	/**
	 *
	 * @return vectorSizeBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
	 */
	public final int getVectorSizeBits() {
		return vectorSizeBits;
	}
	
	/**
	 *
	 * @param vectorSize the number of subkey candidates for a portion of the key targeted by each subkey recovery attack 
	 * (e.g 256 for 8-bit SubBytes attacks on an AES-128 key)
	 */
	public final int getVectorSize() {
		return vectorSize;
	}
	
	/**
	 *
	 * @return access to the raw scores buffer
	 */
	public final List<Double> allScores() {
		return scores;
	}
}

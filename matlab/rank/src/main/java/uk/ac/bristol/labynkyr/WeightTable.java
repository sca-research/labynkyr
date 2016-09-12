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

import java.util.Collections;
import java.util.List;

/**
*
* A WeightTable contains the integer representation of the distinguishing scores associated with all the DPA attacks with the recovery
* of a full key.  In this implementation we assume each attack targets the same size portion of the key (e.g. 16 8-bit SubBytes attack
* on AES, and not 12 8-bit SubBytes and one 32-bit MixColumns attack)
*
* A WeightTable can be created using the DistinguishingTable#mapToWeight(uint32_t precisionBits) method.
*
* The weights are stored internally in a single vector, with the weights for the first distinguishing vector stored first in the buffer,
* the second distinguishing vector in the next portion of the buffer, and so on.
*
* This class is templated on the integer type itself; if less than 32-bits or 16-bits of precision are required by the
* ranking algorithm, then it makes sense to use uint16_t or uint32_t here.
*
* Higher distinguishing scores should have lower integer weights!
*
*/
public class WeightTable {

	private final int vectorCount;
	private final int vectorSizeBits;
	private final int vectorSize;
	private List<Integer> weights;
	
	/**
	 *
	 * @param vectorCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
	 * @param vectorSizeBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
	 * @param weights construct a weights table by passing in a vector of all the weights
	 * @throws IllegalArgumentException
	 */
	public WeightTable(int vectorCount, int vectorSizeBits, List<Integer> weights) {
		this.vectorCount = vectorCount;
		this.vectorSizeBits = vectorSizeBits;
		this.vectorSize = 1 << vectorSizeBits;
		this.weights = weights;
		final int requiredSpace = vectorSize * vectorCount;
		if(requiredSpace != weights.size()) {
			throw new IllegalArgumentException("Required to supply weight array of length " + requiredSpace + " elements." +
				" Supplied length was " + weights.size() + " elements");
		}
	}
	
	/**
	 *
	 * @param vectorIndex
	 * @param subkeyIndex
	 * @return the integer weight associated with the subkeyIndex subkey in the vectorIndex distinguishing vector
	 */
	public final int weight(int vectorIndex, int subkeyIndex) {
		return weights.get(vectorIndex * vectorSize + subkeyIndex);
	}
	
	/**
	 *
	 * @param key a key candidate
	 * @return the weight associated with this key candidate (the sum of the weights for each subkey)
	 */
	public final int weightForKey(Key key) {
		int sum = 0;
		for(int vectorIndex = 0 ; vectorIndex < vectorCount ; vectorIndex++) {
			final int bitOffset = vectorIndex * vectorSizeBits;
			final int keyPortionValue = (int) key.subkeyValue(new BitWindow(bitOffset, vectorSizeBits));
			sum += weight(vectorIndex, keyPortionValue);
		}
		return sum;
	}
	
	/**
	 *
	 * The run-time of rank and enumeration algorithms is typically some function of the weight of the key.  Often, mapToWeight
	 * will produce weights where the minimum is > 1.  There is a considerable efficiency gain from translating the weights
	 * such that the minimum weight is 1.
	 *
	 * @param newMinimumWeight the minimum weight for any subkey will be set to be this value, by shifting the weights
	 */
	public void rebase(int newMinimumWeight) {
		final int minValue = Collections.min(weights);
		if(minValue >= newMinimumWeight) {
			final int shiftAmount = minValue - newMinimumWeight;
			for(int index = 0 ; index < vectorCount * vectorSize ; index++) {
				weights.set(index, weights.get(index) - shiftAmount);
			}
		} else {
			final int shiftAmount = newMinimumWeight - minValue;
			for(int index = 0 ; index < vectorCount * vectorSize ; index++) {
				weights.set(index, weights.get(index) + shiftAmount);
			}
		}
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
	 * @return access to the raw weights buffer
	 */
	public final List<Integer> allWeights() {
		return weights;
	}
}

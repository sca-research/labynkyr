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
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.codec.DecoderException;

public class Rank {


	/**
	 * 
	 * Basic Java implementation of the path-counting rank estimation algorithm of Martin et al.:
	 *
	 * 		How to Enumerate Your Keys Accurately and Efficiently After a Side Channel Attack.
	 * 		Daniel P. Martin, Jonathan F. O'Connell, Elisabeth Oswald, Martijn Stam
	 * 		ASIACRYPT (2) 2015: 313-337 (2015)
	 *
	 * with improvements from:
	 *
	 *		Characterisation and Estimation of the Key Rank Distribution in the Context of Side Channel Evaluations.
	 * 		Daniel P. Martin, Luke Mather, Elisabeth Oswald, Martijn Stam
	 *		IACR Cryptology ePrint Archive 2016: 491 (2016)
	 *
	 * The intended purpose of this Java implementation is to provide a manageable way to allow the estimation of key ranks directly from a 
	 * Matlab environment.  To this end, the functionality implemented is the minimum possible, and the code is not optimised for speed.  If 
	 * speed is important, please use the C++ implementation (or if performing a performance evaluation with a different rank estimation 
	 * algorithm).
	 *
	 * The provided distinguishing scores must meet the following criteria:
	 * 		(1) The most likely subkey candidate must have the *lowest* score;
	 * 		(2) The scores need to be additive;
	 * To meet conditions 1 & 2 using data produced by a correlation attack, consider taking the absolute value of the correlation coefficients,
	 * then taking the logarithm, and then taking the absolute value again.
	 * 
	 * To call this code from Matlab, having added the JAR to the classpath, use something similar to 
	 *		logRank = uk.ac.bristol.labynkyr.Rank.rankKey(key, precisionBits, vectorCount, vectorSizeBits, scores);
	 * where key is a standard Matlab string, precisionBits, vectorCount and vectorSizeBits are of the uint32 type, and scores is a standard Matlab
	 * double precision vector.
	 *
	 * IMPORTANT - definitions of 'rank':
	 * 		- The rank is defined as the number of keys with a better distinguishing score.  Thus, a perfect attack is rank 0.
	 * 		- In the case of ties, the number of ties do not count towards the rank.  E.g. if three keys, including the true one, have the
	 * 		  same (best) distinguishing score, then the rank of the correct key is still 0, and is not 3.
	 * 
	 * 
	 * @param keyHex the key as a hexadecimal string
	 * @param precisionBits the bits of precision retained when converting distinguishing scores to integer values.  ~16 is a
	 * reasonable number.  Larger values produce more accurate rank & enumeration calculations but at the cost of speed.  This Java implementation
	 * is not optimised and will struggle at higher values of precision.  In such cases, consider using the C++ variant of the code.
	 * @param vectorCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
	 * @param vectorSizeBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
	 * @param scores an array of the distinguishing scores.  The scores corresponding to the subkey candidates for the first distinguishing vector
	 * should appear first, the scores for the second distinguishing vector should follow, and so on
	 * @return log_2(rank) of the key, given the distinguishing scores.
	 * @throws DecoderException
	 */
	public static double logRankKey(String keyHex, int precisionBits, int vectorCount, int vectorSizeBits, double[] scores) throws DecoderException {
		final BigInteger rank = rankKey(keyHex, precisionBits, vectorCount, vectorSizeBits, scores);
		final double logRank = Math.log(rank.doubleValue()) / Math.log(2.0);
		return logRank;
	}
	
	/**
	 * 
	 * @param keyHex the key as a hexadecimal string
	 * @param precisionBits the bits of precision retained when converting distinguishing scores to integer values.  ~16 is a
	 * reasonable number.  Larger values produce more accurate rank & enumeration calculations but at the cost of speed.  This Java implementation
	 * is not optimised and will struggle at higher values of precision.  In such cases, consider using the C++ variant of the code.
	 * @param vectorCount the number of distinguishing vectors in the attack (e.g 16 for SubBytes attacks on an AES-128 key)
	 * @param vectorSizeBits the number bits of the key targeted by each subkey recovery attack (e.g 8 for SubBytes attacks on an AES-128 key)
	 * @param scores an array of the distinguishing scores.  The scores corresponding to the subkey candidates for the first distinguishing vector
	 * should appear first, the scores for the second distinguishing vector should follow, and so on
	 * @return the full rank of the key as a Java BigInteger, given the distinguishing scores.
	 * @throws DecoderException
	 */
	public static BigInteger rankKey(String keyHex, int precisionBits, int vectorCount, int vectorSizeBits, double[] scores) throws DecoderException {
		final int keyLenBits = vectorCount * vectorSizeBits;
		final Key key = new Key(keyLenBits, keyHex);

		// Convert the scores into a list and handle the boxing.
		List<Double> scoresList = new ArrayList<Double>(scores.length);
		for(int index = 0 ; index < scores.length ; index++) {
			scoresList.add(scores[index]);
		}

		final DistinguishingTable table = new DistinguishingTable(vectorCount, vectorSizeBits, scoresList);
		final WeightTable weightTable = table.mapToWeight(precisionBits);

		final BigInteger rank = PathCountRank.rank(key, weightTable);
		return rank;
	}
}

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

import static org.junit.Assert.*;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Random;

import org.junit.Test;

public class TestDistinguishingTable {

	@Test
	public void test_mapToWeight_4bits() {
		final int vectorSize = 1 << 8;
		Double[] scores = new Double[2 * vectorSize];

		// Generate random data
		Random rand = new Random();
		for(int index = 0 ; index < scores.length ; index++) {
			final double nextRand = (rand.nextDouble() * 10.0) - 5.0;
			scores[index] = nextRand;
		}
		
		DistinguishingTable table = new DistinguishingTable(2, 8, Arrays.asList(scores));
		table.translateVectorsToPositive();
		table.normaliseDistinguishingVectors();
		table.takeLogarithm(2.0);
		table.applyAbsoluteValue();

		final int precisionBits = 4;
		final WeightTable weightTable = table.mapToWeight(precisionBits);
		final int maxScore = Collections.max(weightTable.allWeights());
		assertTrue(maxScore < 16);
	}

	@Test
	public void test_mapToWeight_12bits() {
		final int vectorSize = 1 << 8;
		Double[] scores = new Double[2 * vectorSize];

		// Generate random data
		Random rand = new Random();
		for(int index = 0 ; index < scores.length ; index++) {
			final double nextRand = (rand.nextDouble() * 14.0) - 12.0;
			scores[index] = nextRand;
		}

		DistinguishingTable table = new DistinguishingTable(2, 8, Arrays.asList(scores));
		table.translateVectorsToPositive();
		table.normaliseDistinguishingVectors();
		table.takeLogarithm(2.0);
		table.applyAbsoluteValue();

		final int precisionBits = 12;
		final WeightTable weightTable = table.mapToWeight(precisionBits);
		final int maxScore = Collections.max(weightTable.allWeights());
		assertTrue(maxScore < 4096);
	}
	
	@Test
	public void test_normaliseDistinguishingVectors() {
		final int vectorSize = 1 << 8;
		Double[] scores = new Double[2 * vectorSize];

		// Generate random data
		Random rand = new Random();
		for(int index = 0 ; index < scores.length ; index++) {
			final double nextRand = (rand.nextDouble() * 10.0) - 5.0;
			scores[index] = nextRand;
		}

		DistinguishingTable table = new DistinguishingTable(2, 8, Arrays.asList(scores));
		table.normaliseDistinguishingVectors();

		// Actual
		double sum1 = 0.0;
		for(int index = 0 ; index < vectorSize ; index++) {
			sum1 += table.allScores().get(index);
		}
		double sum2 = 0.0;
		for(int index = vectorSize ; index < 2 * vectorSize ; index++) {
			sum2 += table.allScores().get(index);
		}
		assertEquals(1.0, sum1, 0.0001);
		assertEquals(1.0, sum2, 0.0001);
	}

	@Test
	public void test_applyAbsoluteValue() {
		final int vectorSize = 1 << 8;
		Double[] scores = new Double[2 * vectorSize];

		// Generate random data
		Random rand = new Random();
		for(int index = 0 ; index < scores.length ; index++) {
			final double nextRand = (rand.nextDouble() * 10.0) - 5.0;
			scores[index] = nextRand;
		}

		DistinguishingTable table = new DistinguishingTable(2, 8, Arrays.asList(scores));
		table.applyAbsoluteValue();

		final double minElement = Collections.min(table.allScores());
		final double maxElement = Collections.max(table.allScores());

		assertTrue(minElement >= 0.0);
		assertTrue(maxElement <= 5.0);
	}

	@Test
	public void test_translateVectorsToPositive() {
		final int vectorSize = 1 << 8;
		Double[] scores = new Double[2 * vectorSize];

		// Generate random data
		Random rand = new Random();
		for(int index = 0 ; index < scores.length ; index++) {
			final double nextRand = (rand.nextDouble() * 10.0) - 5.0;
			scores[index] = nextRand;
		}

		DistinguishingTable table = new DistinguishingTable(2, 8, Arrays.asList(scores));
		table.translateVectorsToPositive();

		final double minElement = Collections.min(table.allScores());
		final double maxElement = Collections.max(table.allScores());

		assertEquals(0.0, minElement, 0.00001);
		assertTrue(maxElement > 9.0);
	}

	@Test
	public void test_translateVectorsToPositive_alreadyPositive() {
		final int vectorSize = 1 << 8;
		Double[] scores = new Double[2 * vectorSize];

		// Generate random data
		Random rand = new Random();
		for(int index = 0 ; index < scores.length ; index++) {
			final double nextRand = (rand.nextDouble() * 4.0) + 1.0;
			scores[index] = nextRand;
		}

		DistinguishingTable table = new DistinguishingTable(2, 8, Arrays.asList(scores));
		table.translateVectorsToPositive();

		final double minElement = Collections.min(table.allScores());
		final double maxElement = Collections.max(table.allScores());

		assertTrue(minElement >= 1.0);
		assertTrue(maxElement <= 5.0);
	}


	@Test
	public void test_takeLogarithm() {
		final int vectorSize = 1 << 8;
		Double[] scores = new Double[2 * vectorSize];
		Double[] expected = new Double[2 * vectorSize];

		// Generate random data
		Random rand = new Random();
		for(int index = 0 ; index < scores.length ; index++) {
			final double nextRand = (rand.nextDouble() * 4.0) + 1.0;
			scores[index] = nextRand;
			expected[index] = Math.log(nextRand) / Math.log(2.0);
		}
		
		DistinguishingTable table = new DistinguishingTable(2, 8, Arrays.asList(scores));
		table.takeLogarithm(2.0);
		
		for(int index = 0 ; index < 2 * vectorSize ; index++) {
			assertEquals(expected[index], table.allScores().get(index), 0.0001);
		}
	}

	@Test
	public void test_score() {
		final List<Double> scores = Arrays.asList(1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8);
		final DistinguishingTable table = new DistinguishingTable(2, 2, scores);

		assertEquals(1.1, table.score(0, 0), 0.0001);
		assertEquals(2.2, table.score(0, 1), 0.0001);
		assertEquals(3.3, table.score(0, 2), 0.0001);
		assertEquals(4.4, table.score(0, 3), 0.0001);
		assertEquals(5.5, table.score(1, 0), 0.0001);
		assertEquals(6.6, table.score(1, 1), 0.0001);
		assertEquals(7.7, table.score(1, 2), 0.0001);
		assertEquals(8.8, table.score(1, 3), 0.0001);
	}
}

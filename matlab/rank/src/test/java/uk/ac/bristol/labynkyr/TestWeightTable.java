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

import static org.junit.Assert.assertEquals;

import java.util.Arrays;
import java.util.List;

import org.apache.commons.codec.DecoderException;
import org.junit.Test;

public class TestWeightTable {
	
	@Test
	public void test_weight() {
		final List<Integer> weights = Arrays.asList(3, 4, 6, 7, 0, 1, 3, 4);
		final WeightTable weightTable = new WeightTable(2, 2, weights);
		assertEquals(3, weightTable.weight(0, 0));
		assertEquals(4, weightTable.weight(0, 1));
		assertEquals(6, weightTable.weight(0, 2));
		assertEquals(7, weightTable.weight(0, 3));
		assertEquals(0, weightTable.weight(1, 0));
		assertEquals(1, weightTable.weight(1, 1));
		assertEquals(3, weightTable.weight(1, 2));
		assertEquals(4, weightTable.weight(1, 3));
	}
	
	@Test
	public void test_allWeights() {
		final List<Integer> weights = Arrays.asList(3, 4, 6, 7, 0, 1, 3, 4);
		final WeightTable weightTable = new WeightTable(2, 2, weights);
		assertEquals(weights, weightTable.allWeights());
	}

	@Test
	public void test_weightForKey_6bits() throws DecoderException {
		final List<Integer> weights = Arrays.asList(4, 3, 1, 1, 	6, 4, 3, 1,  5, 7, 8, 9);
		final WeightTable weightTable = new WeightTable(3, 2, weights);
		final Key secretKey = new Key(6, "09");
		final int expected = 3 + 3 + 5;
		assertEquals(expected, weightTable.weightForKey(secretKey));
	}

	@Test
	public void test_weightForKey_36bits() throws DecoderException {
		Integer[] buffer = new Integer[3 * 4096];
		List<Integer> weights = Arrays.asList(buffer);
		weights.set(0 * 4096 + 513, 3);
		weights.set(1 * 4096 + 48, 3);
		weights.set(2 * 4096 + 772, 5);
		final WeightTable weightTable = new WeightTable(3, 12, weights);
		final Key secretKey = new Key(36, "0102030403");
		final int expected = 3 + 3 + 5;
		assertEquals(expected, weightTable.weightForKey(secretKey));
	}
	
	@Test
	public void test_rebase_minus_0() {
		final List<Integer> weights = Arrays.asList(9, 3, 4, 1, 	6, 4, 3, 1, 	5, 7, 4, 1);
		WeightTable weightTable = new WeightTable(3, 2, weights);
		weightTable.rebase(0);

		final List<Integer> expected = Arrays.asList(8, 2, 3, 0, 	5, 3, 2, 0, 	4, 6, 3, 0);
		assertEquals(expected, weightTable.allWeights());
	}

	@Test
	public void test_rebase_minus_0_v2() {
		final List<Integer> weights = Arrays.asList(10, 4, 5, 2, 	7, 5, 4, 2, 	6, 8, 5, 2);
		WeightTable weightTable = new WeightTable(3, 2, weights);
		weightTable.rebase(0);

		final List<Integer> expected = Arrays.asList(8, 2, 3, 0, 	5, 3, 2, 0, 	4, 6, 3, 0);
		assertEquals(expected, weightTable.allWeights());
	}

	@Test
	public void test_rebase_minus_1() {
		final List<Integer> weights = Arrays.asList(9, 3, 4, 2, 	6, 4, 3, 2, 	5, 7, 4, 2);
		WeightTable weightTable = new WeightTable(3, 2, weights);
		weightTable.rebase(1);

		final List<Integer> expected = Arrays.asList(8, 2, 3, 1, 	5, 3, 2, 1, 	4, 6, 3, 1);
		assertEquals(expected, weightTable.allWeights());
	}

	@Test
	public void test_rebase_minus_1_v2() {
		final List<Integer> weights = Arrays.asList(10, 4, 5, 3, 	7, 5, 4, 3, 	6, 8, 5, 3);
		WeightTable weightTable = new WeightTable(3, 2, weights);
		weightTable.rebase(1);

		final List<Integer> expected = Arrays.asList(8, 2, 3, 1, 	5, 3, 2, 1, 	4, 6, 3, 1);
		assertEquals(expected, weightTable.allWeights());
	}

	@Test
	public void test_rebase_addition_1() {
		final List<Integer> weights = Arrays.asList(9, 3, 4, 0, 	6, 4, 3, 0, 	5, 7, 4, 0);
		WeightTable weightTable = new WeightTable(3, 2, weights);
		weightTable.rebase(1);

		final List<Integer> expected = Arrays.asList(10, 4, 5, 1, 	7, 5, 4, 1, 	6, 8, 5, 1);
		assertEquals(expected, weightTable.allWeights());
	}

	@Test
	public void test_rebase_addition_3() {
		final List<Integer> weights = Arrays.asList(10, 4, 5, 1, 	7, 5, 4, 1, 	6, 8, 5, 1);
		WeightTable weightTable = new WeightTable(3, 2, weights);
		weightTable.rebase(3);

		final List<Integer> expected = Arrays.asList(12, 6, 7, 3, 	9, 7, 6, 3, 	8, 10, 7, 3);
		assertEquals(expected, weightTable.allWeights());
	}

}

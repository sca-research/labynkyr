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

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;

import org.apache.commons.codec.DecoderException;
import org.junit.Test;

public class TestKey {
	
	@Test
	public void test_hexStringConstructor_getLengthBits() throws DecoderException {
		final Key key = new Key(40, "0001020304");
		assertEquals(40, key.getLengthBits());
	}

	@Test
	public void test_hexStringConstructor_asBytes1() throws DecoderException {
		final byte[] expectedBytes = {0x00, 0x01, 0x02, 0x03, 0x04};
		final Key key = new Key(40, "0001020304");
		assertArrayEquals(expectedBytes, key.asBytes());
	}

	@Test
	public void test_hexStringConstructor_asBytes() throws DecoderException {
		final byte[] expectedBytes = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
		final Key key = new Key(64, "0001020304050607");
		assertArrayEquals(expectedBytes, key.asBytes());
	}

	@Test
	public void test_hexStringConstructor_asBytes3() throws DecoderException {
		final byte[] expectedBytes = {0x05};
		final Key key = new Key(4, "05");
		assertArrayEquals(expectedBytes, key.asBytes());
	}

	@Test(expected = DecoderException.class)
	public void test_hexStringConstructor_invalidStringSize1() throws DecoderException {
		new Key(40, "000102030");
	}

	@Test(expected = DecoderException.class)
	public void test_hexStringConstructor_invalidStringSize2() throws DecoderException {
		new Key(40, "00010203040");
	}

	@Test(expected = IllegalArgumentException.class)
	public void test_hexStringConstructor_invalidStringSize3() throws DecoderException {
		new Key(16, "00");
	}

	@Test(expected = IllegalArgumentException.class)
	public void test_hexStringConstructor_invalidStringSize4() throws DecoderException {
		new Key(16, "FFFFFF");
	}

	@Test(expected = IllegalArgumentException.class)
	public void test_hexStringConstructor_invalidStringSize5() throws DecoderException {
		new Key(16, "");
	}
	
	@Test
	public void test_subkeyValue_endBytes() throws DecoderException {
		final Key key = new Key(48, "07070000009F");
		final long actual = key.subkeyValue(new BitWindow(16, 32));
		assertEquals(2667577344L, actual);
	}

	@Test
	public void test_subkeyValue_1() throws DecoderException {
		final Key key = new Key(128, "07070707070707070707070707070707");
		final long actual = key.subkeyValue(new BitWindow(0, 8));
		assertEquals(7, actual);
	}

	@Test
	public void test_subkeyValue_twoBytes() throws DecoderException {
		final Key key = new Key(32, "01020304");
		final long actual = key.subkeyValue(new BitWindow(8, 16));
		assertEquals(770, actual);
	}

	@Test
	public void test_subkeyValue_smallBits() throws DecoderException {
		final Key key = new Key(32, "01020304");
		final long actual = key.subkeyValue(new BitWindow(0, 1));
		assertEquals(1, actual);
	}

	@Test
	public void test_subkeyValue_truncated() throws DecoderException {
		final Key key = new Key(11, "6502");
		// Bits 6, 7, 8 and 9 (so last 2 from the first byte, and the first 2 from the second byte) = 0b1001
		final long actual = key.subkeyValue(new BitWindow(6, 4));
		assertEquals(9, actual);
	}
	
	@Test(expected = IllegalArgumentException.class)
	public void test_subkeyValue_invalidLengths1() throws DecoderException {
		final Key key = new Key(32, "01020304");
		key.subkeyValue(new BitWindow(32, 1));
	}

	@Test(expected = IllegalArgumentException.class)
	public void test_subkeyValue_invalidLengths2() throws DecoderException {
		final Key key = new Key(32, "01020304");
		key.subkeyValue(new BitWindow(31, 2));
	}

	@Test(expected = IllegalArgumentException.class)
	public void test_subkeyValue_invalidLengths3() throws DecoderException {
		final Key key = new Key(32, "01020304");
		key.subkeyValue(new BitWindow(0, 33));
	}
}

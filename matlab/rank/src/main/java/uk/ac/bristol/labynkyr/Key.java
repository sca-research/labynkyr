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

import org.apache.commons.codec.DecoderException;
import org.apache.commons.codec.binary.Hex;

/**
*
* Class for storing and manipulating key values for use in rank calculation algorithms.
*
*/
public class Key {

	private final int lengthBits;
	private final byte[] bytes;
	
	/**
	 *
	 * @param lengthBits the length of the key in bits
	 * @param hexString the key as a hex string.  The string is assumed to be little endian.
	 * @throws DecoderException
	 * @throws IllegalArgumentException if the string is not of the required length specified by the key size
	 */
	public Key(int lengthBits, String hex) throws DecoderException {
		this.lengthBits = lengthBits;
		bytes = Hex.decodeHex(hex.toCharArray());
		final int bitRemainder = lengthBits % 8;
		final int byteCount = (bitRemainder == 0) ? lengthBits / 8 : (lengthBits / 8) + 1;
		if(bytes.length != byteCount) {
			throw new IllegalArgumentException("Key specified to have length " + lengthBits + " bits, but supplied hex string only " 
					+ "specifies " + bytes.length + " bytes");
		}
	}
	
	/**
	 *
	 * @param subkeyPortion the portion of the key to extract the subkey value from
	 * @return the integer value of the portion of the key specified by the subkeyPortion
	 * @throws IllegalArgumentException
	 */
	public final long subkeyValue(BitWindow subkeyPortion) {
		if(subkeyPortion.getBitEnd() >= bytes.length * 8) {
			throw new IllegalArgumentException("Cannot extract subkey value for invalid bit window [" + subkeyPortion.getBitStart() + ", "
					+ subkeyPortion.getBitEnd() + "] given a key of length " + (bytes.length * 8) + " bits");
		}
		long value = 0;
		int stateBitIndex = 0;
		for(int bit = subkeyPortion.getBitStart() ; bit <= subkeyPortion.getBitEnd() ; bit++) {
			final int byteIndex = bit / 8;
			final int bitOffset = bit % 8;
			final long bitValue = (bytes[byteIndex] & (1 << bitOffset)) >> bitOffset;
			value |= (bitValue << stateBitIndex);
			stateBitIndex++;
		}
		return value;
	}
	
	/**
	 * 
	 * @return the length of the key in bits
	 */
	public final int getLengthBits() {
		return lengthBits;
	}
	
	/**
	 *
	 * @return a little endian byte representation of the key
	 */
	public final byte[] asBytes() {
		return bytes;
	}
}

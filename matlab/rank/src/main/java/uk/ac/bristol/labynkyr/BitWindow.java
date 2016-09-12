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
 * BitWindow is used to define a continuous window of bits within an encryption key or some cipher material.  For instance,
 * if one was to try a DPA attack on the first byte of a key, then the BitWindow would be defined as the bit index 0
 * and then a further offset of 8 bits.
 *
 */
public class BitWindow {
	
	private final int bitStart;
	private final int bitCount;
	private final int bitEnd;
	
	/**
	 *
	 * @param bitStart the starting bit index of the window.  Indexes are taken left-to-right over an array; bit index 0 is the
	 * left-most bit of array[0].
	 * @param bitCount the size of the window beginning at bitStart.  Must be > 0.
	 * @throws IllegalArgumentException
	 */
	public BitWindow(int bitStart, int bitCount) {
		this.bitStart = bitStart;
		this.bitCount = bitCount;
		this.bitEnd = bitStart + bitCount - 1;
		if(bitCount == 0) {
			throw new IllegalArgumentException("Cannot construct a BitLocation with a window size of 0.");
		}
	}
	
	/**
	 *
	 * @return the starting bit index of this window
	 */
	public final int getBitStart() {
		return bitStart;
	}
	
	/**
	 *
	 * @return the size (number of bits) of the window
	 */
	public final int getBitCount() {
		return bitCount;
	}
	
	/**
	 *
	 * @return the final bit index associated with this window
	 */
	public final int getBitEnd() {
		return bitEnd;
	}
}

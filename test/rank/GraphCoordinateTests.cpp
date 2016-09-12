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
/*
 * GraphCoordinateTests.cpp
 *
 */

#include "src/labynkyr/rank/GraphCoordinate.hpp"

#include <unittest++/UnitTest++.h>

#include <stdint.h>

namespace labynkyr {
namespace rank {

TEST(GraphCoordinate_getters) {
	GraphCoordinate const coord(4, 5, 6);
	CHECK_EQUAL(4, coord.getVectorIndex());
	CHECK_EQUAL(5, coord.getSubkeyIndex());
	CHECK_EQUAL(6, coord.getWeightIndex());
}

TEST(GraphCoordinate_copyConstructor) {
	GraphCoordinate const coord(4, 5, 6);
	GraphCoordinate const copyCoord(coord);
	CHECK_EQUAL(4, copyCoord.getVectorIndex());
	CHECK_EQUAL(5, copyCoord.getSubkeyIndex());
	CHECK_EQUAL(6, copyCoord.getWeightIndex());
}

TEST(GraphCoordinate_copyConstructor_accept) {
	GraphCoordinate const coord(true);
	GraphCoordinate const copyCoord(coord);
	CHECK_EQUAL(true, copyCoord.isAccept());
	CHECK_EQUAL(false, copyCoord.isReject());
}

TEST(GraphCoordinate_copyConstructor_reject) {
	GraphCoordinate const coord(false);
	GraphCoordinate const copyCoord(coord);
	CHECK_EQUAL(false, copyCoord.isAccept());
	CHECK_EQUAL(true, copyCoord.isReject());
}

TEST(GraphCoordinate_accept) {
	GraphCoordinate const coord(true);
	CHECK_EQUAL(true, coord.isAccept());
	CHECK_EQUAL(false, coord.isReject());
}

TEST(GraphCoordinate_reject) {
	GraphCoordinate const coord(false);
	CHECK_EQUAL(false, coord.isAccept());
	CHECK_EQUAL(true, coord.isReject());
}

TEST(GraphCoordinate_createAccept) {
	GraphCoordinate const coord = GraphCoordinate::acceptCoord();
	CHECK_EQUAL(true, coord.isAccept());
	CHECK_EQUAL(false, coord.isReject());
}

TEST(GraphCoordinate_createReject) {
	GraphCoordinate const coord = GraphCoordinate::rejectCoord();
	CHECK_EQUAL(false, coord.isAccept());
	CHECK_EQUAL(true, coord.isReject());
}

} /* namespace rank */
} /* namespace labynkyr */




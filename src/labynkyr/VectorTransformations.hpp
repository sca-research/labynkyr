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
 * VectorTransformations.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_VECTORTRANSFORMATIONS_HPP_
#define LABYNKYR_SRC_LABYNKYR_VECTORTRANSFORMATIONS_HPP_

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

namespace labynkyr {

/**
 *
 * Static methods for manipulating distinguishing vector values.
 */
template<typename ScoresType>
class VectorTransformations {
public:
	using ScoresIter = typename std::vector<ScoresType>::iterator;
	using ScoresConstIter = typename std::vector<ScoresType>::const_iterator;

	/**
	 *
	 * @param scoresBegin
	 * @param scoresEnd
	 * @return the summation of the scores defined by the iterators, using the Kahan summation algorithm
	 * (https://en.wikipedia.org/wiki/Kahan_summation_algorithm)
	 */
	static ScoresType kahanSummation(ScoresConstIter scoresBegin, ScoresConstIter scoresEnd) {
		ScoresType sum = 0.0;
		ScoresType sumC = 0.0;

		std::for_each(
			scoresBegin,
			scoresEnd,
			[&sum,&sumC](ScoresType const & score) {
				ScoresType const sumY = score - sumC;
				ScoresType const sumT = sum + sumY;
				sumC = (sumT - sum) - sumY;
				sum = sumT;
			}
		);

		return sum;
	}

	/**
	 *
	 * Normalises the scores defined by the iterators, such that the sum of the scores is 1.0
	 *
	 * @param scoresBegin
	 * @param scoresEnd
	 */
	static void normalise(ScoresIter scoresBegin, ScoresIter scoresEnd) {
		ScoresType const vectorSum = kahanSummation(scoresBegin, scoresEnd);
		ScoresType const multiplyConstant = static_cast<ScoresType>(1.0) / vectorSum;

		std::transform(
			scoresBegin, scoresEnd, scoresBegin,
			[&multiplyConstant](ScoresType const & score) {
				return score * multiplyConstant;
			}
		);
	}

	/**
	 *
	 * Applies the abs() operator to each element specified by the iterators
	 *
	 * @param scoresBegin
	 * @param scoresEnd
	 */
	static void absoluteValue(ScoresIter scoresBegin, ScoresIter scoresEnd) {
		std::transform(
			scoresBegin, scoresEnd, scoresBegin,
			[](ScoresType const & score) {
				return std::fabs(score);
			}
		);
	}

	/**
	 *
	 * Applies the log operator to each element specified by the iterators
	 *
	 * @param scoresBegin
	 * @param scoresEnd
	 * @param base the base the logarithms will be taken to
	 */
	static void logarithm(ScoresIter scoresBegin, ScoresIter scoresEnd, ScoresType base) {
		std::transform(
			scoresBegin, scoresEnd, scoresBegin,
			[&base](ScoresType const & score) {
				return std::log(score) / std::log(base);
			}
		);
	}
};

} /*namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_VECTORTRANSFORMATIONS_HPP_ */

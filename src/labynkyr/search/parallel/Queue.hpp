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
 * Queue.hpp
 *
 */

#ifndef LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_QUEUE_HPP_
#define LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_QUEUE_HPP_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace labynkyr {
namespace search {

/**
 *
 * Implementation of a thread-safe non-blocking queue.
 *
 * @tparam T
 */
template<typename T>
class Queue {
public:
	Queue() {}

	virtual ~Queue() {}

	/**
	 *
	 * @param obj
	 */
	void put(std::unique_ptr<T> obj) {
        std::unique_lock<std::mutex> lock(mutex);
        internalQueue.push(std::move(obj));
        queueEmpty.notify_one();
	}

	/**
	 *
	 * @return
	 */
	std::unique_ptr<T> nonBlockingTake() {
        std::unique_lock<std::mutex> lock(mutex);
        if(internalQueue.empty()) {
        	return std::unique_ptr<T>();
        } else {
        	std::unique_ptr<T> item = std::move(internalQueue.front());
        	internalQueue.pop();
        	return std::move(item);
        }
	}

	bool isEmpty() const {
		std::unique_lock<std::mutex> lock(mutex);
		return internalQueue.empty();
	}
private:
	std::queue<std::unique_ptr<T>> internalQueue;
    mutable std::mutex mutex;
    std::condition_variable queueEmpty;
};

} /* namespace search */
} /* namespace labynkyr */

#endif /* LABYNKYR_SRC_LABYNKYR_SEARCH_PARALLEL_QUEUE_HPP_ */

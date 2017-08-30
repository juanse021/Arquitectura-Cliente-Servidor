#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H


#include <queue>
#include <mutex>
#include <thread>
#include <iostream>
#include <condition_variable>

template <class T>

class SafeQueue {

	private:
		queue<T> q;
		std::mutex __mtx;
		std::condition_variable cv;

	
	public:
		SafeQueue() : q(), __mtx(), cv();

		void push(T element) {
			std::lock_guard<std::mutex> lock(__mtx);
			q.push(element);
		}

		T front() {
			std::unique_lock<std::mutex> lock(__mtx);

			while (q.empty())
				cv.wait(lock);

			T element = qq.front();
			q.pop();
			return element;
		}

		bool empty() {
			return q.empty();
		}

		~SafeQueue();
	
};

#endif
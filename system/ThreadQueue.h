#ifndef _THREAD_QUEUE_H
#define _THREAD_QUEUE_H

#include "common.h"
#include "Worker.h"
#include <limits>

class ThreadQueue {
	private:
		Worker *head;
		Worker *tail;
		uint64_t len;
	public:
		ThreadQueue()
			: head (NULL)
			, tail (NULL)
			, len(0) {}

		void enqueue(Worker *t);
		Worker *dequeue();
		Worker *dequeueLazy();
		Worker *front() const;
		void prefetch() const;
		uint64_t length() const {
			return len;
		}

		bool empty() {
			return (head==NULL);
		}
};

template< typename T >
T _max(const T& a, const T& b) {
	return (a > b) ? a : b;
}

template< typename T >
T _min(const T& a, const T& b) {
	return (a < b) ? a : b;
}

class PrefetchingThreadQueue {
	private:
		ThreadQueue *queues;
		uint64_t eq_index;
		uint64_t dq_index;
		uint64_t num_queues;
		uint64_t len;

		void check_invariants() {
			uint64_t max = std::numeric_limits<uint64_t>::min();
			uint64_t min = std::numeric_limits<uint64_t>::max();
			uint64_t sum = 0;
			for (uint64_t i = 0; i < num_queues; i++) {
				max = _max<uint64_t>(max, queues[i].length());
				min = _min<uint64_t>(min, queues[i].length());
				sum += queues[i].length();
			}
			if (static_cast<int64_t>(max)-static_cast<int64_t>(min)<0) {
				exit(-1);
			}
			if (max-min>1) {
				std::cerr << "min = " << min << " max = " << max << std::endl;
				exit(-1);
			}
			if (sum!=len) {
				exit(-1);
			}
		}
	public:
		PrefetchingThreadQueue()
			: queues(NULL)
			, eq_index(0)
			, dq_index(0)
			, num_queues(0)
			, len(0) {}
		
		void init(uint64_t prefetchDistance) {
#ifdef USING_MALLOC
			queues= (ThreadQueue*)malloc(sizeof(ThreadQueue)*prefetchDistance*2);
#else
			queues = new ThreadQueue[prefetchDistance*2];
#endif
			num_queues = prefetchDistance*2;
		}

		uint64_t length() const {
			return len;
		}

		void enqueue(Worker *t) {
			queues[eq_index].enqueue(t);
			eq_index = ((eq_index+1)==num_queues) ? 0 : eq_index+1;
			len++;
		}

		Worker *dequeue() {
			Worker *result = queues[dq_index].dequeueLazy();
			if (result) {
				uint64_t t = dq_index;
				dq_index = ((dq_index+1)==num_queues) ? 0 : dq_index+1;
				len--;
#ifdef WORKER_PREFETCH
				__builtin_prefetch(result->next, 1, WORKER_PREFETCH_LOCALITY);
#endif
				result->next = NULL;

				uint64_t tstack = (t+(num_queues/2))%num_queues;
				Worker *tstack_worker = queues[tstack].front();
				if (tstack_worker) {
#ifdef STACK_PREFETCH
					__builtin_prefetch(tstack_worker->stack, 1, STACK_PREFETCH_LOCALITY);
					//__builtin_prefetch(((char*)(tstack_worker->stack))+64, 1, STACK_PREFETCH_LOCALITY);
					//__builtin_prefetch(((char*)(tstack_worker->stack))+128, 1, STACK_PREFETCH_LOCALITY);

#endif
				}
				return result;
			} else {
				return NULL;
			}
		}
};

inline Worker *ThreadQueue::dequeue() {
	Worker *result = head;
	if (result != NULL) {
		head = result->next;
		result->next = NULL;
		len--;
	} else {
		tail = NULL;
	}
	return NULL;
}

inline Worker *ThreadQueue::dequeueLazy() {
	Worker *result = head;
	if (result != NULL) {
		head = result->next;
		len--;
	} else {
		tail = NULL;
	}
	return result;
}

inline void ThreadQueue::enqueue(Worker *t) {
	if (head == NULL) {
		head = t;
	} else {
		tail->next = t;
	}
	tail = t;
	t->next = NULL;
	len++;
}

inline Worker *ThreadQueue::front() const {
	return head;
}

#endif //_THREAD_QUEUE_H


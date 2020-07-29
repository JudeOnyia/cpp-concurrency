#ifndef THREADPOOLHPP
#define THREADPOOLHPP
#include "ra/queue.hpp"
#include <cstddef>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <utility>


namespace ra::concurrency {

	// Thread pool class.
	class thread_pool
	{
		public:

		// An unsigned integral type used to represent sizes.
		using size_type = std::size_t;

		using func = std::function<void()>;
		using my_queue = typename ra::concurrency::queue<func>;

		// Function performed by each thread
		friend void worker(thread_pool*);

		// Creates a thread pool with the number of threads equal to the
		// hardware concurrency level (if known); otherwise the number of
		// threads is set to 2.
		thread_pool() : allThreadsMade_(false), shutDown_(false), shutDownFinished_(false), queue_(64) {
			size_type num_threads = std::thread::hardware_concurrency();
			if(num_threads == size_type(0)){ num_threads = size_type(2); }
			num_threads_ = num_threads;
			void worker(thread_pool*);
			for(size_type i=0; i<num_threads; ++i){
				workers.emplace_back(worker,this);
			}
			std::scoped_lock<std::mutex> lock(m_);
			allThreadsMade_ = true;
			c_task_.notify_one();
		}

		// Creates a thread pool with num_threads threads.
		// Precondition: num_threads > 0
		thread_pool(std::size_t num_threads) : allThreadsMade_(false), shutDown_(false), shutDownFinished_(false), queue_(64),num_threads_(num_threads) {
			void worker(thread_pool*);
			for(size_type i=0; i<num_threads; ++i){
				workers.emplace_back(worker,this);
			}
			std::scoped_lock<std::mutex> lock(m_);
			allThreadsMade_ = true;
			c_task_.notify_one();
		}

		// A thread pool is not copyable or movable.
		thread_pool(const thread_pool&) = delete;
		thread_pool& operator=(const thread_pool&) = delete;
		thread_pool(thread_pool&&) = delete;
		thread_pool& operator=(thread_pool&&) = delete;

		// Destroys a thread pool, shutting down the thread pool first
		// (if not already shutdown).
		~thread_pool(){
			shutdown();
			for(auto& i : workers){
				if(i.joinable()){
					i.join();
				}
			}
		}

		// Gets the number of threads in the thread pool.
		// This function is not thread safe.
		size_type size() const{
			return num_threads_;
		}

		// Enqueues a task for execution by the thread pool.
		// This function inserts the task specified by the callable
		// entity func into the queue of tasks associated with the
		// thread pool.
		// This function may block if the number of currently
		// queued tasks is sufficiently large.
		// Note: The rvalue reference parameter is intentional and
		// implies that the schedule function is permitted to change
		// the value of func (e.g., by moving from func).
		// Precondition: The thread pool is not in the shutdown state
		// and is not currently in the process of being shutdown via
		// the shutdown member function.
		// This function is thread safe.
		void schedule(std::function<void()>&& funcc){
			std::unique_lock<std::mutex> lock(m_);
			c_add_.wait(lock, [this](){ return ((!queue_.is_full()) || (shutDown_)); });
			if(!shutDown_){
				queue_.push(std::move(funcc));
			}
		}

		// Shuts down the thread pool.
		// This function places the thread pool into a state where
		// new tasks will no longer be accepted via the schedule
		// member function.
		// Then, the function blocks until all queued tasks
		// have been executed and all threads in the thread pool
		// are idle (i.e., not currently executing a task).
		// Finally, the thread pool is placed in the shutdown state.
		// If the thread pool is already shutdown at the time that this
		// function is called, this function has no effect.
		// After the thread pool is shutdown, it can only be destroyed.
		// This function is thread safe.
		void shutdown(){
			std::unique_lock<std::mutex> lock(m_);
			if(!shutDown_){
				shutDown_ = true;
				queue_.close();
				c_task_.notify_all();
				c_done_.wait(lock, [this](){ return (queue_.is_empty()); });
			}
		}

		// Tests if the thread pool has been shutdown.
		// This function is not thread safe.
		bool is_shutdown() const{
			return shutDownFinished_;
		}

		private:
		std::vector<std::thread> workers;
		mutable std::mutex m_;
		mutable std::condition_variable c_task_;
		mutable std::condition_variable c_done_;
		mutable std::condition_variable c_add_;

		my_queue queue_;
		bool allThreadsMade_;
		bool shutDown_;
		bool shutDownFinished_;
		size_type num_threads_;

	};
}
#endif

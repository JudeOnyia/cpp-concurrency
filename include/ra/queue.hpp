#ifndef QUEUEHPP
#define QUEUEHPP
#include <cstddef>
#include <queue>
#include <utility>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace ra::concurrency {

	// Concurrent bounded FIFO queue class.
	template <class T>
	class queue
	{
		public:

			// The type of each of the elements stored in the queue.
			using value_type = T;

			// An unsigned integral type used to represent sizes.
			using size_type = std::size_t;

			// A type for the status of a queue operation.
			enum class status {
				success = 0, // operation successful
				empty, // queue is empty (not currently used)
				full, // queue is full (not currently used)
				closed, // queue is closed
			};

			// A queue is not default constructible.
			queue() = delete;

			// Constructs a queue with a maximum size of max_size.
			// The queue is marked as open (i.e., not closed).
			// Precondition: The quantity max_size must be greater than
			// zero.
			queue(size_type max_size) : capacity_(max_size), stat_(status::empty) {}

			// A queue is not movable or copyable.
			queue(const queue&) = delete;
			queue& operator=(const queue&) = delete;
			queue(queue&&) = delete;
			queue& operator=(queue&&) = delete;

			// Destroys the queue after closing the queue (if not already
			// closed) and clearing the queue (if not already empty).
			~queue(){
				close();
				clear();
			}

			// Inserts the value x at the end of the queue, blocking if
			// necessary.
			// If the queue is full, the thread will be blocked until the
			// queue insertion can be completed or the queue is closed.
			// If the value x is successfully inserted on the queue, the
			// function returns status::success.
			// If the value x cannot be inserted on the queue (due to the
			// queue being closed), the function returns with a return
			// value of status::closed.
			// This function is thread safe.
			// Note: The rvalue reference parameter is intentional and
			// implies that the push function is permitted to change
			// the value of x (e.g., by moving from x).
			status push(value_type&& x){
				std::unique_lock<std::mutex> lock(m_);
				c_full_.wait(lock, [this](){ return (stat_!= status::full);});
				if(stat_==status::closed){
					return stat_;
				}
				else {
					queue_.push(std::move(x));
					if(queue_.size()==capacity_){
						stat_ = status::full;
					}
					if(stat_==status::empty){
						stat_ = status::success;
					}
					c_empty_.notify_one();
					return (status::success);
				}
			}

			// Removes the value from the front of the queue and places it
			// in x, blocking if necessary.
			// If the queue is empty and not closed, the thread is blocked
			// until: 1) a value can be removed from the queue; or 2) the
			// queue is closed.
			// If the queue is closed, the function does not block and either
			// returns status::closed or status::success, depending on whether
			// a value can be successfully removed from the queue.
			// If a value is successfully removed from the queue, the value
			// is placed in x and the function returns status::success.
			// If a value cannot be successfully removed from the queue (due to
			// the queue being both empty and closed), the function returns
			// status::closed.
			// This function is thread safe.
			status pop(value_type& x){
				std::unique_lock<std::mutex> lock(m_);
				c_empty_.wait(lock, [this](){ return (stat_!= status::empty);});
				if((stat_==status::closed) && (queue_.empty())){
					return stat_;
				}
				else{
					x = queue_.front();
					queue_.pop();
					if(stat_==status::full){
						stat_ = status::success;
					}
					if((stat_!=status::closed) && (queue_.empty())){
						stat_ = status::empty;
					}
					c_full_.notify_one();
					return (status::success);
				}
			}

			// Closes the queue.
			// The queue is placed in the closed state.
			// The closed state prevents more items from being inserted
			// on the queue, but it does not clear the items that are
			// already on the queue.
			// Invoking this function on a closed queue has no effect.
			// This function is thread safe.
			void close(){
				std::scoped_lock<std::mutex> lock(m_);
				if(stat_ != status::closed){
					stat_ = status::closed;
				}
				c_full_.notify_all();
				c_empty_.notify_all();
			}

			// Clears the queue.
			// All of the elements on the queue are discarded.
			// This function is thread safe.
			void clear(){
				std::scoped_lock<std::mutex> lock(m_);
				if(stat_ != status::empty || (!queue_.empty())){
					while(!(queue_.empty())){
						queue_.pop();
					}
					if(stat_ != status::closed){
						stat_ = status::empty;
						c_full_.notify_all();
					}
				}
			}

			// Returns if the queue is currently full (i.e., the number of
			// elements in the queue equals the maximum queue size).
			// This function is not thread safe.
			bool is_full() const{
				return (queue_.size()==capacity_);
			}

			// Returns if the queue is currently empty.
			// This function is not thread safe.
			bool is_empty() const{
				return (queue_.empty());
			}

			// Returns if the queue is closed (i.e., in the closed state).
			// This function is not thread safe.
			bool is_closed() const{
				return (stat_==status::closed);
			}

			// Returns the maximum number of elements that can be held in
			// the queue.
			// This function is not thread safe.
			size_type max_size() const{
				return capacity_;
			}

		private:
			std::queue<value_type> queue_;
			mutable std::mutex m_;
			mutable std::condition_variable c_empty_;
			mutable std::condition_variable c_full_;

			size_type capacity_;
			status stat_;

	};
}
#endif

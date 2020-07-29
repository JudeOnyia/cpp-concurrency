#include "ra/thread_pool.hpp"

namespace ra::concurrency {
	// Function performed by each thread
	void worker(thread_pool* obj){
		std::unique_lock<std::mutex> lock(obj->m_);
		while(!(obj->shutDown_ && obj->queue_.is_empty())){
			obj->c_task_.wait(lock, [obj](){ return ((obj->allThreadsMade_ && (!obj->queue_.is_empty())) || (obj->shutDown_));});
			if(!obj->queue_.is_empty()){
				std::function<void()> task_;
				obj->queue_.pop(task_);
				lock.unlock();
				obj->c_add_.notify_one();
				task_();
				lock.lock();
			}
		}
		obj->shutDownFinished_ = true;
		obj->c_done_.notify_all();
	}
}

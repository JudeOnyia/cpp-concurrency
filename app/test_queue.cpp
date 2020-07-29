#include "ra/queue.hpp"
#include <cstddef>
#include <thread>
#include <mutex>
#include <iostream>
#include <utility>

using value_type = double;
using queue = ra::concurrency::queue<value_type>;
using status = ra::concurrency::queue<value_type>::status;
using queueForStat = ra::concurrency::queue<status>;
using std::cout;
using std::endl;

constexpr std::size_t num_elements(10);
queue q(2);
queue result(10);
queueForStat stat_store(30);

int main(){
	std::thread t1([](){
		for(std::size_t i=0; i<num_elements; ++i){
			status stat = q.push(value_type(i));
			stat_store.push(std::move(stat));
		}
		q.close();
	});
	std::thread t2([](){
		bool keep_running(true);
		for(std::size_t i=0; i<num_elements; ++i){
			value_type x;
			status stat = q.pop(x);
			stat_store.push(std::move(stat));
			x *= value_type(2);
			stat = result.push(std::move(x));
			stat_store.push(std::move(stat));
		}
	});
	t1.join(); t2.join();
	cout<<"Is q closed (should be true): "<<(q.is_closed())<<endl;
	value_type res(0);

	while(!result.is_empty()){
		result.pop(res);
		cout<<res<<" ";
	}
	cout<<endl;
	cout<<"Is stat_store full (should be true): "<<(stat_store.is_full())<<endl;
	cout<<"Max size of stat_store: "<<(stat_store.max_size())<<endl;

	status stat(status::success);
	while(!stat_store.is_empty()){
		stat_store.pop(stat);
		if(stat==status::success){ cout<<"success"<<endl; }
		else if(stat==status::closed){ cout<<"closed"<<endl; }
		else { cout<<"Problem here"<<endl; }
	}
}

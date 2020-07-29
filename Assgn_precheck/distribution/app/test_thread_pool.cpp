#include "ra/thread_pool.hpp"
#include <iostream>
#include <cstddef>

using thr_pool = ra::concurrency::thread_pool;
using std::cout;
using std::endl;

int main(){
	thr_pool tp(8);
	for(std::size_t i=0; i<30; ++i){
		tp.schedule([](){cout<<"hello world"<<endl;});
	}
	tp.shutdown();
	//cout<<"Is shutdown (must be false): "<<(tp.is_shutdown())<<endl;
	//cout<<"Num of threads "<<(tp.size())<<endl;
	//while(!tp.is_shutdown()){}
	//cout<<"Is shutdown (must be true): "<<(tp.is_shutdown())<<endl;

}

#ifndef JULIASETHPP
#define JULIASETHPP
#include "ra/thread_pool.hpp"
#include <complex>
#include <boost/multi_array.hpp>
#include <cstddef>
#include <thread>
#include <mutex>
#include <functional>


namespace ra::fractal {
	using size_type = int;

	boost::multi_array<int, 2> a_; // shared pointer to matrix a
	std::mutex m_a; // Mutex used to make access to a_ptr mutually exclusive

	template <class Real>
	std::complex<Real> getZ(size_type l, size_type k, size_type W, size_type H, std::complex<Real> U, std::complex<Real> V){
		Real U0 = U.real();
		Real U1 = U.imag();
		Real V0 = V.real();
		Real V1 = V.imag();
		Real Z0 = U0 + ( (Real(k)/Real(W-1)) * (V0-U0) );
		Real Z1 = U1 + ( (Real(l)/Real(H-1)) * (V1-U1) );
		std::complex<Real> Z(Z0,Z1);
		return Z;
	}

	template <class Real>
	int julia_ym(int m, std::complex<Real> z, std::complex<Real> c){
		int i(0);
		while(!(  (i>=m) || (std::abs(z)>Real(2))   )){
			z = (z * z) + c;
			++i;
		}
		if(i>=m) { return m; }
		else{ return i; }
	}


	template <class Real>
	void compute_julia_set(const std::complex<Real>& bottom_left, const std::complex<Real>& top_right, const std::complex<Real>& c, int max_iters, boost::multi_array<int, 2>& a, int num_threads){
		size_type W = a.shape()[1]; // num of cols
		size_type H = a.shape()[0]; // num of rows
		a_.resize(boost::extents[a.shape()[0]][a.shape()[1]]);
		a_ = a;
		//a_ = a;
		ra::concurrency::thread_pool tp(num_threads); // Create Thread pool
		for(size_type l=0; l<H; ++l){
			//judeboost::multi_array<int, 2>* a_row_ptr = &(a[l][0]);
			tp.schedule([l, W, H, bottom_left, top_right, c, max_iters](){
				std::unique_lock lock(m_a, std::defer_lock);
				for(size_type k=0; k<W; ++k){
					std::complex<Real> z = getZ(l,k,W,H,bottom_left, top_right);
					int a_ym = julia_ym(max_iters,z,c);
					lock.lock();
					a_[l][k] = a_ym;
					lock.unlock();
				}
			});
		}
		tp.shutdown();
		std::unique_lock lock(m_a);
		a.resize(boost::extents[a_.shape()[0]][a_.shape()[1]]);
		a = a_;
	}






}
#endif

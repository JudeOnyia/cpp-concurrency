#include <type_traits>
#include <boost/multi_array.hpp>
#include <ra/julia_set.hpp>

template <class Real>
void test()
{
	namespace rf = ra::fractal;
	std::complex<Real> bottom_left;
	std::complex<Real> top_right;
	std::complex<Real> c;
	int max_iters;
	boost::multi_array<int, 2> a;
	int num_threads;
	rf::compute_julia_set(bottom_left, top_right, c, max_iters, a, num_threads);
}

int main()
{
	test<float>();
	test<double>();
	test<long double>();
}

#include "ra/julia_set.hpp"
#include <complex>
#include <boost/multi_array.hpp>
#include <cstddef>
#include <iostream>
#include <fstream>

int main(){
	using Real = double;
	using std::cout;
	using std::endl;
	std::complex<Real> bottom_left(-1.25,-1.25);
	std::complex<Real> top_right(1.25,1.25);
	std::complex<Real> c(0.37,-0.16);
	int max_iters(255);
	int num_threads(8);
	int num_rows(512);
	int num_cols(512);
	boost::multi_array<int, 2> a(boost::extents[num_rows][num_cols]);
	ra::fractal::compute_julia_set(bottom_left,top_right,c,max_iters,a,num_threads);

	std::ofstream outFile("image.pnm");
	outFile<<"P2 "<<num_cols<<" "<<num_rows<<" "<<max_iters<<"\n";
	cout<<"Did signature"<<endl;
	for(int row(num_rows-1); row>=0; --row){
		cout<<"row: "<<row<<endl;
		for(int col(0); col<num_cols; ++col){
			if(col){ outFile<<" "; }
			outFile<<(a[row][col]);
		}
		outFile<<"\n";
	}
	outFile.close();
	

}

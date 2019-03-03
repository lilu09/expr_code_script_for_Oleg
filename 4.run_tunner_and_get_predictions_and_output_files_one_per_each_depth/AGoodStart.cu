

#include <bits/stdc++.h>
#include <lib/matrix_mul_tunable.h>
#include <lib/tuneit.h>
#include <lib/tuplex.h>

using namespace std;

int main(int argc, char* argv[])
{

	assert(argc==4);

	size_t const num_random_points=atoi(argv[1]),
	       num_trys_per_point=atoi(argv[2]),
	       num_depth=atoi(argv[3]);

	assert( cudaDeviceReset() == cudaSuccess ) ;

	using namespace std;



	ifstream file("../1.initialization_db_and_output_points/random_points_production_run.csv");
	//id, x1, x2, x3
	vector<tuple<size_t, size_t, size_t, size_t> > random_points;
	random_points.reserve(num_random_points);

	vector<size_t> predictions;
	predictions.reserve(num_random_points);

	//depth, sampled_points_count
	vector<tuple<size_t, size_t> > sampled_points_count;
	sampled_points_count.reserve(num_depth-1);

	file_copy(file, random_points);


	vector<bool> mask(4,true);
	constexpr size_t num_variant=MATRIX_MUL_NUM_VARIANTS;
	constexpr size_t num_vertices=8; //manually calculate pow(2,Dim), ugly!  nvcc can not integrate with gcc on c++14!

	//test from 1 to 11
	for(size_t depth=1; depth<=num_depth;++depth){

		cout<<"Now start depth "<<depth<<"..."<<endl;

		tuneit::tuneit_settings<MATRIX_MUL_NUM_DIM, MATRIX_MUL_NUM_VARIANTS> st{depth, mask, true, false, true, 40, {{1,100}, {1,100}, {1,100}} };

		tuneit::tuneit< num_variant, num_vertices, matrix_mul_tunable<float, size_t, size_t, size_t>,
			float, size_t, size_t, size_t> mytuner(st);

		mytuner.train();

		sampled_points_count.emplace_back( std::make_tuple<size_t,
				size_t>(static_cast<size_t>(depth),
					static_cast<size_t>(mytuner.sampled_points_count()) ) );
		predictions.clear();

		for(auto const &i:random_points){
			predictions.emplace_back( mytuner.predict(get<1>(i), get<2>(i), get<3>(i)) );
		}

		//build a new tuple for output and write to file
		//id, x1, x2, x3, predicted winner
		vector<tuple<size_t, size_t, size_t, size_t, size_t> > final_form;
		final_form.reserve(random_points.size());
		for(size_t i=0; i<random_points.size();++i){
			final_form.emplace_back( std::make_tuple<size_t, size_t, size_t, size_t, size_t> (
						static_cast<size_t>(get<0>(random_points[i])),
						static_cast<size_t>(get<1>(random_points[i])),
						static_cast<size_t>(get<2>(random_points[i])),
						static_cast<size_t>(get<3>(random_points[i])),
						static_cast<size_t>(predictions[i])
						) );
		}

		ofstream file2( ("predictions_depth_"+to_string(depth)+".csv").c_str() );
		tuple_ofstream_iterator start2(file2, "");
		copy(final_form.begin(), final_form.end(), start2  );

		ofstream file3("depth_and_sampled_points_count.csv");
		tuple_ofstream_iterator start3(file3, "");
		copy(sampled_points_count.begin(), sampled_points_count.end(), start3);

	}

	//write <depth, sampled_points_count> to table
	/*vector<tuple<size_t, size_t> > final_form;*/

	/*for(size_t depth=1; depth<11;++depth){*/
	/*final_form.emplace_back(*/
	/*std::make_tuple<size_t, size_t>(depth,*/
	/*sampled_points_count[depth-1])*/
	/*);*/
	/*}*/



	return EXIT_SUCCESS;
}

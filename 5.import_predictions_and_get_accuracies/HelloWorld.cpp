

#include <bits/stdc++.h>
#include <unistd.h> //sleep()

#include <lib/socix.h>
#include <lib/tuplex.h>

using namespace std;


int main(int argc, char* argv[])
{

	assert(argc==3);

	size_t const num_depth=stoi(argv[1]),
	       num_random_points=stoi(argv[2]);


	//export select part of ground truth to file

	try{

		using namespace soci;

		session sql(postgresql, "dbname=tune_research");

		sql<<"select id,x1,x2,x3,winner into temp from ground_truth order by id";
		sql<<"COPY temp TO '/tmp/temp.csv' DELIMITER ' ' CSV;";
		sql<<"drop table temp";


	} catch (soci::postgresql_soci_error const & e){

		cerr << "PostgreSQL error: " << e.sqlstate()
			<< " " << e.what() << endl;


	} catch (exception const & e){

		cerr << "Some other error: " << e.what() << endl;

	}

	system("cp -rf /tmp/temp.csv ground_truth.csv");


	//restore to tuple
	//format: id,x1,x2,x3,winner
	vector<tuple<size_t, size_t, size_t, size_t, size_t> > ground_truth;
	ground_truth.reserve(num_random_points);
	ifstream file("ground_truth.csv");
	file_copy(file, ground_truth);

	//depth, accuracy
	vector< tuple<size_t, double> > accuracy;
	accuracy.reserve(num_depth);


	//import all prediction csv
	//and compute its accuracy against ground truth
	for(size_t i=1; i<=num_depth; ++i){


		//restore prediction to tuple
		//format: id,x1,x2,x3,winner
		vector<tuple<size_t, size_t, size_t, size_t, size_t> > predictions;
		predictions.reserve(num_random_points);
		ifstream file2("../4.run_tunner_and_get_predictions_and_output_files_one_per_each_depth/predictions_depth_"+to_string(i)+".csv");
		file_copy(file2, predictions);

		size_t matches=0;

		//compute matches
		for(size_t j=0;j<num_random_points;++j){
			assert( get<1>(predictions[j]) == get<1>(ground_truth[j]) );
			assert( get<2>(predictions[j]) == get<2>(ground_truth[j]) );
			assert( get<3>(predictions[j]) == get<3>(ground_truth[j]) );

			matches+= (get<4>(predictions[j]) == get<4>(ground_truth[j]) );

		}

		accuracy.emplace_back( make_tuple<size_t, double>( static_cast<size_t>(i),  static_cast<double>(double(matches) /double(num_random_points)) ) );
	}


	//restore depth #points csv to tuple
	vector<tuple<size_t, size_t> > depth_points;
	depth_points.reserve(num_depth);
	ifstream file3("../4.run_tunner_and_get_predictions_and_output_files_one_per_each_depth/depth_and_sampled_points_count.csv");
	file_copy(file3, depth_points);


	//combine two kinds of table into #points accuracy
	//write it to both files and database
	vector<tuple<size_t, double> > points_accuracy;
	points_accuracy.reserve(num_depth);
	for(size_t i=1; i<=num_depth; ++i){
		assert(get<0>(depth_points[i-1]) == get<0>(accuracy[i-1]) );
		points_accuracy.emplace_back( make_tuple<size_t, double>( static_cast<size_t>(get<1>(depth_points[i-1])), static_cast<double>(get<1>(accuracy[i-1]) ) ) );
	}

	ofstream file4("final_points_accuracy.csv");
	tuple_ofstream_iterator start(file4, "");
	copy(points_accuracy.begin(), points_accuracy.end(), start);


	//visualize it using R


	//Done!


	return 0;
}

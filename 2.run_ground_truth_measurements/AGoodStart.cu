
#include <iostream>
#include <assert.h>
#include <lib/matrix_mul_tunable.h>
#include <lib/tuplex.h>

using namespace std;

int main(int argc, char* argv[])
{

	assert( cudaDeviceReset() == cudaSuccess ) ;

	size_t const num_random_points=atoi(argv[1]),
	       num_trys_per_point=atoi(argv[2]),
	       total_measurements=num_trys_per_point*MATRIX_MUL_NUM_VARIANTS*num_random_points;

	//rpid, x1, x2, x3
	vector<tuple<size_t, size_t, size_t, size_t> > random_points;
	random_points.reserve(num_random_points);

	//restore random_points from file
	ifstream file("../1.initialization_db_and_output_points/random_points_production_run.csv");
	file_copy(file, random_points);


	/*std::copy(random_points.begin(), random_points.end(),*/
			/*tuple_ostream_iterator(std::cout, ""));*/
	/*cout<<(char)8<<(char)8<<' '<<endl;*/



	//exprid, seq_id, rpid, vid, value
	vector< tuple<size_t, size_t, size_t, size_t, meterpu::CPU_Time::ResultType> >
		measurements(total_measurements);

	size_t exprid=0;


	using namespace meterpu;
	meter<CPU_Time> cpu_meter;
	meter<CUDA_Time> cuda_meter;


	ofstream outfile("all_measurements.csv");
	tuple_ofstream_iterator start(outfile, "");

	auto i=measurements.begin();
	decltype(i) begin, end;

	// Loop all points
	for(auto p= random_points.begin(); p < random_points.end(); ++p){

		//Can use std::tie here, but I just want references
		const size_t& HA=get<1>(*p), &WA=get<2>(*p), &WB=get<3>(*p);

		vectorpu::vector<float> A(WA*HA,1), B(WA*WB,1), C(HA*WB,0), C_ref(HA*WB,WA);



		cout<<"computing "<<HA<<" "<<WA<<" "<<WB<<"..."<<endl;

		begin=i;



		// For each variant
		for(size_t v=0; v<MATRIX_MUL_NUM_VARIANTS;
				++v){

			// For each repeated try on the same point
			for(size_t r=0; r< num_trys_per_point; ++r){


				if(v<MATRIX_MUL_NUM_VARIANTS-1){
					cpu_meter.start();
					(*matrix_mul_dispatch_table[v])(R(A), R(B), W(C), HA, WA, WB);
					cpu_meter.stop();
					cpu_meter.calc();
					*(i++)
						=make_tuple(exprid++,r,p-random_points.begin(),v,cpu_meter.get_value() );
				}
				else{
					cuda_meter.start();
					(*matrix_mul_dispatch_table[v])(GR(A), GR(B), GW(C), HA, WA, WB);
					cuda_meter.stop();
					cuda_meter.calc();
					*(i++)
						=make_tuple(exprid++,r,p-random_points.begin(),v,cuda_meter.get_value() );
				}



				assert( equal(RI(C),REI(C), RI(C_ref)) );

			}
		}

		end=i;



	}


	copy(measurements.begin(), measurements.end(), start);





	return EXIT_SUCCESS;
}

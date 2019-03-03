

#include <bits/stdc++.h>
#include <lib/socix.h>

using namespace std;

#define MATRIX_MUL_NUM_VARIANTS 4


int main(int argc, char* argv[])
{

	try
	{
		size_t const num_random_points=atoi(argv[1]),
		       num_trys_per_point=atoi(argv[2]),
		       total_measurements=num_trys_per_point*MATRIX_MUL_NUM_VARIANTS*num_random_points;

		using namespace soci;

		session sql(postgresql, "dbname=tune_research");

		sql<<"delete from ground_truth";
		sql<<"delete from experiment_data_production_run";

		//create and init variant table
		/*{{{*/
		{

			string table_name="variants_production_run";
			vector<string> col_names{"vid",  "name"};
			vector<string> col_type{"int unique primary key", "varchar(50)"};

			database_output_iterator db_iter(sql, table_name, col_names, col_type);
			//db_iter.destroy();
			//db_iter.create();
			db_iter.clear();

			vector<tuple<size_t, string> > x{
				make_tuple(0,"matrix_mul_cpu"),
					make_tuple(1,"matrix_mul_blas"),
					make_tuple(2,"matrix_mul_openmp"),
					make_tuple(3,"matrix_mul_cublas")};
			batch_copy(x.cbegin(),x.cend(), db_iter);
		}
		/*}}}*/

		//create and init random points table
		/*{{{*/
		{


			string table_name="random_points_production_run";
			vector<string> col_names{"rpid",  "ha", "wa", "wb"};
			vector<string> col_type{"int unique primary key", "int", "int", "int"};

			database_output_iterator db_iter(sql, table_name, col_names, col_type);
			//db_iter.destroy();
			//db_iter.create();
			db_iter.clear();

			vector<tuple<size_t, size_t, size_t, size_t> > random_points(num_random_points);
			size_t rnd_id=0;
			srand (time(NULL));
			generate(random_points.begin(), random_points.end(),
					[&rnd_id]{return make_tuple(
							rnd_id++,
							//rand()%499+1,
							//rand()%499+1,
							//rand()%499+1);}
							rand()%99+1,
							rand()%99+1,
							rand()%99+1);}
				);
			batch_copy(random_points.cbegin(),random_points.cend(), db_iter);

			//output points
			db_iter.copy_to_pwd();
		}
		/*}}}*/



	} catch (soci::postgresql_soci_error const & e){

		cerr << "PostgreSQL error: " << e.sqlstate()
			<< " " << e.what() << endl;


	} catch (exception const & e){

		cerr << "Some other error: " << e.what() << endl;

	}



	return 0;
}

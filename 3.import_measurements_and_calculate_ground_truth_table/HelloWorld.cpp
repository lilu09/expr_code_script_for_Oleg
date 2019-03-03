

#include <bits/stdc++.h>
#include <unistd.h> //sleep()
#include <lib/socix.h>
#include <lib/bash_wrapper.h>

using namespace std;


void calculate_ground_truth_table(soci::session &sql)
{

	using namespace soci;
	sql<<"delete from ground_truth";

	size_t num_trys_per_point, num_random_points;
	sql<<"select count(*) from experiment_data_production_run where rpid=1 and vid=1", into(num_trys_per_point);
	sql<<"select count(distinct rpid) from experiment_data_production_run", into(num_random_points);

	//size_t const num_of_random_points=2;
	size_t seq_id;
	size_t const seq_id_min=1;
	size_t const seq_id_max=num_trys_per_point-1;
	assert(seq_id_max>=seq_id_min);
	size_t total_seq_ids;
	size_t win_count[4];
	double proportions[4];
	double mean[4];
	size_t winner;
	size_t x1,x2,x3;
	size_t vid_temp;
	double value_temp;
	double winner_proportion_temp;
	//size_t id=0;

	for(size_t rpid=0; rpid<num_random_points; ++rpid){
		//put all data into measurements

		//reset win_count for each point,
		//but reuse within each point
		fill(win_count, win_count+4,0);
		fill(mean, mean+4,0);

		//get x1,x2,x3
		sql<<"select ha, wa, wb from random_points_production_run where rpid=:rpid", use(rpid), into(x1), into(x2), into(x3);


		//for each seq_id
		for (size_t seq_id = seq_id_min; seq_id <= seq_id_max; ++seq_id) {

			//compute winner
			sql<<"select vid,value into temp from experiment_data_production_run where rpid=:rpid and seq_id =:seq_id;", use(rpid), use(seq_id);
			sql<<"select vid from temp where value = (select min(value) from temp);", into(winner);

			//compute total count
			//cout<<"winner is: "<<winner<<endl;
			win_count[winner]++;


			//compute mean sum
			for(int i=0;i<4;++i){
				sql<<"select value from temp where vid=:i", use(i), into(value_temp);
				mean[i] += value_temp;
			}

			sql<<"drop table temp";

		}

		//compute proportion and mean
		total_seq_ids=seq_id_max - seq_id_min+1;
		assert(total_seq_ids>0);
		for(size_t i=0;i<4;++i){
			proportions[i]= static_cast<double>(win_count[i])/static_cast<double>(total_seq_ids);
			mean[i]= mean[i]/static_cast<double>(total_seq_ids);
		}

		//compute winner and winner_proportion
		winner=0;
		winner_proportion_temp=proportions[0];

		for (size_t i = 1; i < 4; ++i) {
			if(proportions[i]>winner_proportion_temp){
				winner=i;
				winner_proportion_temp=proportions[i];
			}
		}



		//insert database
		//id here is the same as rpid
		sql<<"insert into ground_truth (id, x1 , x2 , x3 , winner , winner_proportion , winner_mean , v1_mean , v2_mean , v3_mean , v4_mean , v1_proportion , v2_proportion , v3_proportion , v4_proportion) values (:id,  :x1 , :x2 , :x3 , :winner , :winner_proportion , :winner_mean , :v1_mean , :v2_mean , :v3_mean , :v4_mean , :v1_proportion , :v2_proportion , :v3_proportion , :v4_proportion)", use(rpid), use(x1), use(x2), use(x3), use(winner), use(winner_proportion_temp), use(mean[winner]), use(mean[0]), use(mean[1]), use(mean[2]), use(mean[3]), use(proportions[0]), use(proportions[1]), use(proportions[2]), use(proportions[3]);

	}
}

int main(int argc, char* argv[])
{

	size_t const num_random_points=atoi(argv[1]),
	       num_trys_per_point=atoi(argv[2]);

	string filename="../2.run_ground_truth_measurements/all_measurements.csv";
	string abs_path;
	bash::realpath(filename, abs_path);
	//vector<string> _realpath;
	//auto status=cmd("realpath "+filename, _realpath);
	//assert(status==true);
	//assert(_realpath.size()>0);
	//realpath=_realpath[0];

	try
	{

		using namespace soci;

		session sql(postgresql, "dbname=tune_research");

		//create measurement table
		//import data
		/*{{{*/
		{
			string table_name="experiment_data_production_run";
			vector<string> col_names{"exprid",  "seq_id", "rpid", "vid", "value"};
			vector<string> col_type{"int unique primary key", "int", "int references random_points_production_run(rpid)", "int references variants_production_run(vid)", "double precision"};

			database_output_iterator db_iter(sql, table_name, col_names, col_type);
			//db_iter.destroy();
			//db_iter.create();
			db_iter.clear();

			db_iter.copy_from(abs_path);
		}
		/*}}}*/

		//create ground truth table
		//compute it
		{
			string table_name="ground_truth";
			vector<string> col_names{"id", "x1" , "x2" , "x3" , "winner" , "winner_proportion" , "winner_mean" , "v1_mean" , "v2_mean" , "v3_mean" , "v4_mean" , "v1_proportion" , "v2_proportion" , "v3_proportion" , "v4_proportion"};
			vector<string> col_type{"int unique primary key", "int", "int", "int", "int references variants_production_run(vid)", "double precision", "double precision", "double precision", "double precision", "double precision", "double precision", "double precision", "double precision", "double precision", "double precision"};

			database_output_iterator db_iter(sql, table_name, col_names, col_type);
			//db_iter.create();
			db_iter.clear();

			calculate_ground_truth_table(sql);
		}



	} catch (soci::postgresql_soci_error const & e){

		cerr << "PostgreSQL error: " << e.sqlstate()
			<< " " << e.what() << endl;


	} catch (exception const & e){

		cerr << "Some other error: " << e.what() << endl;

	}

	return 0;
}

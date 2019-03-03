#!/bin/bash

# set -e
# set -o pipefail


# Activate debug
# set -x

#two measurement steps can be run on excess
#others can be done locally on my laptop



run()
{

	#step 1
	echo "perform step 1..."
	cd ./1.initialization_db_and_output_points/
	rm *.csv
	make && make run && make allclean
	cd ..

	#step 2
	echo "perform step 2..."
	cd ./2.run_ground_truth_measurements/
	rm *.csv
	make && make run && make allclean
	cd ..

	#step 3
	echo "perform step 3..."
	cd ./3.import_measurements_and_calculate_ground_truth_table/
	make && make run && make allclean
	cd ..
	
	#step 4
	echo "perform step 4..."
	cd ./4.run_tunner_and_get_predictions_and_output_files_one_per_each_depth/
	rm *.csv
	make && make run && make allclean
	cd ..

	#step 5
	echo "perform step 5..."
	cd ./5.import_predictions_and_get_accuracies/
	rm *.csv
	make && make run && make allclean
	cat final_points_accuracy.csv
	cd ..
}

run

#include "stdafx.h"

#include "FannTest.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "floatfann.h"
#include "fann.h"

#include "dataUtility.h"
#include "constants.h"
#include "DataHandlers.h"
#include "Utility.h"


void emgNNfileRemover(){
	remove("NNdata/emg_float.net");
}

template <class Array>
void generateEmgSumArray(Array *emg_values, int array_size, FileDataHandler gesture_training_data){
	double emg_max = -1;
	double emg_min = -1;

	for (int i = 0; i < array_size; i++)
	{
		double emg_sum = 0.0;
		for (int k = 0; k < DATA_LENGTH_EMG; k++)
		{
			emg_sum += pow(gesture_training_data.getSensorData(EMG)[i][k], 2);
		}
		emg_values[i] = emg_sum;
		if (emg_sum > emg_max)
			emg_max = emg_sum;

		if (emg_sum < emg_min || emg_min == -1)
			emg_min = emg_sum;
	}

	for (int i = 0; i < array_size; i++)
	{
		emg_values[i] = (emg_values[i] - emg_min) / (emg_max - emg_min);
	}
}

void buildEmgNeuralNetworkTrainingFile(){
	std::ofstream training_data;
	training_data.open(EMG_NN_TRAINING_FILENAME);

	int number_of_training_instances = training_file_list.size;
	
	training_data << number_of_training_instances << " ";
	training_data << EMG_NN_NUMBER_OF_INPUT << " ";
	training_data << EMG_NN_NUMBER_OF_OUTPUT << std::endl;

	for (int training_instance_i = 0; training_instance_i < number_of_training_instances; training_instance_i++)
	{

		File training_data_file = training_file_list.files[training_instance_i];
		std::cout << training_data_file.filename << std::endl;

		FileDataHandler gesture_training_data(training_data_file, TRAINING);

		int solution = training_data_file.gesture;

		double *emg_values = new double[EMG_NN_NUMBER_OF_INPUT];
		generateEmgSumArray(emg_values, EMG_NN_NUMBER_OF_INPUT, gesture_training_data);

		for (int i = 0; i < EMG_NN_NUMBER_OF_INPUT; i++)
		{
			training_data << std::setprecision(9) << emg_values[i];
			if (i != EMG_NN_NUMBER_OF_INPUT - 1)
				training_data << " ";
		}
		training_data << std::endl;

		for (int o = 0; o < EMG_NN_NUMBER_OF_OUTPUT; o++)
		{
			if (solution == o) {
				training_data << 1;
				std::cout << 1;
			}
			else {
				training_data << 0;
				std::cout << 0;
			}
			std::cout << " ";

			if (o != EMG_NN_NUMBER_OF_INPUT - 1) {
				training_data << " ";
			}
		}

		if (training_instance_i != number_of_training_instances - 1) {
			training_data << std::endl;
			std::cout << std::endl;
		}
			
	}
	std::cout << std::endl;

	training_data.close();
}

void emgTrainNN(){
	const float desired_error = (const float) 0.000001;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 1000;

	struct fann *ann = fann_create_standard(EMG_NN_NUMBER_OF_LAYERS, EMG_NN_NUMBER_OF_INPUT, EMG_NN_NUMBER_OF_NEURONS_HIDDEN[0], EMG_NN_NUMBER_OF_OUTPUT);

	//float 	connection_rate = EMG_NN_NUMBER_OF_INPUT/3;
	//struct fann *ann = fann_create_sparse(connection_rate, EMG_NN_NUMBER_OF_LAYERS, EMG_NN_NUMBER_OF_INPUT, EMG_NN_NUMBER_OF_NEURONS_HIDDEN[0], EMG_NN_NUMBER_OF_OUTPUT);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID);
	fann_set_activation_function_output(ann, FANN_SIGMOID);

	fann_train_on_file(ann, EMG_NN_TRAINING_FILENAME.c_str(), max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, EMG_NN_FILENAME.c_str());

	fann_destroy(ann);
}

void emgTestNN(File file){
	File test_data_file = file;
	Gesture gesture = file.gesture;

	FileDataHandler gesture_training_data(test_data_file, TEST);

	std::cout << "###############################" << std::endl;
	std::cout << "Input file: " << test_data_file.filename << "\n\n";

	fann_type *calc_out;
	fann_type *input = new fann_type[EMG_NN_NUMBER_OF_INPUT];

	struct fann *ann = fann_create_from_file(EMG_NN_FILENAME.c_str());
	
	generateEmgSumArray(input, EMG_NN_NUMBER_OF_INPUT, gesture_training_data);

	calc_out = fann_run(ann, input);

	double max_calc_out = (double)calc_out[0];
	Gesture guess_gesture = (Gesture)0;
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		std::cout << gestureToString((Gesture)i) << ": " << calc_out[i] << std::endl;
		if ((double)calc_out[i] > max_calc_out)
		{
			guess_gesture = (Gesture)i;
			max_calc_out = calc_out[i];
		}
	}
	std::cout << "\nGesture: " << gestureToString(gesture) << std::endl;
	std::cout << "Prediction: " << gestureToString(guess_gesture) << std::endl;

	std::cout << std::endl;

	fann_destroy(ann);
}

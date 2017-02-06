#include "stdafx.h"

#include "FannTest.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>

#include "floatfann.h"
#include "fann.h"

#include "dataUtility.h"
#include "constants.h"
#include "DataHandlers.h"
#include "Utility.h"


void trainNN(){
	const unsigned int num_input = 2;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 3;
	const unsigned int num_neurons_hidden = 3;
	const float desired_error = (const float) 0.000001;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 1000;

	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, "NNdata/xor.data", max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "NNdata/xor_float.net");

	fann_destroy(ann);
}

void testNN(){
	fann_type *calc_out;
	fann_type input[2];

	struct fann *ann = fann_create_from_file("NNdata/xor_float.net");

	input[0] = 0;
	input[1] = 0;
	calc_out = fann_run(ann, input);

	printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
	input[0] = 0;
	input[1] = 1;
	calc_out = fann_run(ann, input);

	printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
	input[0] = 1;
	input[1] = 0;
	calc_out = fann_run(ann, input);

	printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);
	input[0] = 1;
	input[1] = 1;
	calc_out = fann_run(ann, input);

	printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);

	fann_destroy(ann);
}

void buildTrainingFile(){
	std::ofstream training_data;
	training_data.open("NNdata/emg.data");

	int number_of_training_instances = TRAINING_SIZE;
	int number_of_inputs = NUMBER_OF_EMG_ARRAYS;
	int number_of_outputs = NUMBER_OF_GESTURES;
	
	training_data << number_of_training_instances << " ";
	training_data << number_of_inputs << " "; //
	training_data << number_of_outputs << std::endl;




	for (int training_instance_i = 0; training_instance_i < number_of_training_instances; training_instance_i++)
	{
		std::string training_data_filename = getTrainingFilename(training_instance_i);
		FileDataHandler gesture_training_data(training_data_filename, true);

		int solution = static_cast<int>(training_definitives_list[training_instance_i]);

		for (int i = 0; i < number_of_inputs; i++)
		{
			double emg_sum = 0.0;
			for (int k = 0; k < DATA_LENGTH_EMG; k++)
			{
				emg_sum += pow(gesture_training_data.getSensorData(EMG)[i][k], 2);
			}
			training_data << emg_sum;

			if (i != number_of_inputs - 1)
				training_data << " ";
		}
		training_data << std::endl;

		for (int o = 0; o < number_of_outputs; o++)
		{
			if (solution == o)
				training_data << 1;
			else
				training_data << 0;

			if (o != number_of_inputs - 1)
				training_data << " ";
		}

		if (training_instance_i != number_of_training_instances - 1)
			training_data << std::endl;
	}

	training_data.close();
}

void emgTrainNN(){
	const unsigned int num_input = NUMBER_OF_EMG_ARRAYS;
	const unsigned int num_output = NUMBER_OF_GESTURES;
	const unsigned int num_layers = 3;
	const unsigned int num_neurons_hidden = NUMBER_OF_EMG_ARRAYS + 2;
	const float desired_error = (const float) 0.00001;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 1000;

	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, "NNdata/emg.data", max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "NNdata/emg_float.net");

	fann_destroy(ann);
}

void emgTestNN(std::string test_data_filename){
	int number_of_inputs = NUMBER_OF_EMG_ARRAYS;

	FileDataHandler gesture_training_data(test_data_filename, false);

	fann_type *calc_out;
	fann_type input[NUMBER_OF_EMG_ARRAYS];

	struct fann *ann = fann_create_from_file("NNdata/emg_float.net");

	for (int i = 0; i < number_of_inputs; i++)
	{
		double emg_sum = 0.0;
		for (int k = 0; k < DATA_LENGTH_EMG; k++)
		{
			emg_sum += pow(gesture_training_data.getSensorData(EMG)[i][k], 2);
		}
		input[i] = emg_sum;
	}
	calc_out = fann_run(ann, input);

	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		std::cout << gestureToString((Gesture)i) << ": " << calc_out[i] << std::endl;
	}

	fann_destroy(ann);
}
#include "stdafx.h"

#include "FannTest.h"

#include <stdio.h>

#include "floatfann.h"
#include "fann.h"


void trainNN(){
	const unsigned int num_input = 2;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 3;
	const unsigned int num_neurons_hidden = 3;
	const float desired_error = (const float) 0.0001;
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
#include "stdafx.h"

#include <windows.h>
#include <fstream>
#include <iostream>
#include <cmath>

#include <json/json.h>
#include <fftw3.h>

#include "Utility.h"
#include "DataUtility.h"
#include "Constants.h"
#include "SettingsVariables.h"

#include "opennn/opennn.h"


#define min(a,b) (((a)<(b)) ? (a):(b))



void clearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

double crossCorrelation(int maxdelay, double* x, double* y, int size_of_array){
	double xFirst = x[0];
	double yFirst = y[0];

	int i, j;
	double mx, my, sx, sy, sxy, denom, r;
	

	mx = 0;
	my = 0;
	for (i=0; i<size_of_array; i++) {
		mx += x[i] - xFirst;
		my += y[i] - yFirst;
	}
	mx /= size_of_array;
	my /= size_of_array;

	sx = 0;
	sy = 0;
	for (i = 0; i<size_of_array; i++) {
		sx += (x[i] - xFirst - mx) * (x[i] - xFirst - mx);
		sy += (y[i] - yFirst - my) * (y[i] - yFirst - my);
	}
	denom = sqrt(sx*sy);
	r = 0.0;
	int d = -maxdelay;
	for (int delay = -maxdelay; delay < maxdelay; delay++) {
		sxy = 0;
		for (i = 0; i<size_of_array; i++) {
			j = i + delay;
			if (j < 0 || j >= size_of_array)
				continue;
			else
				sxy += (x[i] - xFirst - mx) * (y[j] - yFirst - my);
		}
		
		if (r < sxy / denom){
			r = sxy / denom;
		}

	}

	return r;

}

double calculateEuclideanDistance(double x, double y) {
	return std::sqrt(std::pow((x - y), 2));
}
double calculateDynamicTimeWarpedDistance(double* t0, double* t1, int size_of_array) {

	int m = size_of_array;
	int n = size_of_array;

	// Allocate the Matrix to work on:
	std::vector<std::vector<double>> cost(m, std::vector<double>(n));

	cost[0][0] = calculateEuclideanDistance(t0[0] - t0[0], t1[0] - t1[0]);

	// Calculate the first row:
	for (int i = 1; i < m; i++) {
		cost[i][0] = cost[i - 1][0] + calculateEuclideanDistance(t0[i] - t0[0], t1[0] - t1[0]);
	}

	// Calculate the first column:
	for (int j = 1; j < n; j++) {
		cost[0][j] = cost[0][j - 1] + calculateEuclideanDistance(t0[0] - t0[0], t1[j] - t1[0]);
	}

	// Fill the matrix:
	for (int i = 1; i < m; i++) {
		for (int j = 1; j < n; j++) {
			cost[i][j] = min(cost[i - 1][j], min(cost[i][j - 1], cost[i - 1][j - 1]))
				+ calculateEuclideanDistance(t0[i], t1[j]);
		}
	}

	return cost[m - 1][n - 1];
}
double maxSqrValue(double* array, int n){
	double max = 0;
	for (int i = 0; i < n; i++)
	{
		if (max < pow(array[i],2))
		{
			max = pow(array[i], 2);
		}
	}
	return max;
}

// x is in, y is test
double emgFourierTransformedCompare(double* x, double* y, int n){
	fftw_complex *x_out, *y_out;
	fftw_plan x_p, y_p;

	x_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
	y_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);


	x_p = fftw_plan_dft_r2c_1d(n, x, x_out, FFTW_ESTIMATE);
	y_p = fftw_plan_dft_r2c_1d(n, y, y_out, FFTW_ESTIMATE);

	fftw_execute(x_p);
	fftw_execute(y_p);

	fftw_destroy_plan(x_p);
	fftw_destroy_plan(y_p);


	double* new_x = new double[n];
	double* new_y = new double[n];

	for (int i = 0; i < n; i++)
	{
		new_x[i] = x_out[i][0];
		new_y[i] = y_out[i][0];
	}
	if (!isDtwUsed)
		return crossCorrelation(n / 2, new_x, new_y, n);
	else
		return calculateDynamicTimeWarpedDistance(new_x, new_y, n);

}

double emgEnergyCompare(double* x, double* y, int n){
	double x_max_sqr_value = maxSqrValue(x, n);
	double y_max_sqr_value = maxSqrValue(y, n);

	int intervals = 50;
	int size_of_intervals = n/intervals;

	double* new_x = new double[intervals];
	double* new_y = new double[intervals];

	for (int i = 0; i < intervals; i++){
		double part_sum_x = 0.0;
		double part_sum_y = 0.0;

		for (int j = 0; j < size_of_intervals; ++j)
		{
			part_sum_x += pow(x[i*size_of_intervals + j], 2) / x_max_sqr_value;
			part_sum_y += pow(y[i*size_of_intervals + j], 2) / y_max_sqr_value;
		}
		new_x[i] = part_sum_x;
		new_y[i] = part_sum_y;
	}

	if (!isDtwUsed)
		return crossCorrelation(intervals / 2, new_x, new_y, intervals);
	else
		return calculateDynamicTimeWarpedDistance(new_x, new_y, intervals);
	
}


Json::Value jsonDataArray(std::string dataname, Json::Value obj, int number_of_arrays, int number_of_data){
	const Json::Value& datatype = obj[dataname];
	const Json::Value& data = datatype["data"];

	Json::Value data_vec(Json::arrayValue);
	for (int j = 0; j < number_of_data; j++)
	{
		Json::Value temp_array(Json::arrayValue);
		for (int i = 0; i < number_of_arrays; i++)
		{
			temp_array.append(Json::Value(data[j][i]));
		}
		data_vec.append(Json::Value(temp_array));
	}
	return data_vec;
}
void compressAllFiles(){
	for (int i = 0; i < TRANING_SIZE; i++){
		compressFile(training_filename_list[i]);
	}
	for (int i = 0; i < NUMBER_OF_TESTS; i++){
		compressFile(test_filename_list[i]);
	}

	std::cout << "Comppression finished!" << std::endl;
}
void compressFile(std::string filename){
	std::string input_filename  = TRANING_SET_FOLDER;
	input_filename.append(filename);

	std::ifstream ifs(input_filename);
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);

	Json::Value event;

	event[JSON_EMG_ARRAY_NAME][JSON_ARRAY_DATA_TABLE_NAME] = jsonDataArray(JSON_EMG_ARRAY_NAME, obj, NUMBER_OF_EMG_ARRAYS, DATA_LENGTH_EMG);
	event[JSON_ACC_ARRAY_NAME][JSON_ARRAY_DATA_TABLE_NAME] = jsonDataArray(JSON_ACC_ARRAY_NAME, obj, NUMBER_OF_ACC_ARRAYS, DATA_LENGTH_ACC);
	event[JSON_GYR_ARRAY_NAME][JSON_ARRAY_DATA_TABLE_NAME] = jsonDataArray(JSON_GYR_ARRAY_NAME, obj, NUMBER_OF_GYR_ARRAYS, DATA_LENGTH_GYR);
	event[JSON_ORI_ARRAY_NAME][JSON_ARRAY_DATA_TABLE_NAME] = jsonDataArray(JSON_ORI_ARRAY_NAME, obj, NUMBER_OF_ORI_ARRAYS, DATA_LENGTH_ORI);

	std::ofstream file_id;
	std::string output_filename = TRANING_SET_FOLDER;
	output_filename.append(COMPRESSED_FILENAME_INITIAL);
	output_filename.append(filename);
	std::cout << output_filename << std::endl;
	file_id.open(output_filename);

	Json::StyledWriter styled_writer;
	file_id << styled_writer.write(event);

	file_id.close();
}
std::string getCompressedFilename(int i){
	return getCompressedFilename(training_filename_list[i]);
}

std::string getCompressedFilename(std::string filename){
	std::string  name = COMPRESSED_FILENAME_INITIAL;
	name.append(filename);
	return name;
}


bool isSensorIgnored(Sensor sensor){
	switch (sensor){
	case EMG:
		return !isEmgSensorOn;
		break;
	case ACC:
		return !isAccSensorOn;
		break;
	case GYR:
		return !isGyrSensorOn;
		break;
	case ORI:
		return !isOriSensorOn;
		break;
	default:
		return false;
	}
}
bool isAllSensorOn(){
	return  isEmgSensorOn && isAccSensorOn && isOriSensorOn && isGyrSensorOn;
};

void setDataLengt(int &data_length, Sensor sensor){
	switch (sensor){
	case EMG:
		data_length = DATA_LENGTH_EMG;
		break;
	case ACC:
		data_length = DATA_LENGTH_ACC;
		break;
	case GYR:
		data_length = DATA_LENGTH_GYR;
		break;
	case ORI:
		data_length = DATA_LENGTH_ORI;
		break;
	}
};

void setNumberOfArrays(int &number_of_arrays, Sensor sensor){
	switch (sensor){
	case EMG:
		number_of_arrays = NUMBER_OF_EMG_ARRAYS;
		break;
	case ACC:
		number_of_arrays = NUMBER_OF_ACC_ARRAYS;
		break;
	case GYR:
		number_of_arrays = NUMBER_OF_GYR_ARRAYS;
		break;
	case ORI:
		number_of_arrays = NUMBER_OF_ORI_ARRAYS;
		break;
	}
};

std::string getJsonArrayNameBySensor(Sensor sensor){
	switch (sensor){
	case EMG:
		return JSON_EMG_ARRAY_NAME;
	case ACC:
		return JSON_ACC_ARRAY_NAME;
	case GYR:
		return JSON_GYR_ARRAY_NAME;
	case ORI:
		return JSON_ORI_ARRAY_NAME;
	}
	return "";
}

double compareArrays(double** in, double** test, Sensor sensor){
	int number_of_arrays, data_length;

	setNumberOfArrays(number_of_arrays, sensor);
	setDataLengt(data_length, sensor);
	
	double r = 0;
	for (int i = 0; i < number_of_arrays; i++)
	{
		if (sensor == EMG){
			r += emgFourierTransformedCompare(in[i], test[i], data_length);
		}
		else{
			if (!isDtwUsed)
				r += crossCorrelation(data_length / 2, in[i], test[i], data_length);
			else{
				r += calculateDynamicTimeWarpedDistance(in[i], test[i], data_length);
			}
		}
	}
	return r / number_of_arrays;
}

Gesture gestureComparisonsJsonFile(std::string test_filename){
	FileDataHandler gestureInput(test_filename);
	return gestureComparisons(gestureInput);
}

double maxOfArray(double* a, int size){
	double max = -DBL_MAX;
	for (int i = 0; i < NUMBER_OF_GESTURES; i++){
		if (max < a[i])
		{
			max = a[i];
		}
	}
	return max;
}

Gesture gestureComparisons(DataHandler gesture_input){
	if (!isAllSensorOn())
	{
		std::cout << "Sensor ignored: ";
		for (int k = 0; k < NUMBER_OF_SENSORS; k++)
		{
			Sensor sensor = (Sensor) k;
			if (isSensorIgnored(sensor))
			{
				std::cout << sensorToString(sensor) << ", ";
			}
		}
		std::cout << "\b\b" << "  " << std::endl;
	}

	std::cout << "Comparison method: ";
	if(isDtwUsed)
		std::cout << "Dynamic Time Warping";
	else
		std::cout << "Cross Correlation";
	std::cout << std::endl << std::endl;
	

	Gesture prediction = NONE;
	double r = -DBL_MAX;

	double corr_rs[NUMBER_OF_GESTURES];
	double emg_comparisons[NUMBER_OF_GESTURES];
	int number_of_IMU_sensors = 0;
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		double corr_r = 0.0;
		double emg_comparison = 0.0;
		
		for (int j = 0; j < NUMBER_OF_TRANING_PER_GESTURE; j++)
		{
			std::string test_filename = getCompressedFilename(i * NUMBER_OF_TRANING_PER_GESTURE + j);
			std::cout << test_filename;
			FileDataHandler gesture_test(test_filename);
			number_of_IMU_sensors = 0;
			for (int k = 0; k < NUMBER_OF_SENSORS; k++)
			{
				Sensor sensor = static_cast<Sensor>(k);
				if (isSensorIgnored(sensor))
					continue;
				else if (sensor == EMG){
					emg_comparison += compareArrays(gesture_input.getSensorData(sensor), gesture_test.getSensorData(sensor), sensor);
				}
				else{
					corr_r += compareArrays(gesture_input.getSensorData(sensor), gesture_test.getSensorData(sensor), sensor);
					
					number_of_IMU_sensors++;
				}
			}

			corr_rs[i] = corr_r;
			emg_comparisons[i] = emg_comparison;
			std::cout << '\r';
			std::cout << "                                                                                                      ";
			std::cout << '\r';

		}
		if (!isDtwUsed){
			double similarity = corr_r + emg_comparison;

			std::cout << gestureToString(static_cast<Gesture>(i)) << ": r = " << similarity << ",\tIMU = " << corr_r << ",\tEMG = " << emg_comparison << std::endl;
			if (similarity > r)
			{
				r = similarity;
				prediction = static_cast<Gesture>(i);
			}
		}
	}


	if (isDtwUsed){
		double largest_corr_r = maxOfArray(corr_rs, NUMBER_OF_GESTURES);
		double largest_emg_comparison = maxOfArray(emg_comparisons, NUMBER_OF_GESTURES);

		for (int i = 0; i < NUMBER_OF_GESTURES; i++){
			corr_rs[i] = (1 - corr_rs[i]/largest_corr_r) * number_of_IMU_sensors;
			emg_comparisons[i] = 1 - emg_comparisons[i] / largest_emg_comparison;

			if(number_of_IMU_sensors == 0){
				corr_rs[i] = 0;
			}
			if (isSensorIgnored(EMG)){
				emg_comparisons[i] = 0;
			}

			double similarity = corr_rs[i] + emg_comparisons[i];

			std::cout << gestureToString(static_cast<Gesture>(i)) << ": r = " << similarity << ",\tIMU = " << corr_rs[i] << ",\tEMG = " << emg_comparisons[i] << std::endl;
			if (similarity > r)
			{
				r = similarity;
				prediction = static_cast<Gesture>(i);
			}
		}
	}
	return prediction;
}


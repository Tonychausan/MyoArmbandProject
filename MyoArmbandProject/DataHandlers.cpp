#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <cmath>
#include <json/json.h>

#include "Utility.h"
#include "DataUtility.h"
#include "Constants.h"


DataHandler::DataHandler(){

};


FileDataHandler::FileDataHandler(File input_file, bool isTrainingSet)
	:DataHandler()
{
	file = new File();
	file->gesture = input_file.gesture;
	
	std::string filetype_folder = isTrainingSet ? TRAINING_SET_FOLDER : TEST_SET_FOLDER;

	std::string filename = DATA_SET_FOLDER;
	filename.append(COMPRESSED_DATA_FOLDER);
	filename.append(filetype_folder);
	filename.append(input_file.filename);

	file->filename = filename;

	generateAllSensorData();
}


void FileDataHandler::generateSensorData(Json::Value obj, Sensor sensor){
	const Json::Value& jsonObject = obj[getJsonArrayNameBySensor(sensor)];
	const Json::Value& data = jsonObject["data"];

	int number_of_arrays, data_length;

	setNumberOfArrays(number_of_arrays, sensor);
	setDataLengt(data_length, sensor);

	double **work_arrays = new double*[number_of_arrays];
	for (int i = 0; i < number_of_arrays; i++)
	{
		work_arrays[i] = new double[data_length];
		for (int j = 0; j < data_length; j++)
		{
			work_arrays[i][j] = data[j][i].asDouble();
		}
	}

	setSensorData(work_arrays, sensor);
}

void FileDataHandler::generateAllSensorData(){
	std::ifstream ifs(file->filename);
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);

	for (int k = 0; k < NUMBER_OF_SENSORS; k++)
	{
		Sensor sensor = static_cast<Sensor>(k);
		generateSensorData(obj, sensor);
	}
}

void DataHandler::setSensorData(double** array, Sensor sensor){
	switch (sensor){
	case EMG:
		emg_data = array;
		break;
	case ACC:
		acc_data = array;
		break;
	case GYR:
		gyr_data = array;
		break;
	case ORI:
		ori_data = array;
		break;
	}
}

double** DataHandler::getSensorData(Sensor sensor){
	switch (sensor){
	case EMG:
		return emg_data;
	case ACC:
		return gyr_data;
	case GYR:
		return acc_data;
	default: //ORI
		return ori_data;
	}
}

InputDataHandler::InputDataHandler(){
	for (int k = 0; k < NUMBER_OF_SENSORS; k++)
	{
		Sensor sensor = static_cast<Sensor>(k);
		generateSensorData(sensor);
	}
}

void InputDataHandler::generateSensorData(Sensor sensor){
	int number_of_arrays, data_length;

	setNumberOfArrays(number_of_arrays, sensor);
	setDataLengt(data_length, sensor);

	double **work_data = new double*[number_of_arrays];
	for (int i = 0; i < number_of_arrays; i++)
	{
		work_data[i] = new double[data_length];
	}

	setSensorData(work_data, sensor);
}

void InputDataHandler::setSensorDataValueAt(int position, int array_id, double value, Sensor sensor){
	int number_of_arrays, data_length;

	setNumberOfArrays(number_of_arrays, sensor);
	setDataLengt(data_length, sensor);

	if (array_id > number_of_arrays || position > data_length)
	{
		std::cout << sensorToString(sensor) << ": Out of bound in setSensorArrayValueAt!! (" << position << "," << array_id << ")" << std::endl;
		return;
	}

	double **work_data = getSensorData(sensor);
	work_data[array_id][position] = value;
}
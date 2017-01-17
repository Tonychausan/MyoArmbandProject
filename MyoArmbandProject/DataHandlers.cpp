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

DataFileHandler::DataFileHandler(std::string name)
	:DataHandler()
{
	filename = "data/";
	filename.append(name);

	generateSensorArrays();
}


void DataFileHandler::generateSensorArray(Json::Value obj, Sensor sensor){
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

	setSensorArray(work_arrays, sensor);
}

void DataFileHandler::generateSensorArrays(){
	std::ifstream ifs(filename);
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);

	for (int k = 0; k < NUMBER_OF_SENSORS; k++)
	{
		Sensor sensor = static_cast<Sensor>(k);
		generateSensorArray(obj, sensor);
	}
}

void DataHandler::setSensorArray(double** array, Sensor sensor){
	switch (sensor){
	case EMG:
		emg_arrays = array;
		break;
	case ACC:
		acc_arrays = array;
		break;
	case GYR:
		gyr_arrays = array;
		break;
	case ORI:
		ori_arrays = array;
		break;
	}
}

double** DataHandler::getWorkingArrays(Sensor sensor){
	double **work_arrays;
	switch (sensor){
	case EMG:
		work_arrays = emg_arrays;
		break;
	case ACC:
		work_arrays = acc_arrays;
		break;
	case GYR:
		work_arrays = gyr_arrays;
		break;
	default: //ORI
		work_arrays = ori_arrays;
		break;
	}
	return work_arrays;
}

double** DataHandler::getArrays(Sensor sensor){
	switch (sensor){
	case EMG:
		return emg_arrays;
	case ACC:
		return gyr_arrays;
	case GYR:
		return acc_arrays;
	case ORI:
		return ori_arrays;
	default:
		return NULL;
	}
}

DataInputHandler::DataInputHandler(){
	for (int k = 0; k < NUMBER_OF_SENSORS; k++)
	{
		Sensor sensor = static_cast<Sensor>(k);
		generateSensorArrays(sensor);
	}
}

void DataInputHandler::generateSensorArrays(Sensor sensor){
	int number_of_arrays, data_length;

	setNumberOfArrays(number_of_arrays, sensor);
	setDataLengt(data_length, sensor);

	double **workArrays = new double*[number_of_arrays];
	for (int i = 0; i < number_of_arrays; i++)
	{
		workArrays[i] = new double[data_length];
	}

	setSensorArray(workArrays, sensor);
}

void DataInputHandler::setSensorArrayValueAt(int position, int array_id, double value, Sensor sensor){
	int number_of_arrays, data_length;

	setNumberOfArrays(number_of_arrays, sensor);
	setDataLengt(data_length, sensor);

	if (array_id > number_of_arrays || position > data_length)
	{
		std::cout << sensorToString(sensor) << ": Out of bound in setSensorArrayValueAt!! (" << position << "," << array_id << ")" << std::endl;
		return;
	}

	double **work_arrays = getWorkingArrays(sensor);
	work_arrays[array_id][position] = value;
}
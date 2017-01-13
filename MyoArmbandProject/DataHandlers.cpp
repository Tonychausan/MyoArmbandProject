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

	generateDataArrays();
}


void DataFileHandler::generateSensorDataArray(Json::Value obj, Sensor sensor){
	const Json::Value& jsonObject = obj[getJsonArrayNameBySensor(sensor)];
	const Json::Value& data = jsonObject["data"];

	int numberOfArrays, dataLength;

	setNumberOfArrays(numberOfArrays, sensor);
	setDataLengt(dataLength, sensor);

	double **workArrays = new double*[numberOfArrays];
	for (int i = 0; i < numberOfArrays; i++)
	{
		workArrays[i] = new double[dataLength];
		for (int j = 0; j < dataLength; j++)
		{
			workArrays[i][j] = data[j][i].asDouble();
		}
	}

	setSensorArray(workArrays, sensor);
}

void DataFileHandler::generateDataArrays(){
	std::ifstream ifs(filename);
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);

	for (int k = 0; k < NUMBER_OF_SENSORS; k++)
	{
		Sensor sensor = static_cast<Sensor>(k);
		generateSensorDataArray(obj, sensor);
	}
}

void DataHandler::setSensorArray(double** array, Sensor sensor){
	switch (sensor){
	case EMG:
		emgArrays = array;
		break;
	case ACC:
		accArrays = array;
		break;
	case GYR:
		gyrArrays = array;
		break;
	case ORI:
		oriArrays = array;
		break;
	}
}

double** DataHandler::getWorkingArrays(Sensor sensor){
	double **workArrays;
	switch (sensor){
	case EMG:
		workArrays = emgArrays;
		break;
	case ACC:
		workArrays = accArrays;
		break;
	case GYR:
		workArrays = gyrArrays;
		break;
	default: //ORI
		workArrays = oriArrays;
		break;
	}
	return workArrays;
}

double** DataHandler::getArrays(Sensor sensor){
	switch (sensor){
	case EMG:
		return emgArrays;
	case ACC:
		return gyrArrays;
	case GYR:
		return accArrays;
	case ORI:
		return oriArrays;
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
	int numberOfArrays, dataLength;

	setNumberOfArrays(numberOfArrays, sensor);
	setDataLengt(dataLength, sensor);

	double **workArrays = new double*[numberOfArrays];
	for (int i = 0; i < numberOfArrays; i++)
	{
		workArrays[i] = new double[dataLength];
	}

	setSensorArray(workArrays, sensor);
}

void DataInputHandler::setSensorArrayValueAt(int i, int j, double value, Sensor sensor){
	int numberOfArrays, dataLength;

	setNumberOfArrays(numberOfArrays, sensor);
	setDataLengt(dataLength, sensor);

	if (j > numberOfArrays || i > dataLength)
	{
		std::cout << sensorToString(sensor) << ": Out of bound in setSensorArrayValueAt!! (" << i << "," << j << ")" << std::endl;
		return;
	}

	double **workArrays = getWorkingArrays(sensor);
	workArrays[j][i] = value;
}
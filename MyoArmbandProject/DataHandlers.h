#ifndef DataHandlersH
#define DataHandlersH

#include <String>
#include <json/json.h>

#include "DataUtility.h"

class DataHandler{
protected:
	// Sensor Data
	double** emg_data;
	double** gyr_data;
	double** acc_data;
	double** ori_data;
public:
	DataHandler();
	/*
	* Function: setSensorData
	* ----------------------------
	*   Store array to one of the Sensor data given by sensor
	*
	*	array: array
	*	sensor: sensor
	*/
	void setSensorData(double** array, Sensor sensor);

	/*
	* Function: setSensorData
	* ----------------------------
	*   returns Sensor data given by sensor
	*
	*	sensor: sensor
	*
	*	return: sensor data given by sensor
	*/
	double** getSensorData(Sensor sensor);
};

class FileDataHandler : public DataHandler{
private:
	std::string filename; //JSON-filename
public:
	FileDataHandler(std::string);

	/*
	* Function: generateSensorData
	* ----------------------------
	*   Generate sensor data for sensor from JSON-Object
	*
	*	obj: JSON-object
	*	sensor: sensor
	*/
	void generateSensorData(Json::Value obj, Sensor sensor);
	/*
	* Function: generateAllSensorData
	* ----------------------------
	*   generate all sensor data from JSON-file filename
	*/
	void generateAllSensorData();
};

class InputDataHandler : public DataHandler{
private:
public:
	InputDataHandler();
	/*
	* Function: generateSensorData
	* ----------------------------
	*   Generate empty sensor data for sensor
	*
	*	sensor: sensor
	*/
	void generateSensorData(Sensor);
	/*
	* Function: setSensorDataValueAt
	* ----------------------------
	*   Store a value to a given position of the sensor data
	*
	*	position: position to store value
	*	array_id: the array number of the given sensor data
	*	value: value to store in sensor data
	*	sensor: sensor
	*/
	void setSensorDataValueAt(int position, int array_id, double value, Sensor sensor);
};

#endif
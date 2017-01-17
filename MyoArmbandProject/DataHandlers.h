#ifndef DataHandlersH
#define DataHandlersH

#include <String>
#include <json/json.h>

#include "DataUtility.h"

class DataHandler{
protected:
	// Sensor Arrays
	double** emg_arrays;
	double** gyr_arrays;
	double** acc_arrays;
	double** ori_arrays;
public:
	DataHandler();
	/*
	* Function: setSensorArray
	* ----------------------------
	*   Store array to one of the Sensor arrays given by sensor
	*
	*	array: array
	*	sensor: sensor
	*/
	void setSensorArray(double** array, Sensor sensor);


	double** getWorkingArrays(Sensor);
	double** getArrays(Sensor);
};

class DataFileHandler : public DataHandler{
private:
	std::string filename;
public:
	DataFileHandler(std::string);
	void generateSensorArray(Json::Value, Sensor);
	void generateSensorArrays();
};

class DataInputHandler : public DataHandler{
private:
public:
	DataInputHandler();
	void generateSensorArrays(Sensor);
	void setSensorArrayValueAt(int position, int array_id, double value, Sensor sensor);
	void reset();
};

#endif
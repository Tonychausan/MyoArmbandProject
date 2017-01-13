#ifndef DataHandlersH
#define DataHandlersH

#include <String>
#include <json/json.h>

#include "DataUtility.h"

class DataHandler{
protected:
	double** emgArrays;
	double** gyrArrays;
	double** accArrays;
	double** oriArrays;
public:
	DataHandler();
	void setSensorArray(double**, Sensor);
	double** getWorkingArrays(Sensor);
	double** getArrays(Sensor);
};

class DataFileHandler : public DataHandler{
private:
	std::string filename;
public:
	DataFileHandler(std::string);
	void generateSensorDataArray(Json::Value, Sensor);
	void generateDataArrays();
};

class DataInputHandler : public DataHandler{
private:
public:
	DataInputHandler();
	void generateSensorArrays(Sensor);
	void setSensorArrayValueAt(int, int, double value, Sensor sensor);
	void reset();
};

#endif
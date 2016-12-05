#include "stdafx.h"

#include <windows.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <json/json.h>

#include "Utility.h"

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

// http://paulbourke.net/miscellaneous/correlate/
double crossCorrelation(int maxdelay, double* x, double* y, int n){
	// first values
	double xFirst = x[0];
	double yFirst = y[0];

	int i, j;
	double mx, my, sx, sy, sxy, denom, r;
	/* Calculate the mean of the two series x[], y[] */
	mx = 0;
	my = 0;
	for (i=0; i<n; i++) {
		mx += x[i] - xFirst;
		my += y[i] - yFirst;
	}
	mx /= n;
	my /= n;

	/* Calculate the denominator */
	sx = 0;
	sy = 0;
	for (i = 0; i<n; i++) {
		sx += (x[i] - xFirst - mx) * (x[i] - xFirst - mx);
		sy += (y[i] - yFirst - my) * (y[i] - yFirst - my);
	}
	denom = sqrt(sx*sy);

	/* Calculate the correlation series */
	r = 0.0;
	for (int delay = -maxdelay; delay < maxdelay; delay++) {
		sxy = 0;
		for (i = 0; i<n; i++) {
			j = i + delay;
			if (j < 0 || j >= n)
				continue;
			else
				sxy += (x[i] - xFirst - mx) * (y[j] - yFirst - my);
		}
		
		if (r < sxy / denom){
			r = sxy / denom;
		}


		/* r is the correlation coefficient at "delay" */

	}
	return r;
}

double CalculateEuclideanDistance(double x, double y) {
	return std::sqrt(std::pow((x - y), 2));
}

double CalculateDynamicTimeWarpedDistance(double* t0, double* t1, int size) {

	size_t m = size;
	size_t n = size;

	// Allocate the Matrix to work on:
	std::vector<std::vector<double>> cost(m, std::vector<double>(n));

	cost[0][0] = CalculateEuclideanDistance(t0[0], t1[0]);

	// Calculate the first row:
	for (int i = 1; i < m; i++) {
		cost[i][0] = cost[i - 1][0] + CalculateEuclideanDistance(t0[i], t1[0]);
	}

	// Calculate the first column:
	for (int j = 1; j < n; j++) {
		cost[0][j] = cost[0][j - 1] + CalculateEuclideanDistance(t0[0], t1[j]);
	}

	// Fill the matrix:
	for (int i = 1; i < m; i++) {
		for (int j = 1; j < n; j++) {
			cost[i][j] = min(cost[i - 1][j], min(cost[i][j - 1], cost[i - 1][j - 1]))
				+ CalculateEuclideanDistance(t0[i], t1[j]);
		}
	}

	return cost[m - 1][n - 1];
}

double MaxSqrValue(double* array, int n){
	double max = 0;
	for (int i = 0; i < n; i++)
	{
		if (max < array[i]*array[i])
		{
			max = array[i]*array[i];
		}
	}
	return max;
}

// x is in, y is test
double EmgEnergyCompare(double* x, double* y, int n){
	double xMaxSqrValue = MaxSqrValue(x, n);
	double yMaxSqrValue = MaxSqrValue(y, n);

	double sumX = 0.0;
	double sumY = 0.0;
	for (int i = 0; i < n; i++)
	{
		sumX += (x[i] * x[i])/xMaxSqrValue;
		sumY += (y[i] * y[i])/yMaxSqrValue;
	}
	if (sumX < sumY){
		return sumX/sumY;
	}
	else{
		return sumY/sumX;
	}
}

Json::Value jsonDataArray(std::string dataname, Json::Value obj, int numberOfArrays, int numberOfData){
	const Json::Value& dataType = obj[dataname];
	const Json::Value& data = dataType["data"];

	Json::Value dataVec(Json::arrayValue);
	for (int j = 0; j < numberOfData; j++)
	{
		Json::Value tempArray(Json::arrayValue);
		for (int i = 0; i < numberOfArrays; i++)
		{
			tempArray.append(Json::Value(data[j][i]));
		}
		dataVec.append(Json::Value(tempArray));
	}
	return dataVec;
}
void compressAllJsonFiles(){
	for (int i = 0; i < testFileListSize; i++){
		compressJsonFile(testFileList[i]);
	}
	for (int i = 0; i < preSampledRecordFileListSize; i++){
		compressJsonFile(preSampledRecordFileList[i]);
	}

	std::cout << "Comppression finished!" << std::endl;
}
void compressJsonFile(std::string name){
	std::string inFilename  = "data/";
	inFilename.append(name);

	std::ifstream ifs(inFilename);
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);

	Json::Value event;

	event["emg"]["data"] = jsonDataArray("emg", obj, NUMBER_OF_EMG_ARRAYS, DATA_EMG_LENGTH);
	event["acc"]["data"] = jsonDataArray("acc", obj, NUMBER_OF_ACC_ARRAYS, DATA_ACC_LENGTH);
	event["gyr"]["data"] = jsonDataArray("gyr", obj, NUMBER_OF_GYR_ARRAYS, DATA_GYR_LENGTH);
	event["ori"]["data"] = jsonDataArray("ori", obj, NUMBER_OF_ORI_ARRAYS, DATA_ORI_LENGTH);

	std::ofstream file_id;
	std::string outFilename = "data/";
	outFilename.append("compressed-");
	outFilename.append(name);
	std::cout << outFilename << std::endl;
	file_id.open(outFilename);

	Json::StyledWriter styledWriter;
	file_id << styledWriter.write(event);

	//file_id.close();
}
std::string getCompressedFilename(int i){
	return getCompressedFilename(testFileList[i]);
}

std::string getCompressedFilename(std::string filename){
	std::string  name = "compressed-";
	name.append(filename);
	return name;
}

std::string gestureToString(Gesture gesture){
	switch (gesture){
	case EAT:
		return "EAT";
		break;
	case HELP:
		return "HELP";
		break;
	case SLEEP:
		return "SLEEP";
		break;
	case THANKYOU:
		return "THANKYOU";
		break;
	case WHY:
		return "WHY";
		break;

	default:
		return "";
		break;
	}
}

std::string sensorToString(Sensor sensor){
	switch (sensor){
	case EMG:
		return "Emg";
		break;
	case ACC:
		return "Acc";
		break;
	case GYR:
		return "Gyr";
		break;
	case ORI:
		return "Ori";
		break;
	default:
		return "";
	}
}

bool isThisSensorIgnored(Sensor sensor){
	for (int i = 0; i < sensorToIgnorLength; i++){
		if (sensor == sensorToIgnor[i]){
			return true;
		}
	}
	return false;
}

void setDataLengt(int &dataLength, Sensor sensor){
	switch (sensor){
	case EMG:
		dataLength = DATA_EMG_LENGTH;
		break;
	case ACC:
		dataLength = DATA_ACC_LENGTH;
		break;
	case GYR:
		dataLength = DATA_GYR_LENGTH;
		break;
	case ORI:
		dataLength = DATA_ORI_LENGTH;
		break;
	}
};

void setNumberOfArrays(int &numberOfArrays, Sensor sensor){
	switch (sensor){
	case EMG:
		numberOfArrays = NUMBER_OF_EMG_ARRAYS;
		break;
	case ACC:
		numberOfArrays = NUMBER_OF_ACC_ARRAYS;
		break;
	case GYR:
		numberOfArrays = NUMBER_OF_GYR_ARRAYS;
		break;
	case ORI:
		numberOfArrays = NUMBER_OF_ORI_ARRAYS;
		break;
	}
};

std::string getJsonArrayNameBySensor(Sensor sensor){
	switch (sensor){
	case EMG:
		return "emg";
	case ACC:
		return "acc";
	case GYR:
		return "gyr";
	case ORI:
		return "ori";
	}
	return "";
}

double compareArrays(double** in, double** test, Sensor sensor){
	int numberOfArrays, dataLength;

	setNumberOfArrays(numberOfArrays, sensor);
	setDataLengt(dataLength, sensor);
	
	double r = 0;
	
	for (int i = 0; i < numberOfArrays; i++)
	{
		if (sensor == EMG){
			r += EmgEnergyCompare(in[i], test[i], dataLength);
		}
		else{
			r += crossCorrelation(dataLength / 2, in[i], test[i], dataLength);
			//r += CalculateDynamicTimeWarpedDistance(in[i],test[i], dataLength);
		}
	}
	return r / numberOfArrays;
}

Gesture gestureComparisonsJsonFile(std::string testfile){
	DataFileHandler gestureInput(testfile);
	return gestureComparisons(gestureInput);
}

Gesture gestureComparisons(DataHandler gestureInput){
	if (sensorToIgnorLength > 0)
	{
		std::cout << "Sensor ignored: ";
		Sensor sensor = sensorToIgnor[0];
		std::cout << sensorToString(sensor);
		for (int k = 1; k < sensorToIgnorLength; k++)
		{
			sensor = sensorToIgnor[k];
			std::cout << ", " << sensorToString(sensor);
		}

		std::cout << std::endl << std::endl;
	}

	Gesture prediction = NONE;
	double r = 0.0;
	//double r = -1;
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		double temp_r = 0.0;
		for (int j = 0; j < NUMBER_OF_TEST_PER_GESTURE; j++)
		{
			std::string testFilename = getCompressedFilename(i * NUMBER_OF_TEST_PER_GESTURE + j);
			std::cout << testFilename;
			DataFileHandler testGesture(testFilename);
			for (int k = 0; k < NUMBER_OF_SENSORS; k++)
			{
				Sensor sensor = static_cast<Sensor>(k);
				if (isThisSensorIgnored(sensor))
					continue;
				temp_r += compareArrays(gestureInput.getArrays(sensor), testGesture.getArrays(sensor), sensor);
			}

			std::cout << '\r';
			std::cout << "                                                                                      ";
			std::cout << '\r';
		}
		std::cout << gestureToString(static_cast<Gesture>(i)) << ": r = " << temp_r << std::endl;
		if (temp_r > r)
		{
			r = temp_r;
			prediction = static_cast<Gesture>(i);
		}
	}
	return prediction;
}

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
		std::cout << sensorToString(sensor) << ": Out of bound in setSensorArrayValueAt!! ("<< i << "," << j << ")" << std::endl;
		return;
	}

	double **workArrays = getWorkingArrays(sensor);
	workArrays[j][i] = value;
}

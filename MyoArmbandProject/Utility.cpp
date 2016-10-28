#include "stdafx.h"

#include <windows.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <json/json.h>

#include "Utility.h"

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
			/* Or should it be (?)
			if (j < 0 || j >= n)
			sxy += (x[i] - mx) * (-my);
			else
			sxy += (x[i] - mx) * (y[j] - my);
			*/
		}
		
		if (abs(r) < abs(sxy/denom)){
			r = sxy / denom;
		}

		/* r is the correlation coefficient at "delay" */

	}
	return abs(r);
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

	file_id.close();
}
std::string getCompressedFilename(int i){
	std::string  name = "compressed-";
	name.append(testFileList[i]);
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
	}
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

void setdataLengthMargin(int &margin, Sensor sensor){
	switch (sensor){
	case EMG:
		margin = DATA_EMG_LENGTH_MARGIN;
		break;
	case ACC:
		margin = DATA_ACC_LENGTH_MARGIN;
		break;
	case GYR:
		margin = DATA_GYR_LENGTH_MARGIN;
		break;
	case ORI:
		margin = DATA_ORI_LENGTH_MARGIN;
		break;
	}
}

std::string getJsonArrayNameBySensor(Sensor sensor){
	switch (sensor){
	case EMG:
		return "emg";
		break;
	case ACC:
		return "acc";
		break;
	case GYR:
		return "gyr";
		break;
	case ORI:
		return "ori";
		break;
	}
	return "";
}

double compareArrays(double** in, double** test, Sensor sensor){
	int numberOfArrays;
	int dataLength;
	int dataLengthMargin;

	setNumberOfArrays(numberOfArrays, sensor);
	setDataLengt(dataLength, sensor);
	setdataLengthMargin(dataLengthMargin, sensor);

	double r = 0;
	for (int i = 0; i < numberOfArrays + dataLengthMargin; i++)
	{
		r += crossCorrelation(dataLength/2, in[i], test[i], dataLength);
	}
	return r / numberOfArrays;
}

Gesture gestureComparisons(std::string testfile){
	Gesture prediction = NONE;
	DataFileHandler gestureInput(testfile);
	double r = 0.0;

	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		double temp_r = 0.0;
		for (int j = 0; j < NUMBER_OF_TEST_PER_GESTURE; j++)
		{
			std::string testFilename = getCompressedFilename(i * NUMBER_OF_TEST_PER_GESTURE + j);
			std::cout << testFilename << std::endl;
			DataFileHandler testGesture(testFilename);
			for (int k = 0; k < NUMBER_OF_SENSORS; k++)
			{
				Sensor sensor = static_cast<Sensor>(k);
				temp_r += compareArrays(gestureInput.getArrays(sensor), testGesture.getArrays(sensor), sensor);
			}
		}
		std::cout << gestureToString(static_cast<Gesture>(i)) << ": " << temp_r << std::endl;
		if (temp_r > r)
		{
			r = temp_r;
			prediction = static_cast<Gesture>(i);
		}
	}
	return prediction;
}
Gesture gestureComparisons2(DataInputHandler gestureInput){
	Gesture prediction = NONE;
	double r = 0.0;

	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		double temp_r = 0.0;
		for (int j = 0; j < NUMBER_OF_TEST_PER_GESTURE; j++)
		{
			std::string testFilename = getCompressedFilename(i * NUMBER_OF_TEST_PER_GESTURE + j);
			std::cout << testFilename << std::endl;
			DataFileHandler testGesture(testFilename);
			for (int k = 0; k < 2; k++)
			{
				Sensor sensor = static_cast<Sensor>(k);
				temp_r += compareArrays(gestureInput.getArrays(sensor), testGesture.getArrays(sensor), sensor);
			}
		}
		std::cout << gestureToString(static_cast<Gesture>(i)) << ": " << temp_r << std::endl;
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

void DataFileHandler::generateDataArrays(){
	std::ifstream ifs(filename);
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);



	const Json::Value& emg = obj["emg"];
	const Json::Value& data = emg["data"];

	emgArrays = new double*[NUMBER_OF_EMG_ARRAYS];
	for (int i = 0; i < NUMBER_OF_EMG_ARRAYS; i++)
	{
		emgArrays[i] = new double[DATA_EMG_LENGTH];
		for (int j = 0; j < DATA_EMG_LENGTH; j++)
		{
			emgArrays[i][j] = data[j][i].asDouble();
		}
	}

	// Generate Accelerometer Arrays
	const Json::Value& acc = obj["acc"];
	const Json::Value& accData = acc["data"];
	accArrays = new double*[NUMBER_OF_ACC_ARRAYS];
	for (int i = 0; i < NUMBER_OF_ACC_ARRAYS; i++)
	{
		accArrays[i] = new double[DATA_ACC_LENGTH];
		for (int j = 0; j < DATA_ACC_LENGTH; j++)
		{
			accArrays[i][j] = accData[j][i].asDouble();
		}
	}

	// Generate Gyroscope Arrays
	const Json::Value& gyr = obj["gyr"];
	const Json::Value& gyrData = gyr["data"];
	gyrArrays = new double*[NUMBER_OF_GYR_ARRAYS];
	for (int i = 0; i < NUMBER_OF_GYR_ARRAYS; i++)
	{
		gyrArrays[i] = new double[DATA_GYR_LENGTH];
		for (int j = 0; j < DATA_GYR_LENGTH; j++)
		{
			gyrArrays[i][j] = gyrData[j][i].asDouble();
		}
	}

	// Generate Orientation Arrays
	const Json::Value& ori = obj["ori"];
	const Json::Value& oriData = emg["data"];
	oriArrays = new double*[NUMBER_OF_ORI_ARRAYS];
	for (int i = 0; i < NUMBER_OF_ORI_ARRAYS; i++)
	{
		oriArrays[i] = new double[DATA_ORI_LENGTH];
		for (int j = 0; j < DATA_ORI_LENGTH; j++)
		{
			oriArrays[i][j] = oriData[j][i].asDouble();
		}
	}
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
	}
}

DataInputHandler::DataInputHandler(){
	emgArrays = new double*[NUMBER_OF_EMG_ARRAYS];
	for (int i = 0; i < NUMBER_OF_EMG_ARRAYS; i++)
	{
		emgArrays[i] = new double[DATA_EMG_LENGTH];
	}

	accArrays = new double*[NUMBER_OF_ACC_ARRAYS];
	for (int i = 0; i < NUMBER_OF_ACC_ARRAYS; i++)
	{
		accArrays[i] = new double[DATA_ACC_LENGTH];
	}

	gyrArrays = new double*[NUMBER_OF_GYR_ARRAYS];
	for (int i = 0; i < NUMBER_OF_GYR_ARRAYS; i++)
	{
		gyrArrays[i] = new double[DATA_GYR_LENGTH];
	}

	oriArrays = new double*[NUMBER_OF_ORI_ARRAYS];
	for (int i = 0; i < NUMBER_OF_ORI_ARRAYS; i++)
	{
		oriArrays[i] = new double[DATA_ORI_LENGTH];
	}
}

void DataInputHandler::setSensorArray(int i, int j, double value, Sensor sensor){
	int numberOfArrays;
	int dataLength;
	int dataLengthMargin;

	setNumberOfArrays(numberOfArrays, sensor);
	setDataLengt(dataLength, sensor);
	setdataLengthMargin(dataLengthMargin, sensor);

	if (j > numberOfArrays || i > dataLength + dataLengthMargin)
	{
		std::cout << sensorToString(sensor) << ": Out of bound in setSensorArray!! ("<< i << "," << j << ")" << std::endl;
		return;
	}

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

	workArrays[j][i] = value;
}

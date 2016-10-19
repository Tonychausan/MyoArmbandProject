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

double crossCorrelation(int maxdelay, double* x, double* y, int n){
	int i, j;
	double mx, my, sx, sy, sxy, denom, r;
	/* Calculate the mean of the two series x[], y[] */
	mx = 0;
	my = 0;
	for (i=0; i<n; i++) {
		mx += x[i];
		my += y[i];
	}
	mx /= n;
	my /= n;

	/* Calculate the denominator */
	sx = 0;
	sy = 0;
	for (i = 0; i<n; i++) {
		sx += (x[i] - mx) * (x[i] - mx);
		sy += (y[i] - my) * (y[i] - my);
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
				sxy += (x[i] - mx) * (y[j] - my);
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
	return r;
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
	std::string files[] = {"eat01.json",
		"eat02.json",
		"help01.json",
		"help02.json"
	};

	for (int i = 0; i < 4; i++){
		compressJsonFile(files[i]);
	}
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

CsvHandler::CsvHandler(std::string name)
{
	setFilename(name);
}

void CsvHandler::addSampleDataLine(std::string inputLine)
{
	std::ofstream file;
	file.open(filename, std::ios::app);

	file << inputLine << std::endl;

	file.close();
}

void CsvHandler::createFile()
{
	std::ofstream file;
	file.open(filename);
	file.close();
}

void CsvHandler::removeFile()
{
	std::remove(filename.c_str());
}

void CsvHandler::setFilename(std::string name)
{
	filename = name;
}

EmgCsvHandler::EmgCsvHandler(std::string name)
	:CsvHandler(name)
{
	std::string tempFoldername = foldername;
	setFilename(tempFoldername.append(name));
}

void EmgCsvHandler::addEmgSampleDataLine(std::array<int8_t, 8> emgData)
{
	std::string addLine = std::to_string(emgData[0]);
	for (int i = 1; i < 8; i++) {
		std::string tempString = std::to_string(emgData[i]);

		addLine.append(",");
		addLine.append(tempString);
	}
	addLine.append("\n");
	addSampleDataLine(addLine);
}

DataHandler::DataHandler(std::string name){
	filename = "data/";
	filename.append(name);

	generateDataArrays();
}



void DataHandler::printEmg(){
	std::ifstream ifs(filename);
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);
	const Json::Value& emg = obj["emg"];
	const Json::Value& data = emg["data"];

	for (int i = 0; i < DATA_ARRAY_LENGTH; i++){
		for (int j = 0; j < NUMBER_OF_EMG_ARRAYS - 1; j++){
			std::cout << data[i][j] << ",";
		}
		std::cout << data[i][7] << std::endl;
	}

	system("PAUSE");

}

void DataHandler::generateDataArrays(){
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

double** DataHandler::getEmgArrays(){
	return emgArrays;
}
double** DataHandler::getGyrArrays()
{
	return gyrArrays;
}
double** DataHandler::getAccArrays()
{
	return accArrays;
}
double** DataHandler::getOriArrays()
{
	return oriArrays;
}

#ifndef UtilityH
#define UtilityH

#include <string>
#include <array>
#include <myo/myo.hpp>

static bool isProgramRunning = true;

static int counter = 20; //frequency counter
static time_t start;

static const int DATA_ARRAY_LENGTH= 400;


static const int DATA_TIME_INTERVAL = 2;
static const int DATA_EMG_LENGTH = 200 * DATA_TIME_INTERVAL;
static const int DATA_ORI_LENGTH = 50 * DATA_TIME_INTERVAL;;
static const int DATA_ACC_LENGTH = 50 * DATA_TIME_INTERVAL;;
static const int DATA_GYR_LENGTH = 50 * DATA_TIME_INTERVAL;;

static const int NUMBER_OF_EMG_ARRAYS = 8;
static const int NUMBER_OF_ACC_ARRAYS = 3;
static const int NUMBER_OF_GYR_ARRAYS = 3;
static const int NUMBER_OF_ORI_ARRAYS = 4;


void clearScreen();
double crossCorrelation(int, double*, double*, int);

void compressAllJsonFiles();
void compressJsonFile(std::string);

class CsvHandler{
private:
	std::string filename;
public:
	CsvHandler(std::string);

	void addSampleDataLine(std::string);

	void createFile();
	void removeFile();

	void setFilename(std::string);
};


class EmgCsvHandler : public CsvHandler{
private:
	const std::string foldername = "emg/";
public:
	EmgCsvHandler(std::string);

	void addEmgSampleDataLine(std::array<int8_t, 8>);
};

class DataHandler{
private:
	std::string filename;
	double** emgArrays;
	double** gyrArrays;
	double** accArrays;
	double** oriArrays;
public:
	DataHandler(std::string);

	void generateDataArrays();

	double** getEmgArrays();
	double** getGyrArrays();
	double** getAccArrays();
	double** getOriArrays();
	void printEmg();
};

#endif
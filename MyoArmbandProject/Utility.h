#ifndef UtilityH
#define UtilityH

#include <string>
#include <array>
#include <myo/myo.hpp>

volatile static bool isProgramRunning = true;

static int counter = 20; //frequency counter
static time_t start;

static const std::string RECORD_PRESTART_MESSEGE = "Press <Enter> to start recording...";


// List of sensors
enum Sensor{ EMG, ACC, ORI, GYR };
static const int NUMBER_OF_SENSORS = 4;

static const int DATA_TIME_INTERVAL = 2; //Gesture interval for recording

// Sample rate for each sensor
static const int FREQUENCE_EMG = 200;
static const int FREQUENCE_ORI = 50;
static const int FREQUENCE_ACC = 50;
static const int FREQUENCE_GYR = 50;

// Data length for each sensor for each record based on time interval and frequence
static const int DATA_EMG_LENGTH = FREQUENCE_EMG * DATA_TIME_INTERVAL;
static const int DATA_ORI_LENGTH = FREQUENCE_ORI * DATA_TIME_INTERVAL;
static const int DATA_ACC_LENGTH = FREQUENCE_ACC * DATA_TIME_INTERVAL;
static const int DATA_GYR_LENGTH = FREQUENCE_GYR * DATA_TIME_INTERVAL;

static const int DATA_EMG_LENGTH_MARGIN = 0;//50;
static const int DATA_ACC_LENGTH_MARGIN = 0;//20;
static const int DATA_GYR_LENGTH_MARGIN = 0;//20;
static const int DATA_ORI_LENGTH_MARGIN = 0;//20;

//Number of arrays for each sensor 
static const int NUMBER_OF_EMG_ARRAYS = 8;
static const int NUMBER_OF_ACC_ARRAYS = 3;
static const int NUMBER_OF_GYR_ARRAYS = 3;
static const int NUMBER_OF_ORI_ARRAYS = 4;


// List of pre-sampled gestures
enum Gesture{ EAT, HELP, NONE };
static const int NUMBER_OF_GESTURES = 2;

// List of pre-sample files
static std::string testFileList[] = {
		"eat01.json",
		"eat02.json",
		"eat03.json",
		"help01.json",
		"help02.json",
		"help03.json"
	};
static const int NUMBER_OF_TEST_PER_GESTURE = 3; // pre-samples per gesture
static const int testFileListSize = NUMBER_OF_GESTURES * NUMBER_OF_TEST_PER_GESTURE;

void clearScreen();
double crossCorrelation(int, double*, double*, int);

void compressAllJsonFiles();
void compressJsonFile(std::string);

std::string gestureToString(Gesture);

Gesture gestureComparisons(std::string);

void setDataLengt(int&, Sensor);
void setNumberOfArrays(int&, Sensor);
void setdataLengthMargin(int&, Sensor);

class DataHandler{
protected:
	double** emgArrays;
	double** gyrArrays;
	double** accArrays;
	double** oriArrays;
public:
	DataHandler();
	double** getArrays(Sensor);
};

class DataFileHandler : public DataHandler{
private:
	std::string filename;
public:
	DataFileHandler(std::string);
	void generateDataArrays();
};

class DataInputHandler : public DataHandler{
private:
public:
	DataInputHandler();
	void setSensorArray(int, int, double, Sensor);
	void reset();
};

Gesture gestureComparisons2(DataInputHandler);

#endif
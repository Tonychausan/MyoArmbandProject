#ifndef UtilityH
#define UtilityH

#include <string>
#include <array>
#include <myo/myo.hpp>

static bool isProgramRunning = true;

static int counter = 20; //frequency counter
static time_t start;

enum Gesture{ EAT, HELP, NONE };
static const int NUMBER_OF_GESTURES = 2;

enum Sensor{ EMG, ACC, ORI, GYR };
static const int NUMBER_OF_SENSORS = 4;

static const int DATA_TIME_INTERVAL = 2;
static const int DATA_EMG_LENGTH = 200 * DATA_TIME_INTERVAL;
static const int DATA_ORI_LENGTH = 50 * DATA_TIME_INTERVAL;;
static const int DATA_ACC_LENGTH = 50 * DATA_TIME_INTERVAL;;
static const int DATA_GYR_LENGTH = 50 * DATA_TIME_INTERVAL;;

static const int NUMBER_OF_EMG_ARRAYS = 8;
static const int NUMBER_OF_ACC_ARRAYS = 3;
static const int NUMBER_OF_GYR_ARRAYS = 3;
static const int NUMBER_OF_ORI_ARRAYS = 4;

static std::string testFileList[] = {
		"eat01.json",
		"eat02.json",
		"eat03.json",
		"help01.json",
		"help02.json",
		"help03.json"
	};
static const int NUMBER_OF_TEST_PER_GESTURE = 3;
static const int testFileListSize = NUMBER_OF_GESTURES * NUMBER_OF_TEST_PER_GESTURE;

void clearScreen();
double crossCorrelation(int, double*, double*, int);

void compressAllJsonFiles();
void compressJsonFile(std::string);

std::string gestureToString(Gesture);

Gesture gestureComparisons(std::string);

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

	double** getArrays(Sensor);
};



#endif
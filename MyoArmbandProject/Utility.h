#ifndef UtilityH
#define UtilityH

#include <string>
#include <array>
#include <myo/myo.hpp>
#include <json/json.h>

volatile static bool isProgramRunning = true;

static int counter = 20; //frequency counter
static time_t start;

static const std::string RECORD_PRESTART_MESSEGE = "Press <Enter> to start recording...";


// List of sensors
enum Sensor{ EMG, ACC, ORI, GYR, EMPTY_SENSOR};
static const int NUMBER_OF_SENSORS = 4;

/* ########################################################################### */
/* ########################################################################### */
// List of sensors to ignor in the gesture prediction
static Sensor sensorToIgnor[] = {ACC,GYR,ORI, EMPTY_SENSOR};
static const int sensorToIgnorLength = 0;

//DTW or cross-correlation
static const bool isDTWused = true;
/* ########################################################################### */
/* ########################################################################### */

static const int DATA_TIME_INTERVAL = 3; //Gesture interval for recording

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

//Number of arrays for each sensor 
static const int NUMBER_OF_EMG_ARRAYS = 8;
static const int NUMBER_OF_ACC_ARRAYS = 3;
static const int NUMBER_OF_GYR_ARRAYS = 3;
static const int NUMBER_OF_ORI_ARRAYS = 4;


// List of pre-sampled gestures
enum Gesture{ EAT, HELP, SLEEP, THANKYOU, WHY, NONE };
static const int NUMBER_OF_GESTURES = 5;

// List of pre-sample files
static std::string testFileList[] = {
		"eat01.json",
		"eat02.json",
		"eat03.json",
		"help01.json",
		"help02.json",
		"help03.json",
		"sleep01.json",
		"sleep02.json",
		"sleep03.json",
		"thankyou01.json",
		"thankyou02.json",
		"thankyou03.json",
		"why01.json",
		"why02.json",
		"why03.json"
	};
static const int NUMBER_OF_TEST_PER_GESTURE = 3; // pre-samples per gesture
static const int testFileListSize = NUMBER_OF_GESTURES * NUMBER_OF_TEST_PER_GESTURE;

static std::string preSampledRecordFileList[] = {
	"test-eat01.json",
	"test-eat02.json",
	"test-eat03.json",
	"test-eat04.json",
	"test-eat05.json",
	"test-eat06.json",
	"test-eat07.json",
	"test-eat08.json",
	"test-eat09.json",
	"test-eat10.json",
	"test-help01.json",
	"test-help02.json",
	"test-help03.json",
	"test-help04.json",
	"test-help05.json",
	"test-help06.json",
	"test-help07.json",
	"test-help08.json",
	"test-help09.json",
	"test-help10.json",
	"test-sleep01.json",
	"test-sleep02.json",
	"test-sleep03.json",
	"test-sleep04.json",
	"test-sleep05.json",
	"test-sleep06.json",
	"test-sleep07.json",
	"test-sleep08.json",
	"test-sleep09.json",
	"test-sleep10.json",
	"test-thankyou01.json",
	"test-thankyou02.json",
	"test-thankyou03.json",
	"test-thankyou04.json",
	"test-thankyou05.json",
	"test-thankyou06.json",
	"test-thankyou07.json",
	"test-thankyou08.json",
	"test-thankyou09.json",
	"test-thankyou10.json",
	"test-why01.json",
	"test-why02.json",
	"test-why03.json",
	"test-why04.json",
	"test-why05.json",
	"test-why06.json",
	"test-why07.json",
	"test-why08.json",
	"test-why09.json",
	"test-why10.json"
};
static Gesture preSampledRecordPredictionList[] = {
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY
};
static const int  preSampledRecordFileListSize = 50;

// Test success variables
static int numberOfTestsForGesture[NUMBER_OF_GESTURES];
static int correctTests[NUMBER_OF_GESTURES];

static const int MAX_R = NUMBER_OF_TEST_PER_GESTURE * sensorToIgnorLength;

void clearScreen();
double crossCorrelation(int, double*, double*, int);

double calculateEuclideanDistance(double x, double y);
double CalculateDynamicTimeWarpedDistance(double*, double*, int);

void compressAllJsonFiles();
void compressJsonFile(std::string);
std::string getCompressedFilename(std::string);

bool isThisSensorIgnored(Sensor);

std::string gestureToString(Gesture);
std::string sensorToString(Sensor);

Gesture gestureComparisonsJsonFile(std::string);

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
	void setSensorArrayValueAt(int, int, double, Sensor);
	void reset();
};

Gesture gestureComparisons(DataHandler);

#endif
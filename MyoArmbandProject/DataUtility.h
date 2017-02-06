#ifndef DataUtilityH
#define DataUtilityH

#include <string>

// List of sensors
enum Sensor{ EMG, ACC, ORI, GYR, EMPTY_SENSOR };
static std::string sensor_names[] = { "Emg", "Acc", "Ori", "Gyr" };

// List of pre-sampled gestures
enum Gesture{ EAT, HELP, SLEEP, THANKYOU, WHY, NONE };
static std::string gesture_names[] = { "EAT", "HELP", "SLEEP", "THANKYOU", "WHY" };
static const int NUMBER_OF_GESTURES = 5;

enum DataSetType{ TRAINING, TEST };
enum DataSetFormat{ COMPRESSED, RAW };

/*
*Function: gestureToString
* ----------------------------
*	gesture : gesture
*
*   returns : gesture name
*/
/*
*Function: sensorToString
* ----------------------------
*	sensor : sensor
*
*   returns : sensor name
*/
std::string gestureToString(Gesture gesture);
std::string sensorToString(Sensor sensor);

/*
*Function: isSensorIgnored
* ----------------------------
*   Check if given sensor is ignored by the system
*
*	sensor : sensor to check
*
*   returns : true if sensor is ignored, else false
*/
bool isSensorIgnored(Sensor sensor);

struct Filelist{
	std::string *files;
	int size;
};

static Filelist training_file_list;
static Filelist test_file_list;

/*
*Function: getDataSetPath
* ----------------------------
*   returns the data set path to the parameter path
*
*	path : return parameter, the path of the data set
*	format : Compressed or raw data set
*	type : Training or test data set
*
*/
std::string getDataSetPath(DataSetFormat format, DataSetType type);
void generateFilelist(Filelist *filelist, std::string path_string);


// List of pre-sample files
static std::string training_filename_list[] = {
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
static const int NUMBER_OF_TRANING_PER_GESTURE = 3; // pre-samples per gesture
static const int TRAINING_SIZE = NUMBER_OF_GESTURES * NUMBER_OF_TRANING_PER_GESTURE;

static Gesture training_definitives_list[] = {
	EAT, EAT, EAT,
	HELP, HELP, HELP,
	SLEEP, SLEEP, SLEEP,
	THANKYOU, THANKYOU, THANKYOU,
	WHY, WHY, WHY
};

static std::string test_filename_list[] = {
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
static Gesture test_definitives_list[] = {
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
static const int  NUMBER_OF_TESTS = 50;


#endif
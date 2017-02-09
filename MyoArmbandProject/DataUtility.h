#ifndef DataUtilityH
#define DataUtilityH

#include <string>

// List of sensors
enum Sensor{ EMG, ACC, ORI, GYR, EMPTY_SENSOR };
static std::string sensor_names[] = { "Emg", "Acc", "Ori", "Gyr" };

// List of gestures
enum Gesture{ EAT, HELP, SLEEP, THANKYOU, WHY, NONE_GESTURE };
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

class File{
public:
	std::string filename;
	Gesture answer;
};

class Filelist{
public:
	/*std::string *files;
	Gesture *answers;*/
	File *files;
	int size;
};

extern Filelist training_file_list;
extern Filelist test_file_list;

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

#endif
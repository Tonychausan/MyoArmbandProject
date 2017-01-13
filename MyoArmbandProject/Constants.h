#ifndef ConstantH
#define ConstantH

#include <string>;


static const int NUMBER_OF_SENSORS = 4;

// Sample rate for each sensor
static const int FREQUENCY_EMG = 200;
static const int FREQUENCY_ORI = 50;
static const int FREQUENCY_ACC = 50;
static const int FREQUENCY_GYR = 50;

static const int GESTURE_DATA_TIME_INTERVAL = 3; //Gesture interval for recording

// Data length for each sensor for each record based on time interval and frequence
static const int DATA_EMG_LENGTH = FREQUENCY_EMG * GESTURE_DATA_TIME_INTERVAL;
static const int DATA_ORI_LENGTH = FREQUENCY_ORI * GESTURE_DATA_TIME_INTERVAL;
static const int DATA_ACC_LENGTH = FREQUENCY_ACC * GESTURE_DATA_TIME_INTERVAL;
static const int DATA_GYR_LENGTH = FREQUENCY_GYR * GESTURE_DATA_TIME_INTERVAL;

//Number of arrays for each sensor 
static const int NUMBER_OF_EMG_ARRAYS = 8;
static const int NUMBER_OF_ACC_ARRAYS = 3;
static const int NUMBER_OF_GYR_ARRAYS = 3;
static const int NUMBER_OF_ORI_ARRAYS = 4;


static const std::string RECORD_PRESTART_MESSEGE = "Press <Enter> to start recording...";


#endif
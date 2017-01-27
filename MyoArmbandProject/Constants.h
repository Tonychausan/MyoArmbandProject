#ifndef ConstantH
#define ConstantH

#include <string>
#include "SettingsVariables.h"


static const int NUMBER_OF_SENSORS = 4;

// Sample rate for each sensor
static const int FREQUENCY_EMG = 200;
static const int FREQUENCY_ORI = 50;
static const int FREQUENCY_ACC = 50;
static const int FREQUENCY_GYR = 50;



// Data length for each sensor for each record based on time interval and frequence
static const int DATA_LENGTH_EMG = FREQUENCY_EMG * DATA_TIME_INTERVAL_GESTURE;
static const int DATA_LENGTH_ORI = FREQUENCY_ORI * DATA_TIME_INTERVAL_GESTURE;
static const int DATA_LENGTH_ACC = FREQUENCY_ACC * DATA_TIME_INTERVAL_GESTURE;
static const int DATA_LENGTH_GYR = FREQUENCY_GYR * DATA_TIME_INTERVAL_GESTURE;

//Number of arrays for each sensor 
static const int NUMBER_OF_EMG_ARRAYS = 8;
static const int NUMBER_OF_ACC_ARRAYS = 3;
static const int NUMBER_OF_GYR_ARRAYS = 3;
static const int NUMBER_OF_ORI_ARRAYS = 4;


static const std::string RECORD_PRESTART_MESSEGE = "Press <Enter> to start recording...";


#endif
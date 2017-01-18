#ifndef SettingsVariablesH
#define SettingsVariablesH

// List of sensors to ignor in the gesture prediction
static const bool isEmgSensorOn = false;
static const bool isAccSensorOn = true;
static const bool isGyrSensorOn = true;
static const bool isOriSensorOn = true;

//DTW or cross-correlation
static const bool isDtwUsed = false;

static const int DATA_TIME_INTERVAL_GESTURE = 3; //Gesture interval for recording in seconds

#endif
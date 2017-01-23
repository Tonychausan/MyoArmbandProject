#ifndef SettingsVariablesH
#define SettingsVariablesH

// List of sensors to ignor in the gesture prediction
static const bool isEmgSensorOn = true;
static const bool isAccSensorOn = false;
static const bool isGyrSensorOn = false;
static const bool isOriSensorOn = false;

//DTW or cross-correlation
static const bool isDtwUsed = false;

static const int DATA_TIME_INTERVAL_GESTURE = 3; //Gesture interval for recording in seconds

#endif
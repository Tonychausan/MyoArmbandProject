#include "stdafx.h"

#include <string>

#include "DataUtility.h"

std::string gestureToString(Gesture gesture){
	int i = static_cast<int>(gesture);
	return gesture_names[i];
}

std::string sensorToString(Sensor sensor){
	int i = static_cast<int>(sensor);
	return sensor_names[i];
}

#include "DataUtility.h"
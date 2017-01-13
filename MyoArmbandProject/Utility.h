#ifndef UtilityH
#define UtilityH

#include <string>
#include <array>
#include <myo/myo.hpp>

#include "DataUtility.h"
#include "DataHandlers.h"
#include "SettingsVariables.h"

volatile static bool isProgramRunning = true;

static int counter = 20; //frequency counter
static time_t start;

// Test success variables
static int number_of_tests_for_gesture[NUMBER_OF_GESTURES];
static int number_of_correct_recognition[NUMBER_OF_GESTURES];

void clearScreen();
double crossCorrelation(int, double*, double*, int);
double calculateDynamicTimeWarpedDistance(double*, double*, int);

void compressAllFiles();
void compressFile(std::string);
std::string getCompressedFilename(std::string);

bool isSensorIgnored(Sensor);

std::string gestureToString(Gesture);
std::string sensorToString(Sensor);

Gesture gestureComparisonsJsonFile(std::string);

void setDataLengt(int&, Sensor);
void setNumberOfArrays(int&, Sensor);

Gesture gestureComparisons(DataHandler);

std::string getJsonArrayNameBySensor(Sensor sensor);

#endif
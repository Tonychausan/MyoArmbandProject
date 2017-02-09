#ifndef UtilityH
#define UtilityH

#include <string>
#include <array>

#include <myo/myo.hpp>
#include <fftw3.h>

#include "DataUtility.h"
#include "DataHandlers.h"
#include "SettingsVariables.h"

volatile static bool isProgramRunning = true;

static time_t start;

// Test success variables
static int number_of_tests[NUMBER_OF_GESTURES];
static int number_of_correct_recognition[NUMBER_OF_GESTURES];


/*
* Function: clearScreen
* ----------------------------
*   Clear the terminal window on Windows
*/
void clearScreen();


/*
* Function: crossCorrelation
* ----------------------------
*   Returns the square of the largest of its two input values
*	http://paulbourke.net/miscellaneous/correlate/
*
*	maxdelay: the maxium delay for comparision
*   input_array: input array
*   test_array: test array from training set
*	size_of_array: the data size
*
*   returns: the correlation of input_array and test_array using cross correlation
*/
double crossCorrelation(int maxdelay, double* input_array, double* test_array, int size_of_array);


/*
* Function: calculateDynamicTimeWarpedDistance
* ----------------------------
*   Returns the square of the largest of its two input values
*
*	maxdelay: the maxium delay for comparision
*   input_array: input array
*   test_array: test array from training set
*	size_of_array: the data size
*
*   returns: the distance of input_array and test_array using DTW
*/
double calculateDynamicTimeWarpedDistance(double* input_array, double* test_array, int size_of_array);

/*
* Function: compressAllFiles
* ----------------------------
*   Compress all the raw json files from pwter 
*/
/*
* Function: compressFile
* ----------------------------
*   Compress json-file filename
*
*	filename: filename of input file
*/
void compressAllFiles();
void compressFile(File file, bool isTrainingSet);

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

fftw_complex *fourierTransform(double *x, int n);

Gesture gestureComparisonsJsonFile(std::string testfilename);

void setDataLengt(int& data_length, Sensor sensor);
void setNumberOfArrays(int& number_of_arrays, Sensor sensor);

Gesture gestureComparisons(DataHandler);

std::string getJsonArrayNameBySensor(Sensor sensor);



#endif
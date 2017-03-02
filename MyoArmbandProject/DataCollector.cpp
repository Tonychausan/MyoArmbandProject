#include "stdafx.h"

// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <array>
#include <sstream>
#include <string>
#include <time.h>

#include "Utility.h"
#include "DataCollector.h"
#include "DataUtility.h"
#include "Constants.h"

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

clock_t begin_time;

DataCollector::DataCollector()
	: onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0)
	{
		isRecording = false;

		isEmgRecording = false;
		isGyrRecording = false;
		isOriRecording = false;
		isAccRecording = false;

		emg_sampling_counter = 0;
		gyr_sampling_counter = 0;
		acc_sampling_counter = 0;
		ori_sampling_counter = 0;
	}

// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
void DataCollector::onUnpair(myo::Myo* myo, uint64_t timestamp)
{
	// We've lost a Myo.
	// Let's clean up some leftover state.
	roll_w = 0;
	pitch_w = 0;
	yaw_w = 0;
	onArm = false;
	isUnlocked = false;
	emg_data.fill(0);
}

// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
// as a unit quaternion.
void DataCollector::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
{
	using std::atan2;
	using std::asin;
	using std::sqrt;
	using std::max;
	using std::min;

	// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
	float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
		1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
	float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
	float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
		1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

	// Convert the floating point angles in radians to a scale from 0 to 18.
	roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
	pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
	yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);

	// Recorder for Orientation Data
	recorder(ORI, quat);

}

void DataCollector::onAccelerometerData(myo::Myo* 	myo, uint64_t 	timestamp, const myo::Vector3<float> &accel){
	acc_data = accel;

	// Recorder for Accelerometer Data
	recorder(ACC, accel);
}

void DataCollector::onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3< float > & gyro)
{
	gyr_data = gyro;

	// Recorder for Gyroscope Data
	recorder(GYR, gyro);
}

// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
void DataCollector::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
	for (int i = 0; i < NUMBER_OF_EMG_ARRAYS; i++) {
		emg_data[i] = emg[i];
	}

	// Recorder for EMG Data
	recorder(EMG, emg);

	// Recording indicator
	if (isRecording && emg_sampling_counter % (DATA_LENGTH_EMG/10) == 0)
	{
		std::cout << ".";
	}

	// Turn off recording if all the sensors are finished, using highest frequency sensor
	if (isRecording && !isRecordingFinished()){
		std::cout << std::endl;
		gestureRecordOff();
	}
}

// onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
// arm. This lets Myo know which arm it's on and which way it's facing.
void DataCollector::onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation, myo::WarmupState warmupState)
{
	onArm = true;
	whichArm = arm;
}

// onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
// when Myo is moved around on the arm.
void DataCollector::onArmUnsync(myo::Myo* myo, uint64_t timestamp)
{
	onArm = false;
}

// onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
void DataCollector::onUnlock(myo::Myo* myo, uint64_t timestamp)
{
	isUnlocked = true;
}

// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
void DataCollector::onLock(myo::Myo* myo, uint64_t timestamp)
{
	isUnlocked = false;
}

void DataCollector::printStatus()
{
	if (onArm){
		std::cout << "Status:\t" << (isUnlocked ? "Unlocked" : "Locked") << std::endl;
		std::cout << "Arm:\t" << (whichArm == myo::armLeft ? "Left" : "Right") << std::endl;
	}
	else {
		std::cout << "Status:\t" << "Not on Arm" << std::endl;
		std::cout << "Arm:\t" << "???" << std::endl;
	}
	std::cout << std::endl;
}

// We define this function to print the current values that were updated by the on...() functions above.
void DataCollector::printEMG()
{
	// Print out the EMG data.
	std::cout << "EMG:\t\t";
	for (size_t i = 0; i < emg_data.size(); i++) {
		std::ostringstream oss;
		oss << static_cast<int>(emg_data[i]);
		std::string emgString = oss.str();
		std::cout << '[' << emgString << std::string(4 - emgString.size(), ' ') << ']';
	}
	std::cout << std::endl;
	std::cout << std::flush;
}

void printVector3(myo::Vector3<float> vector){
	int const sizeOfVector = 3;
	for (size_t i = 0; i < sizeOfVector; i++) {
		std::ostringstream oss;
		oss << static_cast<float>(vector[i]);
		std::string outString = oss.str();
		std::cout << '[' << outString << std::string(15 - outString.size(), ' ') << ']';
	}
	std::cout << std::endl;
}

void DataCollector::printGyro()
{
	std::cout << "Gyroscop:\t";
	printVector3(gyr_data);
	std::cout << std::flush;
}

void DataCollector::printOrientation(){
	std::cout << "Orientation:\t";

	int const number_of_orientations = 3;
	float values[number_of_orientations] = { roll_w, pitch_w, yaw_w };
	for (size_t i = 0; i < number_of_orientations; i++) {
		std::ostringstream oss;
		oss << static_cast<float>(values[i]);
		std::string out_string = oss.str();
		std::cout << '[' << out_string << std::string(3 - out_string.size(), ' ') << ']';
	}
	std::cout << "\t\t(roll, pitch, yaw)";
	std::cout << std::endl;
}

void DataCollector::printAccelerometer()
{
	std::cout << "Accelerometer:\t";
	printVector3(acc_data);
	std::cout << std::flush;
}

void DataCollector::gestureRecordOn(){
	isRecording = true;

	isEmgRecording  = true;
	isGyrRecording = true;
	isOriRecording = true;
	isAccRecording = true;


	emg_sampling_counter = 0;
	gyr_sampling_counter = 0;
	acc_sampling_counter = 0;
	ori_sampling_counter = 0;

	begin_time = clock();

	std::cout << "Recording gesture..." << std::endl;
}

void DataCollector::gestureRecordOff(){
	std::cout << "gesture recording finished..." << std::endl << std::endl;

	begin_time = clock();
	Gesture prediction = gestureComparisons(input_gesture);

	std::cout << "Recognize: " << gestureToString(prediction) << std::endl;
	std::cout << "Analyse time: " << float(clock() - begin_time) / CLOCKS_PER_SEC << std::endl;

	std::cout << std::endl << RECORD_PRESTART_MESSEGE << std::endl;

	isRecording  = false;
}

void DataCollector::recorder(Sensor sensor, myo::Quaternion<float> quat){
	bool *isRecording = &isOriRecording;
	int *counter = &ori_sampling_counter;

	int data_length;
	int number_of_arrays;

	setDataLengt(data_length, sensor);
	setNumberOfArrays(number_of_arrays, sensor);

	if (*isRecording && *counter < data_length){
		for (int i = 0; i < number_of_arrays; i++) {
			double* ori = new double[NUMBER_OF_ORI_ARRAYS];
			ori[0] = quat.x();
			ori[1] = quat.y();
			ori[2] = quat.z();
			ori[3] = quat.w();
			input_gesture.setSensorDataValueAt(*counter, i, (double)ori[i], sensor);
		}
		*counter += 1;
	}
	else if (*isRecording){
		*isRecording = false;
	}
}

template <typename DataArray>
void DataCollector::recorder(Sensor sensor, DataArray array){
	bool *isRecording = NULL;
	int *counter = NULL;
	switch (sensor){
	case EMG:
		isRecording = &isEmgRecording;
		counter = &emg_sampling_counter;
		break;
	case ACC:
		isRecording = &isAccRecording;
		counter = &acc_sampling_counter;
		break;
	case GYR:
		isRecording = &isGyrRecording;
		counter = &gyr_sampling_counter;
		break;
	}

	int data_length, number_of_arrays;

	setDataLengt(data_length, sensor);
	setNumberOfArrays(number_of_arrays, sensor);

	if (*isRecording && *counter < data_length){
		for (int i = 0; i < number_of_arrays; i++) {
			if (sensor == EMG){
				input_gesture.setSensorDataValueAt(*counter, i, (int)array[i], sensor);
			}
			else
			{
				input_gesture.setSensorDataValueAt(*counter, i, (double)array[i], sensor);
			}
		}
		*counter += 1;
	}
	else if (*isRecording){
		*isRecording = false;
	}
}

// Check if all sensor have finished
bool DataCollector::isRecordingFinished(){
	return isEmgRecording || isAccRecording || isOriRecording || isGyrRecording;
}

void DataCollector::setInputGestureSensorArrayValueAt(int position, int array_id, double value, Sensor sensor){
	input_gesture.setSensorDataValueAt(position, array_id, value,sensor);
}



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

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

DataCollector::DataCollector()
	: onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose(), emgSamples(), gestureEmgCounter(0), isEmgRecording (0)
	{
		isGyrRecording = false;
		isOriRecording = false;
		isAccRecording = false;

		gestureGyrCounter = 0;
		gestureAccCounter = 0;
		gestureOriCounter = 0;
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
	emgSamples.fill(0);
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


	if (isOriRecording && gestureOriCounter < DATA_ORI_LENGTH){
		double* ori = new double[NUMBER_OF_ORI_ARRAYS];
		ori[0] = quat.x();
		ori[1] = quat.y();
		ori[2] = quat.z();
		ori[3] = quat.w();

		for (int i = 0; i < NUMBER_OF_ORI_ARRAYS; i++) {
			inputGesture.setSensorArray(gestureOriCounter, i, (double) ori[i], ORI);
		}
		//std::cout << gestureEmgCounter << std::endl;
		gestureOriCounter += 1;
	}
	else if (isOriRecording){
		isOriRecording = false;
	}

}

void DataCollector::onAccelerometerData(myo::Myo* 	myo, uint64_t 	timestamp, const myo::Vector3<float> &accel){
	accelerometerData = accel;


	if (isAccRecording && gestureAccCounter < DATA_ACC_LENGTH){
		for (int i = 0; i < NUMBER_OF_ACC_ARRAYS; i++) {
			inputGesture.setSensorArray(gestureAccCounter, i, (double) accel[i], ACC);
		}
		//std::cout << gestureAccCounter << std::endl;
		gestureAccCounter += 1;
	}
	else if (isAccRecording){
		isAccRecording = false;
	}
}

void DataCollector::onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3< float > & gyro)
{
	gyroData = gyro;

	if (isGyrRecording && gestureGyrCounter < DATA_GYR_LENGTH){
		for (int i = 0; i < NUMBER_OF_GYR_ARRAYS; i++) {
			inputGesture.setSensorArray(gestureGyrCounter, i, (double) gyro[i], GYR);
		}
		//std::cout << gestureEmgCounter << std::endl;
		gestureGyrCounter += 1;
	}
	else if (isGyrRecording){
		isGyrRecording = false;
	}
}

// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
void DataCollector::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
	for (int i = 0; i < NUMBER_OF_EMG_ARRAYS; i++) {
		emgSamples[i] = emg[i];
	}

	if (isEmgRecording && gestureEmgCounter < DATA_EMG_LENGTH){
		for (int i = 0; i < NUMBER_OF_EMG_ARRAYS; i++) {
			inputGesture.setSensorArray(gestureEmgCounter, i, (int) emg[i], EMG);
		}
		//std::cout << gestureEmgCounter << std::endl;
		gestureEmgCounter += 1;
	}
	else if (isEmgRecording ){
		isEmgRecording = false;
	}

	if (!isEmgRecording && !isAccRecording && !isGyrRecording && !isOriRecording){
		gestureRecordOff();
	}
}

// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
// making a fist, or not making a fist anymore.
void DataCollector::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
{
	currentPose = pose;

	if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
		// Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
		// Myo becoming locked.
		myo->unlock(myo::Myo::unlockHold);

		// Notify the Myo that the pose has resulted in an action, in this case changing
		// the text on the screen. The Myo will vibrate.
		myo->notifyUserAction();
	}
	else {
		// Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
		// are being performed, but lock after inactivity.
		myo->unlock(myo::Myo::unlockTimed);
	}
}

// onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
// arm. This lets Myo know which arm it's on and which way it's facing.
void DataCollector::onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
	myo::WarmupState warmupState)
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

// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
// For this example, the functions overridden above are sufficient.

// We define this function to print the current values that were updated by the on...() functions above.
void DataCollector::print()
{
	// Clear the current line
	std::cout << '\r';

	// Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
	//std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
	//<< '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
	//<< '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';
	std::cout << '[' << roll_w << ']'
		<< '[' << pitch_w << ']'
		<< '[' << yaw_w << ']';

	if (onArm) {
		// Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.

		// Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
		// output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
		// that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
		std::string poseString = currentPose.toString();

		std::cout << '[' << (isUnlocked ? "unlocked" : "locked  ") << ']'
			<< '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
			<< '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
	}
	else {
		// Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
		std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
	}

	std::cout << std::flush;
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
	for (size_t i = 0; i < emgSamples.size(); i++) {
		std::ostringstream oss;
		oss << static_cast<int>(emgSamples[i]);
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
	printVector3(gyroData);
	std::cout << std::flush;
}

void DataCollector::printOrientation(){
	std::cout << "Orientation:\t";

	int const nrOfOrientations = 3;
	float values[nrOfOrientations] = { roll_w, pitch_w, yaw_w };
	for (size_t i = 0; i < nrOfOrientations; i++) {
		std::ostringstream oss;
		oss << static_cast<float>(values[i]);
		std::string outString = oss.str();
		std::cout << '[' << outString << std::string(3 - outString.size(), ' ') << ']';
	}
	std::cout << "\t\t(roll, pitch, yaw)";
	std::cout << std::endl;
}

void DataCollector::printAccelerometer()
{
	std::cout << "Accelerometer:\t";
	printVector3(accelerometerData);
	std::cout << std::flush;
}

void DataCollector::gestureRecordOn(){
	isEmgRecording = true;
	isGyrRecording = true;
	isOriRecording = true;
	isAccRecording = true;

	gestureEmgCounter = 0;
	gestureGyrCounter = 0;
	gestureAccCounter = 0;
	gestureOriCounter = 0;

	std::cout << "Recording gesture..." << std::endl;
}

void DataCollector::gestureRecordOff(){

	std::cout << "gesture recording finished..." << std::endl << std::endl;
	std::cout << gestureToString(gestureComparisons2(inputGesture)) << std::endl;

	isEmgRecording = false;
}

bool DataCollector::isRecording(){
	return isEmgRecording ;
}

void DataCollector::setInputGestureAt(int i, int j, double value, Sensor sensor){
	inputGesture.setSensorArray(i,j,value,sensor);
}



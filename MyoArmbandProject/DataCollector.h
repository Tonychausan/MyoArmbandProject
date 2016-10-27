#ifndef DataCollectorH
#define DataCollectorH

#include <myo/myo.hpp>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <array>
#include <sstream>
#include <string>

#include "Utility.h"

class DataCollector : public myo::DeviceListener {
private:
	// These values are set by onArmSync() and onArmUnsync().
	bool onArm;
	myo::Arm whichArm;

	// This is set by onUnlocked() and onLocked().
	bool isUnlocked;

	// These values are set by onOrientationData() and onPose().
	int roll_w, pitch_w, yaw_w;
	myo::Pose currentPose;

	// The values of this array is set by onEmgData().
	std::array<int8_t, 8> emgSamples;

	// The values of this array is set by ononAccelerometerData().
	myo::Vector3<float> accelerometerData;

	// The values of this array is set by onGyroscopeData().
	myo::Vector3<float> gyroData;


	bool isEmgRecording ;
	bool isOriRecording;
	bool isGyrRecording;
	bool isAccRecording;

	// Counter for number of measurment for each gesture
	int gestureEmgCounter;
	int gestureGyrCounter;
	int gestureAccCounter;
	int gestureOriCounter;


	int inputgestureEmgCounter;
	DataInputHandler inputGesture;



public:
	DataCollector();

	void onUnpair(myo::Myo* myo, uint64_t timestamp);

	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);
	void onAccelerometerData(myo::Myo* 	myo, uint64_t 	timestamp, const myo::Vector3<float> &accel);
	void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3< float > & gyro);
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg);
	
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose);
	
	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation, myo::WarmupState warmupState);
	void onArmUnsync(myo::Myo* myo, uint64_t timestamp);
	
	void onUnlock(myo::Myo* myo, uint64_t timestamp);
	void onLock(myo::Myo* myo, uint64_t timestamp);
	
	// Print functions for different Data Samples
	void print();
	void printStatus();
	void printEMG();
	void printGyro();
	void printOrientation();
	void printAccelerometer();

	void gestureRecordOn();
	void gestureRecordOff();
	bool isRecording();
	void setInputGestureAt(int, int, double, Sensor);

};

void printVector3(myo::Vector3<float> vector);

#endif
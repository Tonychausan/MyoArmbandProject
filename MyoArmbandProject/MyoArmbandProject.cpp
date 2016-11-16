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
#include <fstream>
#include <thread>
#include <windows.h>

#include "Utility.h"
#include "DataCollector.h"

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

#include <json/json.h>

volatile bool isRecording = false;
volatile bool dummy = false;

void keyboardInterruptDetector()
{
	while (isProgramRunning)
	{
		if (!isRecording)
		{
			// prevent first time auto-start bug
			if (!dummy)
			{
				std::cin.ignore();
				dummy = true;
			}
			std::cin.ignore();
			isRecording = true;
		}
	}
	
}

void testFile(std::string filename, Gesture gesture){
	Gesture prediction = gestureComparisonsJsonFile(filename);
	std::cout << "Test file: " << filename << std::endl;
	std::cout << "Gesture: " << gestureToString(gesture) << std::endl;
	std::cout << "Prediction: " << gestureToString(prediction) << std::endl << std::endl;
}

int main(int argc, char** argv)
{
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	// Action menu
	int action = 0;


	try {
		// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
		// publishing your application. The Hub provides access to one or more Myos.
		myo::Hub hub("com.example.hello-myo");

		std::cout << "Attempting to find a Myo..." << std::endl;

		// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
		// immediately.
		// waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
		// if that fails, the function will return a null pointer.
		myo::Myo* myo = hub.waitForMyo(10000);

		// If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
			std::cin.ignore();
		}

		// We've found a Myo.
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

		// Next we enable EMG streaming on the found Myo.
		myo->setStreamEmg(myo::Myo::streamEmgEnabled);

		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
		DataCollector collector;

		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub.addListener(&collector);
			
		// Finally we enter our main loop.
		isProgramRunning = false;

		while (true){
			clearScreen();
			std::cout << "Choose an action" << std::endl;
			std::cout << "1) Try gestures" << std::endl;
			std::cout << "2) Measurment display" << std::endl;
			std::cout << "3) Compress files" << std::endl;
			std::cout << "4) Pre-data gesture comparison" << std::endl;
			std::cin >> action;

			// Compress files
			if (action == 3){
				compressAllJsonFiles();
				std::cin.ignore();
				clearScreen();
			}
			// Print Measurment
			else if (action == 2){
				isProgramRunning = true;
				while (isProgramRunning) {
					// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
					// In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
					hub.run(1000 / 5);

					clearScreen();

					collector.printStatus();
					collector.printEMG();
					collector.printAccelerometer();
					collector.printGyro();
					collector.printOrientation();
				}
			}
			// Test gestures with pre-sampled file tests
			else if (action == 4){

				testFile("compressed-test-sleep01.json", SLEEP);
				testFile("compressed-test-thankyou01.json", THANKYOU);

				system("PAUSE");
			}
			// Try some gesture recording
			else {
				isProgramRunning = true;
				std::cout << RECORD_PRESTART_MESSEGE << std::endl;
				std::thread keyboardInterrupt(keyboardInterruptDetector);


				while (isProgramRunning){
					if (isRecording){
						collector.gestureRecordOn();
						isRecording = false;
					}
					hub.run(1000 / 5);
				}
				keyboardInterrupt.join();
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}

	system("PAUSE");
	return 0;
}

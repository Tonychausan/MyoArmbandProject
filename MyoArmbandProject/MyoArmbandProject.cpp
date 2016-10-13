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

#include "Utility.h"
#include "DataCollector.h"

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>



int main(int argc, char** argv)
{
	// We catch any exceptions that might occur below -- see the catch statement for more details.
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

		//EmgCsvHandler testCSV("test.csv");
		//testCSV.createFile();
		//testCSV.addSampleDataLine("100,100,100");
			
		// Finally we enter our main loop.

		// Variable
		int isRecording = 0;

		// Action menu
		std::cout << "Choose an action" << std::endl;
		std::cout << "1) Record new gesture" << std::endl;
		std::cout << "2) Try gestures" << std::endl;
		std::cin >> isRecording;

		// if recording new gesture
		if (isRecording == 1){
			std::string nameOfGesture;
			std::cout << "Type the name of the new gesture: ";
			std::cin >> nameOfGesture;

			collector.createNewGestureOn(true);
		}



		while (isProgramRunning) {
			// In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
			// In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
			hub.run(1000 / 5);
			// After processing events, we call the print() member function we defined above to print out the values we've
			// obtained from any events that have occurred.
			/*ClearScreen();

			//collector.print();
			collector.printStatus();
			collector.printEMG();
			collector.printAccelerometer();
			collector.printGyro();
			collector.printOrientation();*/
		}

		// If a standard exception occurred, we print out its message and exit.
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

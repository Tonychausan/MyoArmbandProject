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
#include "MenuActions.h"
#include "Constants.h"
#include "FannTest.h"
#include "DataUtility.h"

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>
#include <json/json.h>

//volatile bool isRecording = false;
//volatile bool dummy = false;

Filelist training_file_list;
Filelist test_file_list;

bool isMyoFound = false;

DataCollector collector;

void findMyo(myo::Hub &hub){
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
	isMyoFound = true;

	// Next we enable EMG streaming on the found Myo.
	myo->setStreamEmg(myo::Myo::streamEmgEnabled);

	// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
	// Hub::run() to send events to all registered device listeners.
	hub.addListener(&collector);
}

void printMenu(){
	clearScreen();
	std::cout << "Choose an action" << std::endl;
	std::cout << "1) Try gestures" << std::endl;
	std::cout << "2) Measurment display" << std::endl;
	std::cout << "3) Compress files" << std::endl;
	std::cout << "4) Pre-data gesture comparison" << std::endl;
	std::cout << "5) Test Neural Network" << std::endl;
}

bool isMyoDependentActivity(int action){
	switch (action) {
	case 3:
	case 5:
	case 4: return false;
	default:
		return true;
	}
}

void menu(int& action, bool print, myo::Hub *hub = NULL){
	if (!print)
		return;


	action = 0;
	printMenu();
	std::cin >> action;

	if (action == 3){
		compressFiles();
	}
	else if (action == 4){
		runPreSampledDataTests();
	}
	else if (action == 5){
		// do some neural network shit
		/*while (true){
			int hello;
			std::cout << "(1)Build Training file\n(2)Train from file\n(3)Test\n";
			std::cin >> hello;

			if (hello == 1)
				buildTrainingFile();
			else if (hello == 2)
				emgTrainNN();
			else{
				for (int i = 0; i < NUMBER_OF_TESTS; i++)
				{
					std::cout << test_filename_list[i] << std::endl;
					emgTestNN(test_filename_list[i]);
					std::cout << std::endl;
				}
			}
		}*/
		
		generateFilelist(&training_file_list, getDataSetPath(RAW, TEST));
		
		for (int i = 0; i < training_file_list.size; i++)
		{
			std::cout << training_file_list.files[i] << std::endl;
		}
	}
	else if (hub != NULL){ //Myo dependent activity
		
		if (action == 2){
			liveDataPrint(collector, *hub);
		}
		else {
			liveGestureRecognition(collector, *hub);
		}
	}
}


/*void keyboardInterruptDetector()
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
	
}*/

void initTests(){
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		number_of_correct_recognition[i] = 0;
		number_of_tests[i] = 0;
	}
}

int main(int argc, char** argv)
{
	generateFilelist(&training_file_list, getDataSetPath(COMPRESSED, TRAINING));
	generateFilelist(&test_file_list, getDataSetPath(COMPRESSED, TEST));

	int action; // action variable for menu

START_MENU:
	menu(action, true);

	if (!isMyoDependentActivity(action))
		//menu(action, false);
		int a = 3;
	else{
		try {
			// First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
			// publishing your application. The Hub provides access to one or more Myos.
			myo::Hub hub("com.example.hello-myo");
			if (!isMyoFound){
				findMyo(hub);
			}

			// Finally we enter our main loop.
			isProgramRunning = false;
			bool printMenu = false;
			while (true){
				menu(action, printMenu, &hub);
				printMenu = true;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			system("PAUSE");
			goto START_MENU;
		}
	}

	system("PAUSE");
	return 0;
}

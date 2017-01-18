#include "stdafx.h"

#include <thread>

#include "MenuActions.h"
#include "DataCollector.h"
#include "Utility.h"
#include "Constants.h"

bool dummy = false;
bool isRecording = false;

void testFile(std::string filename, Gesture gesture){
	std::cout << "##############################" << std::endl << "Test file: " << filename << std::endl;

	Gesture prediction = gestureComparisonsJsonFile(getCompressedFilename(filename));
	std::cout << std::endl << "Gesture: " << gestureToString(gesture) << std::endl;
	std::cout << "Prediction: " << gestureToString(prediction) << std::endl << std::endl;

	number_of_tests[gesture] += 1;
	if (prediction == gesture)
	{
		number_of_correct_recognition[gesture] += 1;
	}
}

void runPreSampledDataTests(){
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		number_of_correct_recognition[i] = 0;
		number_of_tests[i] = 0;
	}

	for (int i = 0; i < NUMBER_OF_TESTS; i++)
	{
		testFile(test_filename_list[i], test_definitives_list[i]);
	}

	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		std::cout << gestureToString(static_cast<Gesture>(i)) << ": " << number_of_correct_recognition[i] << " of " << number_of_tests[i] << std::endl;
	}

	system("PAUSE");
}

void compressFiles(){
	compressAllFiles();
	std::cin.ignore();
	clearScreen();
}

void liveDataPrint(DataCollector &collector, myo::Hub &hub){
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

void liveGestureRecognition(DataCollector &collector, myo::Hub &hub){
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
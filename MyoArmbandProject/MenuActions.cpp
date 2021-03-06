#include "stdafx.h"

#include <thread>

#include "MenuActions.h"
#include "DataCollector.h"
#include "Utility.h"
#include "Constants.h"
#include "FannTest.h"

bool dummy = false;
bool isRecording = false;

void testFile(File file){
	std::cout << "##############################" << std::endl << "Test file: " << file.filename << std::endl;

	Gesture prediction = gestureComparisonsJsonFile(file);
	std::cout << std::endl << "Gesture: " << gestureToString(file.gesture) << std::endl;
	std::cout << "Prediction: " << gestureToString(prediction) << std::endl << std::endl;

	number_of_tests[file.gesture] += 1;
	if (prediction == file.gesture)
	{
		number_of_correct_recognition[file.gesture] += 1;
	}
}

void runPreSampledDataTests(){
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		number_of_correct_recognition[i] = 0;
		number_of_tests[i] = 0;
	}

	for (int i = 0; i < test_file_list.size; i++)
	{
		testFile(test_file_list.files[i]);
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

void neuralNetworkTest(){
	int hello;
	std::cout << "(1)Build Training file\n(2)Train from file\n(3)Test\n(4)Remove\n";
	std::cin >> hello;

	if (hello == 1)
		buildEmgNeuralNetworkTrainingFile();
	else if (hello == 2)
		emgTrainNN();
	else if (hello == 3){
		for (int i = 0; i < test_file_list.size; i++)
		{
			emgTestNN(test_file_list.files[i]);
		}
	}
	else{
		emgNNfileRemover();
	}
}

void liveDataPrint(DataCollector &collector, myo::Hub &hub){
	isProgramRunning = true;
	while (isProgramRunning) {
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
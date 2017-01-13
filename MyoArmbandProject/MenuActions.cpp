#include "stdafx.h"

#include "MenuActions.h"
#include "DataCollector.h"
#include "Utility.h"

void testFile(std::string filename, Gesture gesture){
	std::cout << "##############################" << std::endl << "Test file: " << filename << std::endl;

	Gesture prediction = gestureComparisonsJsonFile(getCompressedFilename(filename));
	std::cout << std::endl << "Gesture: " << gestureToString(gesture) << std::endl;
	std::cout << "Prediction: " << gestureToString(prediction) << std::endl << std::endl;

	numberOfTestsForGesture[gesture] += 1;
	if (prediction == gesture)
	{
		correctTests[gesture] += 1;
	}
}

void preDataTest(){
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		correctTests[i] = 0;
		numberOfTestsForGesture[i] = 0;
	}

	for (int i = 0; i < NUMBER_OF_TESTS; i++)
	{
		testFile(preSampledRecordFileList[i], preSampledRecordPredictionList[i]);
	}

	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		std::cout << gestureToString(static_cast<Gesture>(i)) << ": " << correctTests[i] << " of " << numberOfTestsForGesture[i] << std::endl;
	}

	system("PAUSE");
}

void compressFiles(){
	compressAllJsonFiles();
	std::cin.ignore();
	clearScreen();
}
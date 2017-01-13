#include "stdafx.h"

#include "MenuActions.h"
#include "DataCollector.h"
#include "Utility.h"

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

void preDataTest(){
	for (int i = 0; i < NUMBER_OF_GESTURES; i++)
	{
		number_of_correct_recognition[i] = 0;
		number_of_tests[i] = 0;
	}

	for (int i = 0; i < NUMBER_OF_TESTS; i++)
	{
		testFile(presampled_test_filename_list[i], presampled_definitives_list[i]);
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
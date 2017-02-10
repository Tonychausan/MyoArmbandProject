#ifndef FannTestH
#define FannTestH

#include <string>

#include "DataUtility.h" 

void trainNN();
void testNN();

void buildTrainingFile();
void buildFourierTransformedTrainingFile();

void emgNNfileRemover();

void emgTestNN(File);
void emgFourierTransformedTestNN(File);

void emgTrainNN();


#endif
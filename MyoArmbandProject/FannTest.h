#ifndef FannTestH
#define FannTestH

#include <string>

#include "DataUtility.h" 

void trainNN();
void testNN();

void buildTrainingFile();

void emgNNfileRemover();
void emgTestNN(File);
void emgTrainNN();


#endif
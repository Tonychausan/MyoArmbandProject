#ifndef FannTestH
#define FannTestH

#include <string>

#include "DataUtility.h" 

void emgNNfileRemover();

void buildEmgNeuralNetworkTrainingFile();
void emgTrainNN();
void emgTestNN(File);



#endif
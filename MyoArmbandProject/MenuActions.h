#ifndef MenuActionsH
#define MenuActionsH

#include <string>

#include <myo/myo.hpp>

#include "Utility.h"
#include "DataCollector.h"


void testFile(std::string filename, Gesture gesture);

void runPreSampledDataTests();

void compressFiles();

void liveDataPrint(DataCollector &collector, myo::Hub &hub);

#endif
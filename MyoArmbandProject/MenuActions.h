#ifndef MenuActionsH
#define MenuActionsH

#include <string>

#include <myo/myo.hpp>

#include "Utility.h"
#include "DataCollector.h"


void testFile(File);

void runPreSampledDataTests();

void compressFiles();

void liveDataPrint(DataCollector &collector, myo::Hub &hub);

void liveGestureRecognition(DataCollector &collector, myo::Hub &hub);

#endif
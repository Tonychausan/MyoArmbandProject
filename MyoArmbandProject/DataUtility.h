#ifndef DataUtilityH
#define DataUtilityH

#include <string>

// List of sensors
enum Sensor{ EMG, ACC, ORI, GYR, EMPTY_SENSOR };

// List of pre-sampled gestures
enum Gesture{ EAT, HELP, SLEEP, THANKYOU, WHY, NONE };
static const int NUMBER_OF_GESTURES = 5;

// List of pre-sample files
static std::string testFileList[] = {
	"eat01.json",
	"eat02.json",
	"eat03.json",
	"help01.json",
	"help02.json",
	"help03.json",
	"sleep01.json",
	"sleep02.json",
	"sleep03.json",
	"thankyou01.json",
	"thankyou02.json",
	"thankyou03.json",
	"why01.json",
	"why02.json",
	"why03.json"
};
static const int NUMBER_OF_TRANING_PER_GESTURE = 3; // pre-samples per gesture
static const int TRANING_SIZE = NUMBER_OF_GESTURES * NUMBER_OF_TRANING_PER_GESTURE;


static std::string preSampledRecordFileList[] = {
	"test-eat01.json",
	"test-eat02.json",
	"test-eat03.json",
	"test-eat04.json",
	"test-eat05.json",
	"test-eat06.json",
	"test-eat07.json",
	"test-eat08.json",
	"test-eat09.json",
	"test-eat10.json",
	"test-help01.json",
	"test-help02.json",
	"test-help03.json",
	"test-help04.json",
	"test-help05.json",
	"test-help06.json",
	"test-help07.json",
	"test-help08.json",
	"test-help09.json",
	"test-help10.json",
	"test-sleep01.json",
	"test-sleep02.json",
	"test-sleep03.json",
	"test-sleep04.json",
	"test-sleep05.json",
	"test-sleep06.json",
	"test-sleep07.json",
	"test-sleep08.json",
	"test-sleep09.json",
	"test-sleep10.json",
	"test-thankyou01.json",
	"test-thankyou02.json",
	"test-thankyou03.json",
	"test-thankyou04.json",
	"test-thankyou05.json",
	"test-thankyou06.json",
	"test-thankyou07.json",
	"test-thankyou08.json",
	"test-thankyou09.json",
	"test-thankyou10.json",
	"test-why01.json",
	"test-why02.json",
	"test-why03.json",
	"test-why04.json",
	"test-why05.json",
	"test-why06.json",
	"test-why07.json",
	"test-why08.json",
	"test-why09.json",
	"test-why10.json"
};
static Gesture preSampledRecordPredictionList[] = {
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	EAT,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	HELP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	SLEEP,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	THANKYOU,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY,
	WHY
};
static const int  NUMBER_OF_TESTS = 1;


#endif
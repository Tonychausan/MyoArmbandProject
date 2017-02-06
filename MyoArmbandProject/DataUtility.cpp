#include "stdafx.h"

#include <string>

#include "dirent.h"

#include "DataUtility.h"
#include "Constants.h"

std::string gestureToString(Gesture gesture){
	int i = static_cast<int>(gesture);
	return gesture_names[i];
}

std::string sensorToString(Sensor sensor){
	int i = static_cast<int>(sensor);
	return sensor_names[i];
}

std::string getDataSetPath(DataSetFormat format, DataSetType type){
	std::string path = DATA_SET_FOLDER;

	std::string format_folder = COMPRESSED_DATA_FOLDER;
	if (format == RAW)
		format_folder = RAW_DATA_FOLDER;
	path.append(format_folder);

	std::string type_folder = TEST_SET_FOLDER;
	if (type == TRAINING)
		type_folder = TRAINING_SET_FOLDER;
	path.append(type_folder);

	return path;
};

void generateFilelist(Filelist *filelist, std::string path_string){
	const char *path = path_string.c_str();

	DIR *dir;
	struct dirent *ent;

	int file_counter = 0;
	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if ((std::string)ent->d_name == "." || (std::string)ent->d_name == "..")
				continue;
			file_counter++;
		}
		closedir(dir);
	}
	else {
		perror("");
	}

	filelist->size = file_counter;
	filelist->files = new std::string[file_counter];
	if ((dir = opendir(path)) != NULL) {
		int i = 0;
		while ((ent = readdir(dir)) != NULL) {
			if ((std::string)ent->d_name == "." || (std::string)ent->d_name == "..")
				continue;
			filelist->files[i] = (std::string)ent->d_name;
			i++;
		}
		closedir(dir);
	}
	else {
		perror("");
	}
}


#include "DataUtility.h"
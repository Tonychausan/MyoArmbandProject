#ifndef UtilityH
#define UtilityH

#include <string>
#include <array>
#include <myo/myo.hpp>

static bool isProgramRunning = true;

void ClearScreen();

class CsvHandler{
private:
	std::string filename;
public:
	CsvHandler(std::string);

	void addSampleDataLine(std::string);

	void createFile();
	void removeFile();

	void setFilename(std::string);
};


class EmgCsvHandler : public CsvHandler{
private:
	const std::string foldername = "emg/";
public:
	EmgCsvHandler(std::string);

	void addEmgSampleDataLine(std::array<int8_t, 8>);
};

#endif
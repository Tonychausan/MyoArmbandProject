#include "stdafx.h"

#include <windows.h>
#include <fstream>
#include <iostream>
#include <cmath>

#include "Utility.h"


void ClearScreen()
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) return;

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

CsvHandler::CsvHandler(std::string name)
{
	setFilename(name);
}

void CsvHandler::addSampleDataLine(std::string inputLine)
{
	std::ofstream file;
	file.open(filename, std::ios::app);

	file << inputLine << std::endl;

	file.close();
}

void CsvHandler::createFile()
{
	std::ofstream file;
	file.open(filename);
	file.close();
}

void CsvHandler::removeFile()
{
	std::remove(filename.c_str());
}

void CsvHandler::setFilename(std::string name)
{
	filename = name;
}

EmgCsvHandler::EmgCsvHandler(std::string name)
	:CsvHandler(name)
{
	std::string tempFoldername = foldername;
	setFilename(tempFoldername.append(name));
}

void EmgCsvHandler::addEmgSampleDataLine(std::array<int8_t, 8> emgData)
{
	std::string addLine = std::to_string(emgData[0]);
	for (int i = 1; i < 8; i++) {
		std::string tempString = std::to_string(emgData[i]);

		addLine.append(",");
		addLine.append(tempString);
	}
	addLine.append("\n");
	addSampleDataLine(addLine);
}

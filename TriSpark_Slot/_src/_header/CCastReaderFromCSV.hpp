#pragma once
#include "IReadCSV.h"
struct SCastData;

class CCastReaderFromCSV : IReadCSVBase {
public:
	bool FileInit();
	bool FileInit(int pFileID);
	bool MakeData(SCastData& p_Data);
};

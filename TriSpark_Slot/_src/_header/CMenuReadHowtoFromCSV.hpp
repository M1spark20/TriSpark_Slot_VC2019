#pragma once
#include "IReadcsv.h"

class CMenuReadHowtoFromCSV : IReadCSVBase {
public:
	bool FileInit(int pFileID);
	int GetImageID(int pageNo);
};

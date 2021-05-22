#pragma once
#include "IReadcsv.h"
#include <vector>

struct SMenuReadBonusTypePos {
	int payoutID, resID, x, y, w, h;
	SMenuReadBonusTypePos() { payoutID = -1; resID = -1; x = -1; y = -1; w = -1; h = -1; }
};

class CMenuReadBonusTypeFromCSV : IReadCSVBase {
public:
	bool FileInit(int pFileID);
	bool GetImagePos(std::vector<SMenuReadBonusTypePos>& pData);
};
#pragma once
#include <vector>

struct SReelChaUnit{
	int charaID;
	int imageID;
	int x, y, w, h;
};

struct SReelChaData{
	std::vector<SReelChaUnit>	reelData;
	int							reelID;
	std::vector<int>			arrayData;
};
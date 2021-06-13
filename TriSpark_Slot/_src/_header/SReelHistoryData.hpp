#pragma once
#include <vector>

struct SReelHistoryData {
	std::vector<int>	reelPos;
	std::vector<int>	slipCount;
	int					betNum;
	int					firstStop;

	SReelHistoryData() {
		betNum = 0;
		firstStop = -1;
	}
};

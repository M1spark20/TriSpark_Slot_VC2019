#pragma once
#include <vector>

struct SReelControlHeader{
	char			flagID;
	char			bonusID;
	char			ctrlType;
	unsigned short	dataSize;
};

struct SReelControlSpot{
	unsigned short	spot;
	int				slipStart;
};

struct SReelControlCommand{
	std::vector<SReelControlSpot>	spotData;
	std::vector<unsigned long long>	slipData;
};

struct SReelControlData{
	std::vector<SReelControlHeader>		header;		// 制御データヘッダ
	std::vector<SReelControlCommand>	command;	// 制御データ本体
};

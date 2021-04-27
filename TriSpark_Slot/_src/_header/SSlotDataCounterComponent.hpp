#pragma once
#include <vector>

struct SSlotDataCounterBonusHistoryData {
	int startGame;
	int medalBefore;
	int medalAfter;
	int getPayoutEffect;	// 図柄特定用
	int flagLossGame;
	std::vector<int> flagMageGameReelPos;
	bool isActivate;
	bool isSetGet;

	SSlotDataCounterBonusHistoryData() {
		startGame = -1;
		medalBefore = -1;
		medalAfter = -1;
		getPayoutEffect = -1;
		flagLossGame = 0;
		isActivate = false;
		isSetGet = false;
	}
};

struct SSlotDataCounterComponent {
	int												totalGame;
	int												startGame;
	int												inCount;
	int												outCount;
	float											payoutRate;
	int												payoutRateX10;

	SSlotDataCounterComponent() {
		totalGame = 0;
		startGame = 0;
		inCount = 0;
		outCount = 0;
		payoutRate = 0.f;
		payoutRateX10 = 0;
	}
};

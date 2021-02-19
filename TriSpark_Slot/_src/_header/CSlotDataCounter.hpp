#pragma once
#include "SSlotDataCounterComponent.hpp"
#include <vector>
class CSlotInternalDataManager;

class CSlotDataCounter {
	SSlotDataCounterComponent	mCountData;
	int							mLastGameMode;

	std::vector<int>								mCoinStatusForGraph;	// History: medalCount
	std::vector<SSlotDataCounterBonusHistoryData>	mBonusHistory;
	std::vector<std::pair<int, int>>				mBonusCount;			// gameModeFor, count

	void PayoutCalculate();
public:
	CSlotDataCounter();
	void ReelStart(const CSlotInternalDataManager& pInternal);
	void SetResult(const CSlotInternalDataManager& pInternal, const int pPayoutEffect);

	SSlotDataCounterComponent GetBasicData() const { return mCountData; }
	SSlotDataCounterBonusHistoryData GetBonusHistory(int pHistCount) const;
	int GetBonusCount(int pGameModeType) const;
	int GetBonusRateX10(int pGameModeType) const;
};

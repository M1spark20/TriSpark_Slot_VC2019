#pragma once
#include "SSlotDataCounterComponent.hpp"
#include <vector>
#include <deque>
class CSlotInternalDataManager;
class CReelManager;

class CSlotDataCounter {
	SSlotDataCounterComponent	mCountData;
	int							mLastGameMode;
	int							mLastBonusFlag;
	static const int			GRAPH_INTERVAL = 10;
	static const int			GRAPH_MAXSIZE  = 100000;
	int							mNextStoreGraphGame;

	std::deque<int>									mCoinStatusForGraph;	// History: medalCount
	std::vector<SSlotDataCounterBonusHistoryData>	mBonusHistory;
	std::vector<std::pair<int, int>>				mBonusCount;			// gameModeFor, count

	void PayoutCalculate();
public:
	CSlotDataCounter();
	void ReelStart(const CSlotInternalDataManager& pInternal);
	void SetResult(const CSlotInternalDataManager& pInternal, const CReelManager& pReel, const int pPayoutEffect);

	SSlotDataCounterComponent GetBasicData() const { return mCountData; }
	SSlotDataCounterBonusHistoryData GetBonusHistory(int pHistCount) const;
	int GetBonusCount(int pGameModeType) const;
	int GetBonusRateX10(int pGameModeType) const;

	// bool DrawCoinGraphData();

};

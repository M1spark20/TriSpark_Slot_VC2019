#include "_header/CSlotDataCounter.hpp"
#include "_header/CSlotInternalDataManager.hpp"

CSlotDataCounter::CSlotDataCounter() {
	mLastGameMode = 0;
}

void CSlotDataCounter::PayoutCalculate() {
	if (mCountData.inCount == 0)	mCountData.payoutRate = 100.f;
	else							mCountData.payoutRate = mCountData.outCount / (float)mCountData.inCount * 100.f;

	mCountData.payoutRateX10 = (int)(mCountData.payoutRate * 10.f);
}

void CSlotDataCounter::ReelStart(const CSlotInternalDataManager& pInternal) {
	const auto internalData = pInternal.GetData();
	if (internalData.gameMode == 0 && internalData.betNum == 3) {
		++mCountData.totalGame;
		++mCountData.startGame;
	}
	// inCountのみ同期
	mCountData.inCount = internalData.inCount;
	PayoutCalculate();
}

// 呼び出しタイミング:払い出し終了後
void CSlotDataCounter::SetResult(const CSlotInternalDataManager& pInternal, const int pPayoutEffect) {
	const auto internalData = pInternal.GetData();

	mCountData.inCount = internalData.inCount;
	mCountData.outCount = internalData.outCount;
	PayoutCalculate();

	if (mLastGameMode != internalData.gameMode) {
		if (mLastGameMode == 0) {
			// ボーナススタート
			SSlotDataCounterBonusHistoryData bonusHistory;
			bonusHistory.startGame = mCountData.startGame;
			bonusHistory.medalBefore = mCountData.outCount - mCountData.inCount;
			bonusHistory.getPayoutEffect = pPayoutEffect;
			mBonusHistory.push_back(bonusHistory);

			bool addFlag = false;
			for (auto& bCount : mBonusCount) {
				if (bCount.first != internalData.gameMode) continue;
				++bCount.second;
				addFlag = true;	break;
			}
			if (!addFlag) {
				mBonusCount.push_back(std::pair<int, int>(internalData.gameMode, 1));
			}
		} else {
			// ボーナス終了
			mCountData.startGame = 0;
			auto it = mBonusHistory.rbegin();
			if (it != mBonusHistory.rend()) {
				it->medalAfter = mCountData.outCount - mCountData.inCount;
				it->isSetGet = true;
			}
		}
	}
	mLastGameMode = internalData.gameMode;

	// グラフ実装は後で
}

SSlotDataCounterBonusHistoryData CSlotDataCounter::GetBonusHistory(int pHistCount) const {
	if (pHistCount < 0 || (size_t)pHistCount >= mBonusHistory.size())
		return SSlotDataCounterBonusHistoryData();
	return mBonusHistory[mBonusHistory.size() - 1 - (size_t)pHistCount];
}

int CSlotDataCounter::GetBonusCount(int pGameModeType) const {
	for (auto& bCount : mBonusCount) {
		if (bCount.first != pGameModeType) continue;
		return bCount.second;
	}
	return 0;
}

int CSlotDataCounter::GetBonusRateX10(int pGameModeType) const {
	const int bonusNum = GetBonusCount(pGameModeType);
	if (bonusNum == 0) return 0;
	return mCountData.totalGame * 10 / bonusNum;
}

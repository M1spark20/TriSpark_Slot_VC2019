#include "_header/CSlotDataCounter.hpp"
#include "_header/CSlotInternalDataManager.hpp"
#include "_header/CReelManager.hpp"

CSlotDataCounter::CSlotDataCounter() {
	mLastGameMode = 0;
	mLastBonusFlag = 0;
	mNextStoreGraphGame = GRAPH_INTERVAL;
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

	// グラフ描画ゲーム減算(0で描画点追加)
	--mNextStoreGraphGame;

	// ボーナス成立後はlossGame加算
	if (!mBonusHistory.empty()){
		if (!mBonusHistory.rbegin()->isActivate) ++mBonusHistory.rbegin()->flagLossGame;
	}
}

// 呼び出しタイミング:払い出し終了後
void CSlotDataCounter::SetResult(const CSlotInternalDataManager& pInternal,const CReelManager& pReel, const int pPayoutEffect) {
	const auto internalData = pInternal.GetData();

	mCountData.inCount = internalData.inCount;
	mCountData.outCount = internalData.outCount;
	PayoutCalculate();

	// ボーナス成立察知
	if (mLastBonusFlag != internalData.flag.second && mLastBonusFlag == 0) {
		SSlotDataCounterBonusHistoryData bonusHistory;
		bonusHistory.flagMageGameReelPos = pReel.GetReelPos();
		mBonusHistory.push_back(bonusHistory);
	}
	mLastBonusFlag = internalData.flag.second;

	if (mLastGameMode != internalData.gameMode) {
		if (mLastGameMode == 0) {
			// ボーナススタート
			if (!mBonusHistory.empty()) {
				SSlotDataCounterBonusHistoryData& bonusHistory = *mBonusHistory.rbegin();
				bonusHistory.startGame = mCountData.startGame;
				bonusHistory.medalBefore = mCountData.outCount - mCountData.inCount;
				bonusHistory.getPayoutEffect = pPayoutEffect;
				bonusHistory.isActivate = true;

				bool addFlag = false;
				for (auto& bCount : mBonusCount) {
					if (bCount.first != internalData.gameMode) continue;
					++bCount.second;
					addFlag = true;	break;
				}
				if (!addFlag) {
					mBonusCount.push_back(std::pair<int, int>(internalData.gameMode, 1));
				}
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

	// グラフ用描画点定義
	if (mNextStoreGraphGame <= 0) {
		mNextStoreGraphGame = GRAPH_INTERVAL;
		if (mCoinStatusForGraph.size() >= (size_t)GRAPH_MAXSIZE) mCoinStatusForGraph.pop_front();
		mCoinStatusForGraph.push_back(mCountData.outCount - mCountData.inCount);
	}
}

SSlotDataCounterBonusHistoryData CSlotDataCounter::GetBonusHistory(int pHistCount) const {
	// ボーナス入賞前の場合、データを1つスキップする
	if (mBonusHistory.empty()) return SSlotDataCounterBonusHistoryData();
	if (!mBonusHistory.rbegin()->isActivate) ++pHistCount;

	if (pHistCount < 0 || (size_t)pHistCount >= mBonusHistory.size())
		return SSlotDataCounterBonusHistoryData();
	return mBonusHistory[mBonusHistory.size() - 1 - (size_t)pHistCount];
}

int CSlotDataCounter::GetBonusCount(int pGameModeType) const {
	// 全ボーナス合算
	if (pGameModeType <= 0) {
		int ans = 0;
		for (auto& bCount : mBonusCount) { ans += bCount.second; }
		return ans;
	}

	// 各種ボーナスのみ
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

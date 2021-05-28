#include "_header/CSlotDataCounter.hpp"
#include "_header/CSlotInternalDataManager.hpp"
#include "_header/CReelManager.hpp"
#include "_header/CRestoreManager.hpp"

CSlotDataCounter::CSlotDataCounter() {
	mLastGameMode = 0;
	mLastBonusFlag = 0;
	mNextStoreGraphGame = GRAPH_INTERVAL;
	mIsMadeHistoryData = false;
	mCoinStatusForGraph.push_back(0);
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
		bonusHistory.flagMadeGameReel = pReel.GetHistoryData();
		mBonusHistory.push_back(bonusHistory);
		mIsMadeHistoryData = true;
	}
	mLastBonusFlag = internalData.flag.second;

	if (mLastGameMode != internalData.gameMode) {
		if (mLastGameMode == 0) {
			// ボーナススタート
			if (!mIsMadeHistoryData) {
				SSlotDataCounterBonusHistoryData bonusHistory;
				bonusHistory.flagMadeGameReel = pReel.GetHistoryData();
				mBonusHistory.push_back(bonusHistory);
			}
			mIsMadeHistoryData = false;
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


bool CSlotDataCounter::ReadRestore(CRestoreManagerRead& pReader) {
	if (!pReader.ReadNum(mCountData.totalGame)) return false;
	if (!pReader.ReadNum(mCountData.startGame)) return false;
	if (!pReader.ReadNum(mCountData.inCount)) return false;
	if (!pReader.ReadNum(mCountData.outCount)) return false;
	if (!pReader.ReadNum(mCountData.payoutRate)) return false;
	if (!pReader.ReadNum(mCountData.payoutRateX10)) return false;

	if (!pReader.ReadNum(mLastGameMode)) return false;
	if (!pReader.ReadNum(mLastBonusFlag)) return false;
	if (!pReader.ReadNum(mNextStoreGraphGame)) return false;
	if (!pReader.ReadNum(mIsMadeHistoryData)) return false;

	size_t loopCount = 0;
	mCoinStatusForGraph.clear();
	if (!pReader.ReadNum(loopCount)) return false;
	for (size_t i = 0; i < loopCount; ++i) {
		int coin = 0;
		if (!pReader.ReadNum(coin)) return false;
		mCoinStatusForGraph.push_back(coin);
	}

	if (!pReader.ReadNum(loopCount)) return false;
	for (size_t i = 0; i < loopCount; ++i) {
		SSlotDataCounterBonusHistoryData data;
		if (!pReader.ReadNum(data.startGame)) return false;
		if (!pReader.ReadNum(data.medalBefore)) return false;
		if (!pReader.ReadNum(data.medalAfter)) return false;
		if (!pReader.ReadNum(data.getPayoutEffect)) return false;
		if (!pReader.ReadNum(data.flagLossGame)) return false;

		size_t reelNum = 0;
		if (!pReader.ReadNum(reelNum)) return false;
		for (int j = 0; j < reelNum; ++j) {
			int reelPos = 0;
			if (!pReader.ReadNum(reelPos)) return false;
			data.flagMadeGameReel.reelPos.push_back(reelPos);
		}
		if (!pReader.ReadNum(data.flagMadeGameReel.betNum)) return false;
		if (!pReader.ReadNum(data.flagMadeGameReel.firstStop)) return false;

		if (!pReader.ReadNum(data.isActivate)) return false;
		if (!pReader.ReadNum(data.isSetGet)) return false;
		mBonusHistory.push_back(data);
	}

	if (!pReader.ReadNum(loopCount)) return false;
	for (size_t i = 0; i < loopCount; ++i) {
		std::pair<int, int> bCount;
		if (!pReader.ReadNum(bCount)) return false;
		mBonusCount.push_back(bCount);
	}

	return true;
}

bool CSlotDataCounter::WriteRestore(CRestoreManagerWrite& pWriter) const {
	if (!pWriter.WriteNum(mCountData.totalGame)) return false;
	if (!pWriter.WriteNum(mCountData.startGame)) return false;
	if (!pWriter.WriteNum(mCountData.inCount)) return false;
	if (!pWriter.WriteNum(mCountData.outCount)) return false;
	if (!pWriter.WriteNum(mCountData.payoutRate)) return false;
	if (!pWriter.WriteNum(mCountData.payoutRateX10)) return false;

	if (!pWriter.WriteNum(mLastGameMode)) return false;
	if (!pWriter.WriteNum(mLastBonusFlag)) return false;
	if (!pWriter.WriteNum(mNextStoreGraphGame)) return false;
	if (!pWriter.WriteNum(mIsMadeHistoryData)) return false;

	if (!pWriter.WriteNum((size_t)mCoinStatusForGraph.size())) return false;
	for (size_t i = 0; i < mCoinStatusForGraph.size(); ++i) {
		if (!pWriter.WriteNum(mCoinStatusForGraph[i])) return false;
	}

	if (!pWriter.WriteNum((size_t)mBonusHistory.size())) return false;
	for (size_t i = 0; i < mBonusHistory.size(); ++i) {
		if (!pWriter.WriteNum(mBonusHistory[i].startGame)) return false;
		if (!pWriter.WriteNum(mBonusHistory[i].medalBefore)) return false;
		if (!pWriter.WriteNum(mBonusHistory[i].medalAfter)) return false;
		if (!pWriter.WriteNum(mBonusHistory[i].getPayoutEffect)) return false;
		if (!pWriter.WriteNum(mBonusHistory[i].flagLossGame)) return false;

		if (!pWriter.WriteNum((size_t)mBonusHistory[i].flagMadeGameReel.reelPos.size())) return false;
		for (int j = 0; j < mBonusHistory[i].flagMadeGameReel.reelPos.size(); ++j) {
			if (!pWriter.WriteNum(mBonusHistory[i].flagMadeGameReel.reelPos[j])) return false;
		}
		if (!pWriter.WriteNum(mBonusHistory[i].flagMadeGameReel.betNum)) return false;
		if (!pWriter.WriteNum(mBonusHistory[i].flagMadeGameReel.firstStop)) return false;

		if (!pWriter.WriteNum(mBonusHistory[i].isActivate)) return false;
		if (!pWriter.WriteNum(mBonusHistory[i].isSetGet)) return false;
	}

	if (!pWriter.WriteNum((size_t)mBonusCount.size())) return false;
	for (size_t i = 0; i < mBonusCount.size(); ++i) {
		if (!pWriter.WriteNum(mBonusCount[i])) return false;
	}

	return true;
}
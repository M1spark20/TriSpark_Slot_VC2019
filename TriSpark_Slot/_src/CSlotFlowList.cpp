#include "_header/CSlotFlowList.hpp"
#include "_header/SSlotGameDataWrapper.hpp"
#include "_header/keyexport.h"
#include "DxLib.h"

CSlotFlowBet::CSlotFlowBet() : BET_INTERVAL(std::pair<int, int>(65, 170)){
}

bool CSlotFlowBet::Init(SSlotGameDataWrapper& pGameData){
	m_betFor = 0;
	pGameData.timeManager.SetTimer(eTimerBetWaitStart);
	const unsigned int lastBetCount = pGameData.internalDataManager.GetData().betNum;
	pGameData.internalDataManager.ResetBetCount(false);
	if (pGameData.internalDataManager.GetData().isReplay) {
		SetBetFor(pGameData, lastBetCount, true);
	}
	return true;
}

ESlotFlowFlag CSlotFlowBet::Process(SSlotGameDataWrapper& pGameData){
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	long long timeGetter;
	const auto intData = pGameData.internalDataManager.GetData();
	const bool isReverAvailable = pGameData.timeManager.GetTime(timeGetter, eTimerLeverAvailable);
	bool isBetInput = false;
	pGameData.timeManager.GetTime(timeGetter, eTimerBetWaitStart);
	if (timeGetter >= intData.betFreeze || intData.isReplay) {
		if (isReverAvailable) {
			// リール始動
			if (key.ExportKeyState(KEY_INPUT_UP) && key.GetExportStatus() == EKeyExportStatus::eGameMain) {
				pGameData.internalDataManager.LatchBet();
				pGameData.dataCounter.ReelStart(pGameData.internalDataManager);

				pGameData.timeManager.DisableTimer(eTimerBetWaitStart);
				pGameData.timeManager.DisableTimer(eTimerLeverAvailable);
				/* Payoutリセット */
				pGameData.timeManager.DisableTimer(eTimerPayout);
				return eFlowWaiting;
			}
		}
		isBetInput = pGameData.timeManager.GetTime(timeGetter, eTimerBetInput);
		if ((m_betFor == 0 || !isBetInput) && !intData.isReplay && key.GetExportStatus() == EKeyExportStatus::eGameMain) {
			if (key.ExportKeyState(KEY_INPUT_1)) {
				SetBetFor(pGameData, 1, false);
				isBetInput = true;
			}
			if (key.ExportKeyState(KEY_INPUT_2)) {
				int betNum = 2;
				if (pGameData.randManager.GetMaxBetNum(intData.gameMode) < betNum)
					betNum = pGameData.randManager.GetMaxBetNum(intData.gameMode);
				SetBetFor(pGameData, 2, false);
				isBetInput = true;
			}
			if (key.ExportKeyState(KEY_INPUT_3) || key.ExportKeyState(KEY_INPUT_UP)) {
				SetBetFor(pGameData, pGameData.randManager.GetMaxBetNum(intData.gameMode), false);
				isBetInput = true;
			}
		}
	}

	// ベット加算処理
	if (m_betFor > 0 && isBetInput){
		const int waitTime = intData.isReplay ? BET_INTERVAL.second : BET_INTERVAL.first;
		while (true){
			if (!pGameData.timeManager.GetTime(timeGetter, eTimerBetInput)) return eFlowErrEnd;
			// betは変動するため最新の値を常に取得する
			const unsigned int nowBetCount = pGameData.internalDataManager.GetData().betNum;
			if (timeGetter < waitTime*nowBetCount) break;
			if (nowBetCount >= (unsigned int)m_betFor){
				pGameData.timeManager.DisableTimer(eTimerBetInput);
				// レバーを有効にできるか
				if (pGameData.randManager.GetBetAvailable(intData.gameMode, nowBetCount)) {
					pGameData.timeManager.SetTimer(eTimerLeverAvailable);
					if (intData.isReplay) pGameData.restoreManager.SetActivate();
				}
				break;
			} else {
				pGameData.internalDataManager.AddBetCount(intData.isReplay);
			}
		}
	}

	if (!intData.isReplay) pGameData.restoreManager.SetActivate();
	return eFlowContinue;
}

void CSlotFlowBet::SetBetFor(SSlotGameDataWrapper& pGameData, int pBetNum, bool pIsReplay){
	if (m_betFor > pBetNum){
		pGameData.internalDataManager.ResetBetCount(true);
		m_betFor = 0;
	}
	pGameData.timeManager.DisableTimer(eTimerLeverAvailable);
	const int waitTime = pIsReplay ? BET_INTERVAL.second : BET_INTERVAL.first;
	pGameData.timeManager.SetTimer(eTimerBetInput, waitTime*m_betFor);
	m_betFor = pBetNum;
	/* エフェクトリセット */
}

bool CSlotFlowWaiting::Init(SSlotGameDataWrapper& pGameData){
	pGameData.castChecker.ResetCastData();
	// フラグ抽選
	pGameData.randManager.Role(pGameData.internalDataManager);
	// タイマーセット
	pGameData.timeManager.SetTimer(eTimerWaitStart);
	// データ保存フラグリセット
	pGameData.restoreManager.ActivateFlagReset();
	return true;
}

ESlotFlowFlag CSlotFlowWaiting::Process(SSlotGameDataWrapper& pGameData){
	long long intervalTime;
	const int waitMax = 0;
	if (!pGameData.timeManager.GetTime(intervalTime, eTimerAllReelStart)){
		pGameData.timeManager.DisableTimer(eTimerWaitStart);
		pGameData.timeManager.SetTimer(eTimerWaitEnd);
		return eFlowReelAcceration;
	}
	if (intervalTime >= waitMax){
		pGameData.timeManager.DisableTimer(eTimerWaitStart);
		pGameData.timeManager.SetTimer(eTimerWaitEnd);
		return eFlowReelAcceration;
	}
	return eFlowContinue;
}

bool CSlotFlowReelAcc::Init(SSlotGameDataWrapper& pGameData){
	// リール始動(各リールのタイマー制御はCReel側で行う)
	const auto data = pGameData.internalDataManager.GetData();
	for (int i = 0; i < pGameData.reelManager.GetReelNum(); ++i){
		pGameData.reelManager.StartReel(data.flag.first, data.flag.second, data.betNum);
	}

	pGameData.timeManager.DisableTimer(eTimerAllReelStop);
	pGameData.timeManager.SetTimer(eTimerAllReelStart);
	return true;
}

ESlotFlowFlag CSlotFlowReelAcc::Process(SSlotGameDataWrapper& pGameData){
	if (pGameData.reelManager.GetRotatingReelNum() == pGameData.reelManager.GetReelNum())
		return eFlowReelMoving;
	return eFlowContinue;
}

bool CSlotFlowReelMove::Init(SSlotGameDataWrapper& pGameData){
	return true;
}

ESlotFlowFlag CSlotFlowReelMove::Process(SSlotGameDataWrapper& pGameData){
	CKeyExport_S& key = CKeyExport_S::GetInstance();

	bool noInput = (key.ExportKeyStateFrame(KEY_INPUT_UP) == 0);
	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_1) == 0);
	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_2) == 0);
	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_3) == 0);
	noInput &= (key.GetExportStatus() == EKeyExportStatus::eGameMain);

	if (noInput){
		if (key.ExportKeyState(KEY_INPUT_LEFT)){
			pGameData.reelManager.StopReel(0);
			pGameData.reachCollection.JudgeCollection(pGameData.reelManager, pGameData.internalDataManager);
		}
		else if (key.ExportKeyState(KEY_INPUT_DOWN)) {
			pGameData.reelManager.StopReel(1);
			pGameData.reachCollection.JudgeCollection(pGameData.reelManager, pGameData.internalDataManager);
		}
		else if (key.ExportKeyState(KEY_INPUT_RIGHT)){
			pGameData.reelManager.StopReel(2);
			pGameData.reachCollection.JudgeCollection(pGameData.reelManager, pGameData.internalDataManager);
		}
	}

	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_LEFT)	 == 0);
	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_DOWN)	 == 0);
	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_RIGHT) == 0);
	if(!pGameData.castChecker.SetReachData(pGameData)) return eFlowErrEnd;

	if (pGameData.reelManager.GetRotatingReelNum() == 0){
		long long temp;
		if (!pGameData.timeManager.GetTime(temp, eTimerAllReelStop))
			pGameData.timeManager.SetTimer(eTimerAllReelStop);

		if (noInput) return eFlowPayout;
	}
	return eFlowContinue;
}

bool CSlotFlowPayout::Init(SSlotGameDataWrapper& pGameData){
	const int beforeMode = pGameData.internalDataManager.GetData().gameMode;
	pGameData.timeManager.SetTimer(eTimerPayout);
	if (beforeMode == 0) pGameData.reelManager.SetHistoryData();
	if (!pGameData.castChecker.SetCastData(pGameData)) return false;
	m_payoutFor = pGameData.castChecker.GetPayout();
	pGameData.internalDataManager.SetReplayStatus(pGameData.castChecker.IsReplay());
	pGameData.castChecker.SetGameMode(pGameData.internalDataManager);

	const int afterMode = pGameData.internalDataManager.GetData().gameMode;
	pGameData.reachCollection.Latch(beforeMode == afterMode);

	m_isDummyPay = m_payoutFor < 0;
	if (m_isDummyPay) m_payoutFor *= -1;
	m_nowPayCount = 0;
	return true;
}

ESlotFlowFlag CSlotFlowPayout::Process(SSlotGameDataWrapper& pGameData){
	const int payTime[] = { 115, 800 };
	const auto data = pGameData.internalDataManager.GetData();
	long long time;
	if (data.isReplay){
		if (!pGameData.timeManager.GetTime(time, eTimerPayout)) return eFlowErrEnd;
		if (time - data.payoutFreeze >= payTime[1]){
			pGameData.internalDataManager.CheckGameModeEnd(true, false);
			pGameData.dataCounter.SetResult(pGameData.internalDataManager, pGameData.reelManager, pGameData.castChecker.GetPayoutEffect());
			return eFlowBetting;
		}
	} else {
		while (true){
			if (!pGameData.timeManager.GetTime(time, eTimerPayout)) return eFlowErrEnd;
			// betは変動するため最新の値を常に取得する
			if (time - data.payoutFreeze < payTime[0]*m_nowPayCount) break;
			if (m_nowPayCount >= m_payoutFor){
				pGameData.internalDataManager.CheckGameModeEnd(true, m_payoutFor>0 && !m_isDummyPay);
				pGameData.dataCounter.SetResult(pGameData.internalDataManager, pGameData.reelManager, pGameData.castChecker.GetPayoutEffect());
				return eFlowBetting;
			}

			if (!m_isDummyPay) pGameData.internalDataManager.AddPayout();
			++m_nowPayCount;
		}
	}
	return eFlowContinue;
}

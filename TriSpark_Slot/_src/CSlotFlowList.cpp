#include "_header\CSlotFlowList.hpp"
#include "_header\SSlotGameDataWrapper.hpp"
#include "_header\keyexport.h"
#include "DxLib.h"

CSlotFlowBet::CSlotFlowBet() : BET_INTERVAL(std::pair<int, int>(65, 130)){
}

bool CSlotFlowBet::Init(SSlotGameDataWrapper& pGameData){
	m_betFor = 0;
	pGameData.timeManager.SetTimer(eTimerBetWaitStart);
	const unsigned int lastBetCount = pGameData.internalDataManager.GetData().betNum;
	pGameData.internalDataManager.ResetBetCount(false);
	if (pGameData.internalDataManager.GetData().isReplay)
		SetBetFor(pGameData, lastBetCount, true);
	return true;
}

ESlotFlowFlag CSlotFlowBet::Process(SSlotGameDataWrapper& pGameData){
	CKeyExport_S& key = CKeyExport_S::GetInstance();
	long long timeGetter;
	const auto intData = pGameData.internalDataManager.GetData();
	const bool isReverAvailable = pGameData.timeManager.GetTime(timeGetter, eTimerLeverAvailable);
	if (isReverAvailable){
		// リール始動
		if (key.ExportKeyState(KEY_INPUT_UP)){
			pGameData.internalDataManager.LatchBet();
			pGameData.timeManager.DisableTimer(eTimerBetWaitStart);
			pGameData.timeManager.DisableTimer(eTimerLeverAvailable);
			/* Payoutリセット */
			pGameData.timeManager.DisableTimer(eTimerPayout);
			return eFlowWaiting;
		}
	}
	bool isBetInput = pGameData.timeManager.GetTime(timeGetter, eTimerBetInput);
	if ((m_betFor == 0 || !isBetInput) && !intData.isReplay){
		if (key.ExportKeyState(KEY_INPUT_1)){
			SetBetFor(pGameData, 1, false);
			isBetInput = true;
		}
		if (key.ExportKeyState(KEY_INPUT_3) || key.ExportKeyState(KEY_INPUT_UP)){
			SetBetFor(pGameData, pGameData.randManager.GetMaxBetNum(intData.gameMode), false);
			isBetInput = true;
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
				if (pGameData.randManager.GetBetAvailable(intData.gameMode, nowBetCount))
					pGameData.timeManager.SetTimer(eTimerLeverAvailable);
				break;
			} else {
				pGameData.internalDataManager.AddBetCount(intData.isReplay);
			}
		}
	}
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
	// フラグ抽選
	pGameData.randManager.Role(pGameData.internalDataManager);
	// タイマーセット
	pGameData.timeManager.SetTimer(eTimerWaitStart);
	return true;
}

ESlotFlowFlag CSlotFlowWaiting::Process(SSlotGameDataWrapper& pGameData){
	long long intervalTime;
	const int waitMax = 4100;
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
		pGameData.reelManager.StartReel(data.flag.first, data.flag.second);
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

	if (noInput){
		if (key.ExportKeyState(KEY_INPUT_LEFT)){
			pGameData.reelManager.StopReel(0);
		}
		else if (key.ExportKeyState(KEY_INPUT_DOWN)) {
			pGameData.reelManager.StopReel(1);
		}
		else if (key.ExportKeyState(KEY_INPUT_RIGHT)){
			pGameData.reelManager.StopReel(2);
		}
	}

	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_LEFT)	 == 0);
	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_DOWN)	 == 0);
	noInput &= (key.ExportKeyStateFrame(KEY_INPUT_RIGHT) == 0);

	if (pGameData.reelManager.GetRotatingReelNum() == 0){
		long long temp;
		if (!pGameData.timeManager.GetTime(temp, eTimerAllReelStop))
			pGameData.timeManager.SetTimer(eTimerAllReelStop);

		if (noInput) return eFlowPayout;
	}
	return eFlowContinue;
}

bool CSlotFlowPayout::Init(SSlotGameDataWrapper& pGameData){
	pGameData.timeManager.SetTimer(eTimerPayout);
	if (!pGameData.castChecker.SetCastData(pGameData)) return false;
	m_payoutFor = pGameData.castChecker.GetPayout();
	pGameData.internalDataManager.SetReplayStatus(pGameData.castChecker.IsReplay());
	pGameData.castChecker.SetGameMode(pGameData.internalDataManager);

	m_isDummyPay = m_payoutFor < 0;
	if (m_isDummyPay) m_payoutFor *= -1;
	m_nowPayCount = 0;
	return true;
}

ESlotFlowFlag CSlotFlowPayout::Process(SSlotGameDataWrapper& pGameData){
	const int payTime[] = { 120, 1000 };
	const auto data = pGameData.internalDataManager.GetData();
	long long time;
	if (data.isReplay){
		if (!pGameData.timeManager.GetTime(time, eTimerPayout)) return eFlowErrEnd;
		if (time - data.payoutFreeze >= payTime[1]){
			pGameData.internalDataManager.CheckGameModeEnd(true, false);
			return eFlowBetting;
		}
	} else {
		while (true){
			if (!pGameData.timeManager.GetTime(time, eTimerPayout)) return eFlowErrEnd;
			// betは変動するため最新の値を常に取得する
			if (time - data.payoutFreeze < payTime[0]*m_nowPayCount) break;
			if (m_nowPayCount >= m_payoutFor){
				pGameData.internalDataManager.CheckGameModeEnd(true, m_payoutFor>0 && !m_isDummyPay);
				return eFlowBetting;
			}

			if (!m_isDummyPay) pGameData.internalDataManager.AddPayout();
			++m_nowPayCount;
		}
	}
	return eFlowContinue;
}
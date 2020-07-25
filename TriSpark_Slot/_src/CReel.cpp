#include "_header\CReel.hpp"
#include "DxLib.h"
#include "_header\CGameDataManage.h"
#include "_header\SReelDrawData.hpp"
#include "_header\CSlotTimerManager.hpp"
#include <cmath>

bool CReel::Init(const SReelChaData& pReelData){
	m_reelData = pReelData;
	if (m_reelData.arrayData.empty()) return false;
	if (m_reelData.reelData.empty()) return false;
	m_rotatePos = 0.f;
	m_speed = 0.f;
	m_comaPos = 0;
	m_destination = 0;
	m_lastStatus = eStoping;
	m_nowStatus = eStoping;
	return true;
}

bool CReel::Process(CSlotTimerManager& pTimer){
	const float speedMax = (float)m_reelData.reelData[0].h * GetComaNum() * 80.f / 60000.f;
	const float accVal = speedMax / 400.f;	// 300msで飽和

	// タイマー初期化
	if (m_nowStatus == eAccerating && m_lastStatus == eStoping){
		pTimer.DisableTimer(eTimerReelStop, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelStart, m_reelData.reelID);
	}
	if (m_nowStatus == eSliping && m_lastStatus == eRotating){
		pTimer.DisableTimer(eTimerReelStopAvailable, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelPush, m_reelData.reelID);
	}
	/* 初回起動時にstopタイマを作動させる */{
		long long temp;
		if (!pTimer.GetTime(temp, eTimerReelStop, m_reelData.reelID))
			pTimer.SetTimer(eTimerReelStop, m_reelData.reelID);
	}

	if (m_nowStatus == eAccerating){
		long long diff;
		if (!pTimer.GetTimeDiff(diff, eTimerReelStart, m_reelData.reelID)) return false;
		// 加速をより厳密に再現するために、1msずつ速度と位置を更新する
		// ただし最大速度に達した場合それ以上は掛け算で処理
		long long count;
		for (count = 0; count < diff; ++count){
			m_speed += accVal;
			if (m_speed > speedMax){
				pTimer.SetTimer(eTimerReelStopAvailable, m_reelData.reelID);

				m_speed = speedMax; m_nowStatus = eRotating;
				break; 
			}
			m_rotatePos -= m_speed;
		}
		m_rotatePos -= m_speed * (count-diff);
	} else if (m_nowStatus == eRotating || m_nowStatus == eSliping) {
		long long diff;
		if (!pTimer.GetTimeDiff(diff, eTimerReelStart, m_reelData.reelID)) return false;
		m_rotatePos -= m_speed * diff;
	}

	bool reelAdj = false;	// リール補正有無
	// 順回転位置補正(補正時はリールを停止させる)
	while (m_rotatePos < 0.f){
		reelAdj = true;
		m_rotatePos += GetComaNum() * m_reelData.reelData[0].h;
	}
	// 逆回転位置補正(補正時はリールを停止させる)
	while (m_rotatePos > GetComaNum() * m_reelData.reelData[0].h){
		reelAdj = true;
		m_rotatePos -= GetComaNum() * m_reelData.reelData[0].h;
	}

	// リール位置判断 & 停止判断
	const int newComaPos = (unsigned int)std::floorf(m_rotatePos / m_reelData.reelData[0].h);
	if (m_nowStatus == eSliping && m_comaPos == m_destination && (newComaPos != m_destination || reelAdj)){
		m_rotatePos = (float)m_destination * m_reelData.reelData[0].h;
		m_speed = 0.f;	m_nowStatus = eStoping;
		pTimer.DisableTimer(eTimerReelStart, m_reelData.reelID);
		pTimer.DisableTimer(eTimerReelPush, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelStop, m_reelData.reelID);
	}
	m_comaPos = (unsigned int)std::floorf(m_rotatePos / m_reelData.reelData[0].h);
	m_lastStatus = m_nowStatus;
	return true;
}

bool CReel::ReelStart(){
	if (m_nowStatus != eStoping) return false;
	m_nowStatus = eAccerating;
	return true;
}

bool CReel::ReelStop(unsigned int pDest, bool pForceFlag){
	if (pForceFlag){
		// 即時停止
		m_destination = pDest % m_reelData.arrayData.size();
		m_comaPos = m_destination;
		m_speed = 0.f;
		m_rotatePos = (float)m_reelData.reelData[0].h * pDest;
		m_nowStatus = eStoping;
	} else {
		// 滑って停止する
		if (m_nowStatus != eRotating) return false;
		m_destination = pDest % m_reelData.arrayData.size();
		m_nowStatus = eSliping;
	}
	return true;
}

bool CReel::SetFrashData(SReelFrashConfig pInputData){
	m_flashData.push_back(pInputData);
	return true;
}

void CReel::ClearFrashData(){
	m_flashData.clear();
}

int CReel::GetReelComaByReelPos(int pOffset) const{
	if (m_nowStatus != eStoping) return -1;
	const int dst = (m_reelData.arrayData.size() + m_comaPos + pOffset) % m_reelData.arrayData.size();
	return m_reelData.arrayData[dst];
}

int CReel::GetReelComaByFixedPos(int pComa) const{
	if (m_nowStatus != eStoping) return -1;
	if (pComa < 0 || pComa >= m_reelData.arrayData.size()) return -1;
	return m_reelData.arrayData[pComa];
}

bool CReel::DrawReelMain(const CGameDataManage& pDataManager, SReelDrawData pData, int pCanvas, unsigned int pStartComa, bool pIsFixed) const{
	//DxLib::FillGraph(
	const unsigned int comaDivNum = m_reelData.arrayData.size();
	unsigned int destY = 0;
	DxLib::SetDrawScreen(pCanvas);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	for (unsigned int i = 0; i < pData.comaNum+4; ++i){
		const unsigned int pos = (comaDivNum + pStartComa + i-2) % comaDivNum;
		const SReelChaUnit& nowComa = m_reelData.reelData[m_reelData.arrayData[pos]];
		const int graphH = pDataManager.GetDataHandle(nowComa.imageID);
		DxLib::DrawRectGraph(0, destY, nowComa.x, nowComa.y, nowComa.w, nowComa.h, graphH, TRUE);
		destY += nowComa.h;
	}

	const SReelChaUnit& baseComa = m_reelData.reelData[0];
	DxLib::SetDrawScreen(DX_SCREEN_BACK);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	const float extRate[] = { pData.comaW / (float)baseComa.w, pData.comaH / (float)baseComa.h };
	const float drawOffset = (pIsFixed ? 0.f : m_rotatePos - (float)baseComa.h * pStartComa) +
		((float)baseComa.h * 2.f) - (pData.offsetUpper / extRate[1]);
	const float drawSrcH = pData.comaNum * baseComa.h + (pData.offsetLower + pData.offsetUpper) / extRate[1];
	const float drawDstH = pData.comaH * pData.comaNum + pData.offsetUpper + pData.offsetLower;
	DxLib::DrawRectExtendGraphF(pData.x, pData.y, pData.x + pData.comaW, pData.y + drawDstH,
		0, drawOffset, baseComa.w, (int)drawSrcH, pCanvas, TRUE);
	return true;
}

bool CReel::DrawReel(const CGameDataManage& pDataManager, SReelDrawData pData, int pCanvas) const{
	return DrawReelMain(pDataManager, pData, pCanvas, m_comaPos, false);
}

bool CReel::DrawReel(const CGameDataManage& pDataManager, SReelDrawData pData, int pCanvas, unsigned int pComaStart) const{
	if (pComaStart >= m_reelData.arrayData.size()) return false;
	return DrawReelMain(pDataManager, pData, pCanvas, pComaStart, true);
}
#include "_header\CReel.hpp"
#include "DxLib.h"
#include "_header\CGameDataManage.h"
#include "_header\SReelDrawData.hpp"
#include "_header\CSlotTimerManager.hpp"
#include "_header/ImageSourceManager.hpp"
#include "_header/CImageColorManager.hpp"
#include "_header/CImageColorController.hpp"
#include <cmath>

bool CReel::Init(const SReelChaData& pReelData){
	m_reelData = pReelData;
	if (m_reelData.arrayData.empty()) return false;
	if (m_reelData.reelData.empty()) return false;
	m_rotatePos = 0.f;
	m_pushPos = 0.f;
	m_speed = 0.f;
	m_comaPos = 0;
	m_destination = 0;
	m_lastRotationTime = -1;
	m_lastStatus = EReelStatus::eInitial;
	m_nowStatus = EReelStatus::eStoping;

	m_speedMax = (float)m_reelData.reelData[0].h * GetComaNum() * m_reelData.rpm / 60000.f;
	m_accVal = m_speedMax / m_reelData.accTime;
	return true;
}

bool CReel::Process(CSlotTimerManager& pTimer){

	// タイマー初期化
	if (m_nowStatus == EReelStatus::eAccerating && m_lastStatus == EReelStatus::eStoping){
		pTimer.DisableTimer(eTimerReelPush, m_reelData.reelID);
		pTimer.DisableTimer(eTimerReelStop, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelStart, m_reelData.reelID);
	}
	if (m_nowStatus == EReelStatus::eSliping && m_lastStatus == EReelStatus::eRotating){
		pTimer.DisableTimer(eTimerReelStopAvailable, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelPush, m_reelData.reelID);
	}
	/* 初回起動時にstopタイマを作動させる */
	if (m_lastStatus == EReelStatus::eInitial) {
		long long temp;
		if (!pTimer.GetTime(temp, eTimerReelStop, m_reelData.reelID))
			pTimer.SetTimer(eTimerReelStop, m_reelData.reelID);
	}

	if (m_nowStatus == EReelStatus::eAccerating){
		long long diff;
		if (!pTimer.GetTimeDiff(diff, eTimerReelStart, m_reelData.reelID)) return false;
		// 加速をより厳密に再現するために、1msずつ速度と位置を更新する
		// ただし最大速度に達した場合それ以上は掛け算で処理
		long long count;
		for (count = 0; count < diff; ++count){
			m_speed += m_accVal;
			if (m_speed > m_speedMax){
				long long temp;
				if(!pTimer.GetTime(temp, eTimerReelStopAvailable, m_reelData.reelID))
					pTimer.SetTimer(eTimerReelStopAvailable, m_reelData.reelID);
				m_speed = m_speedMax; m_nowStatus = EReelStatus::eRotating;
			}
			m_rotatePos -= m_speed;
		}
		m_rotatePos -= m_speed * (count-diff);
	} else if (m_nowStatus == EReelStatus::eRotating || m_nowStatus == EReelStatus::eSliping) {
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
	if (m_nowStatus == EReelStatus::eSliping && m_comaPos == m_destination && (newComaPos != m_destination || reelAdj)){
		m_rotatePos = (float)m_destination * m_reelData.reelData[0].h;
		m_speed = 0.f;	m_nowStatus = EReelStatus::eStoping;
		pTimer.DisableTimer(eTimerReelStart, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelStop, m_reelData.reelID);
	}
	m_comaPos = (unsigned int)std::floorf(m_rotatePos / m_reelData.reelData[0].h);
	m_lastStatus = m_nowStatus;
	if (!pTimer.GetTime(m_lastRotationTime, eTimerReelStart, m_reelData.reelID)) m_lastRotationTime = -1;
	return true;
}

bool CReel::ReelStart(){
	if (m_nowStatus != EReelStatus::eStoping) return false;
	m_nowStatus = EReelStatus::eAccerating;
	return true;
}

bool CReel::ReelStop(unsigned int pDest, bool pForceFlag){
	if (pForceFlag){
		// 即時停止
		m_destination = pDest % m_reelData.arrayData.size();
		m_comaPos = m_destination;
		m_speed = 0.f;
		m_pushPos = m_rotatePos;
		m_rotatePos = (float)m_reelData.reelData[0].h * pDest;
		m_nowStatus = EReelStatus::eStoping;
	} else {
		// 滑って停止する
		if (m_nowStatus != EReelStatus::eRotating) return false;
		m_pushPos = m_rotatePos;
		m_destination = pDest % m_reelData.arrayData.size();
		m_nowStatus = EReelStatus::eSliping;
	}
	return true;
}

int CReel::GetReelComaByReelPos(int pOffset) const{
	if (m_nowStatus != EReelStatus::eStoping) return -1;
	const int dst = (m_reelData.arrayData.size() + m_comaPos + pOffset) % m_reelData.arrayData.size();
	return m_reelData.arrayData[dst];
}

int CReel::GetReelComaByFixedPos(int pComa) const{
	if (m_nowStatus != EReelStatus::eStoping) return -1;
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

bool CReel::DrawReel(const CGameDataManage& pDataManager, IImageSourceManager* const pSrcData, CImageColorController& pColorData, const SReelDrawDataFromCSV pData) const {
	// src画像を仮描画スクリーンに描画する
	const unsigned int comaDivNum = m_reelData.arrayData.size();
	int destY = 0;
	int comaSizeY = 0;
	DxLib::SetDrawScreen(pData.preDrawScr);
	DxLib::ClearDrawScreen();								// 仮描画画面は自動クリアされる
	DxLib::SetDrawMode(DX_DRAWMODE_NEAREST);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);	// src画像を生成するため、固定(colorManagerの値は反映)
	
	const int comaBegin = (pData.comaBegin < 0 ? m_comaPos : pData.comaBegin) - pData.originComa;
	for (unsigned int i = 0; i < pData.comaNum; ++i) {
		// コマ番号を取得
		const unsigned int pos = (comaDivNum + comaBegin + i) % comaDivNum;
		const int comaNumber = m_reelData.arrayData[pos];
		const int comaColorID =
			i < pData.originComa ? 0 :
			i - pData.originComa >= pData.comaIndexMax ? pData.comaIndexMax : i - pData.originComa;

		// src定義を使用して仮描画
		auto srcData = pSrcData->GetImageSource(comaNumber, 0);
		for (int colorC = 0; ; ++colorC) {
			const auto colorPtr = pColorData.GetColorData(pSrcData->GetEffectDataName(), colorC);
			if (colorPtr == nullptr) break;
			if (colorPtr->GetColorData(pDataManager, srcData, comaColorID)) break;	// trueで正常なセット完了
		}
		DxLib::SetDrawBright(srcData.r, srcData.g, srcData.b);
		DxLib::DrawRectGraph(
			0, destY, srcData.x, srcData.y, srcData.w, srcData.h,
			pDataManager.GetDataHandle(srcData.imageID), TRUE
		);
		destY += srcData.h;
		if(i==0) comaSizeY = srcData.h; 
	}

	// 仮描画した画像を用いて本画面に画像を描画
	const int drawOffset = pData.comaBegin < 0 ?
		floor(m_rotatePos - comaSizeY * floor(m_rotatePos / comaSizeY)) : 0;
	int backTime = m_lastRotationTime;
	float blewOffset = 0.f;

	DxLib::SetDrawScreen(pData.destScr);
	DxLib::SetDrawMode(pData.extendModeID);
	DxLib::SetDrawBright(255, 255, 255);
	for (int blewCount = 0; blewCount <= pData.blew; ++blewCount) {
		for (int i = 0; i < pData.blewTime && blewCount>0; ++i) {
			if (--backTime < 0) break;
			if (backTime >= m_reelData.accTime) blewOffset += m_speedMax;
			else blewOffset += (m_speedMax - m_accVal * (m_reelData.accTime - backTime));
		}
		const float opacityRate = 1.f / pow(2.5f,blewCount);

		DxLib::SetDrawBlendMode(pData.blendModeID, static_cast<int>(pData.a * opacityRate));
		DxLib::DrawRectExtendGraph(
			pData.dstPos.x, pData.dstPos.y, pData.dstPos.x + pData.dstPos.w, pData.dstPos.y + pData.dstPos.h,
			pData.srcPos.x, pData.srcPos.y + drawOffset + blewOffset, pData.srcPos.w, pData.srcPos.h,
			pData.preDrawScr, TRUE
		);
		if (m_nowStatus == EReelStatus::eStoping) break;
	}
	return true;
}

// [act]各リールコマを16分割した位置を取得する
int CReel::GetReelDetailPos() const {
	if (m_nowStatus == EReelStatus::eAccerating || m_nowStatus == EReelStatus::eRotating) return -1;
	const int pos = (int)(m_pushPos * 16.f / m_reelData.reelData[0].h);
	return pos;
}

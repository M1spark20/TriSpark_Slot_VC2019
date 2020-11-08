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
	m_speed = 0.f;
	m_comaPos = 0;
	m_destination = 0;
	m_lastRotationTime = -1;
	m_lastStatus = eStoping;
	m_nowStatus = eStoping;

	m_speedMax = (float)m_reelData.reelData[0].h * GetComaNum() * m_reelData.rpm / 60000.f;
	m_accVal = m_speedMax / m_reelData.accTime;
	return true;
}

bool CReel::Process(CSlotTimerManager& pTimer){

	// �^�C�}�[������
	if (m_nowStatus == eAccerating && m_lastStatus == eStoping){
		pTimer.DisableTimer(eTimerReelStop, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelStart, m_reelData.reelID);
	}
	if (m_nowStatus == eSliping && m_lastStatus == eRotating){
		pTimer.DisableTimer(eTimerReelStopAvailable, m_reelData.reelID);
		pTimer.SetTimer(eTimerReelPush, m_reelData.reelID);
	}
	/* ����N������stop�^�C�}���쓮������ */{
		long long temp;
		if (!pTimer.GetTime(temp, eTimerReelStop, m_reelData.reelID))
			pTimer.SetTimer(eTimerReelStop, m_reelData.reelID);
	}

	if (m_nowStatus == eAccerating){
		long long diff;
		if (!pTimer.GetTimeDiff(diff, eTimerReelStart, m_reelData.reelID)) return false;
		// ��������茵���ɍČ����邽�߂ɁA1ms�����x�ƈʒu���X�V����
		// �������ő呬�x�ɒB�����ꍇ����ȏ�͊|���Z�ŏ���
		long long count;
		for (count = 0; count < diff; ++count){
			m_speed += m_accVal;
			if (m_speed > m_speedMax){
				long long temp;
				if(!pTimer.GetTime(temp, eTimerReelStopAvailable, m_reelData.reelID))
					pTimer.SetTimer(eTimerReelStopAvailable, m_reelData.reelID);
				m_speed = m_speedMax; m_nowStatus = eRotating;
			}
			m_rotatePos -= m_speed;
		}
		m_rotatePos -= m_speed * (count-diff);
	} else if (m_nowStatus == eRotating || m_nowStatus == eSliping) {
		long long diff;
		if (!pTimer.GetTimeDiff(diff, eTimerReelStart, m_reelData.reelID)) return false;
		m_rotatePos -= m_speed * diff;
	}

	bool reelAdj = false;	// ���[���␳�L��
	// ����]�ʒu�␳(�␳���̓��[�����~������)
	while (m_rotatePos < 0.f){
		reelAdj = true;
		m_rotatePos += GetComaNum() * m_reelData.reelData[0].h;
	}
	// �t��]�ʒu�␳(�␳���̓��[�����~������)
	while (m_rotatePos > GetComaNum() * m_reelData.reelData[0].h){
		reelAdj = true;
		m_rotatePos -= GetComaNum() * m_reelData.reelData[0].h;
	}

	// ���[���ʒu���f & ��~���f
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
	if (!pTimer.GetTime(m_lastRotationTime, eTimerReelStart, m_reelData.reelID)) m_lastRotationTime = -1;
	return true;
}

bool CReel::ReelStart(){
	if (m_nowStatus != eStoping) return false;
	m_nowStatus = eAccerating;
	return true;
}

bool CReel::ReelStop(unsigned int pDest, bool pForceFlag){
	if (pForceFlag){
		// ������~
		m_destination = pDest % m_reelData.arrayData.size();
		m_comaPos = m_destination;
		m_speed = 0.f;
		m_rotatePos = (float)m_reelData.reelData[0].h * pDest;
		m_nowStatus = eStoping;
	} else {
		// �����Ē�~����
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

bool CReel::DrawReel(const CGameDataManage& pDataManager, IImageSourceManager* const pSrcData, CImageColorController& pColorData, const SReelDrawDataFromCSV pData) const {
	// src�摜�����`��X�N���[���ɕ`�悷��
	const unsigned int comaDivNum = m_reelData.arrayData.size();
	int destY = 0;
	int comaSizeY = 0;
	DxLib::SetDrawScreen(pData.preDrawScr);
	DxLib::ClearDrawScreen();								// ���`���ʂ͎����N���A�����
	DxLib::SetDrawMode(DX_DRAWMODE_NEAREST);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);	// src�摜�𐶐����邽�߁A�Œ�(colorManager�̒l�͔��f)
	
	const int comaBegin = (pData.comaBegin < 0 ? m_comaPos : pData.comaBegin) - pData.originComa;
	for (unsigned int i = 0; i < pData.comaNum; ++i) {
		// �R�}�ԍ����擾
		const unsigned int pos = (comaDivNum + comaBegin + i) % comaDivNum;
		const int comaNumber = m_reelData.arrayData[pos];
		const int comaColorID =
			i < pData.originComa ? 0 :
			i - pData.originComa >= pData.comaIndexMax ? pData.comaIndexMax : i - pData.originComa;

		// src��`���g�p���ĉ��`��
		auto srcData = pSrcData->GetImageSource(comaNumber, 0);
		for (int colorC = 0; ; ++colorC) {
			const auto colorPtr = pColorData.GetColorData(pSrcData->GetEffectDataName(), colorC);
			if (colorPtr == nullptr) break;
			if (colorPtr->GetColorData(pDataManager, srcData, comaColorID)) break;	// true�Ő���ȃZ�b�g����
		}
		DxLib::SetDrawBright(srcData.r, srcData.g, srcData.b);
		DxLib::DrawRectGraph(
			0, destY, srcData.x, srcData.y, srcData.w, srcData.h,
			pDataManager.GetDataHandle(srcData.imageID), TRUE
		);
		destY += srcData.h;
		if(i==0) comaSizeY = srcData.h; 
	}

	// ���`�悵���摜��p���Ė{��ʂɉ摜��`��
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
		if (m_nowStatus == eStoping) break;
	}
	return true;
}
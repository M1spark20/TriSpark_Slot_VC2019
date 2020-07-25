#include "_header\CReelManager.hpp"
#include "_header\CReelDataReaderFromCSV.hpp"
#include "_header\CGameDataManage.h"
#include "_header\CGetSysDataFromCSV.hpp"
#include "_header\CSlotTimerManager.hpp"
#include "DxLib.h"

bool CReelManager::Init(const CGameDataManage& pDataManager, CGetSysDataFromCSV& pSysData){
	// m_reelChaData�Ǎ�
	CReelDataReaderFromCSV chaReader;
	chaReader.FileInit(pDataManager.GetDataHandle(pSysData.GetSysDataID("reelCha")));
	std::vector<SReelChaData> reelData;
	if(!chaReader.MakeData(reelData, pDataManager)) return false;

	m_drawingCanvas = DxLib::MakeScreen(reelData[0].reelData[0].w, reelData[0].reelData[0].h * 7, TRUE);

	// CReel�쐬
	for (auto it = reelData.begin(); it != reelData.end(); ++it){
		m_reelChaData.push_back(CReel());
		if (!(m_reelChaData.end()-1)->Init(*it)) return false;
	}

	return m_controller.Init(pDataManager, pSysData.GetSysDataID("control"), GetReelNum());
}

bool CReelManager::Process(CSlotTimerManager& pTimer){
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		it->Process(pTimer);
	}
	return true;
}

bool CReelManager::DrawReel(const CGameDataManage& pDataManager, const SReelDrawData pDrawData){
	if (pDrawData.comaNum >= 4) return false; // ���̐���
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pDrawData.reelID) continue;
		return it->DrawReel(pDataManager, pDrawData, m_drawingCanvas);
	}
	return false;
}

bool CReelManager::DrawReel(const CGameDataManage& pDataManager, const SReelDrawData pDrawData, unsigned int comaBegin){
	if (pDrawData.comaNum >= 4) return false; // ���̐���
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pDrawData.reelID) continue;
		return it->DrawReel(pDataManager, pDrawData, m_drawingCanvas, comaBegin);
	}
	return false;
}

int CReelManager::GetCharaNum(int pReelID) const {
	if (pReelID < 0 || (unsigned int)pReelID >= m_reelChaData.size()) return -1;
	return m_reelChaData[pReelID].GetComaNum();
}

// [act]�e���[���̂����AeRotating, eSliping�ƂȂ��Ă��郊�[�������擾����
//		����0->3�ƂȂ邱�Ƃŉ����̊����A3->0�ƂȂ邱�ƂŃ��[���̒�~���킩��
int CReelManager::GetRotatingReelNum(){
	int ans = 0;
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelStatus() == eRotating || it->GetReelStatus() == eSliping)
			++ans;
	}
	return ans;
}

std::vector<CReel>::const_iterator CReelManager::FindReelData(int pIndex) const {
	if (pIndex < 0 || (unsigned int)pIndex >= m_reelChaData.size())
		return m_reelChaData.cend();
	return m_reelChaData.cbegin() + pIndex;
}

bool CReelManager::StartReel(int pFlagID, int pBonusID){
	m_flagID = pFlagID;
	m_bonusID = pBonusID;
	m_controller.ResetReelData();
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (!it->ReelStart()) return false;
	}
	return true;
}

bool CReelManager::StopReel(int pStopReelID){
	if (pStopReelID < 0 || (unsigned int)pStopReelID >= m_reelChaData.size())return false;
	// ���[�������łɒ�~������s���Ă���ꍇ��true��Ԃ�
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pStopReelID) continue;
		if (it->GetReelStatus() != eRotating) return true;
		const int dest = m_controller.GetStopPosition(m_flagID, m_bonusID, pStopReelID, it->GetReelPos());
		return it->ReelStop(dest, false);
	}
	return false;
}

bool CReelManager::StopReel(int pStopReelID, int pForceStopComa){
	if (pStopReelID < 0 || (unsigned int)pStopReelID >= m_reelChaData.size()) return false;
	// ���[�������łɒ�~������s���Ă���ꍇ��true��Ԃ�
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pStopReelID) continue;
		//if (it->GetReelStatus() != eRotating) return true;
		const int dest = m_controller.GetStopPosition(m_flagID, m_bonusID, pStopReelID, pForceStopComa);
		return it->ReelStop(dest, true);
	}
	return false;
}

// [act]�w��ID�̃��[���̈ʒu��Ԃ��B�S��ԂŌ���̃��[���̈ʒu���Ԃ��Ă���
int CReelManager::GetReelPos(int pCheckReelID) const {
	if (pCheckReelID < 0 || (unsigned int)pCheckReelID >= m_reelChaData.size()) return false;
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pCheckReelID) continue;
		return it->GetReelPos();
	}
	return -1;
}

// [act]�S���[���̏����܂Ƃ߂ĕԂ�
std::vector<int> CReelManager::GetReelPos() const {
	std::vector<int> ans(m_reelChaData.size());
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		ans.at(it->GetReelID()) = GetReelPos(it->GetReelID());
	}
	return ans;
}

// [act]���[����charaID��Ԃ�
// [ret]-1			: �G���[����(notFound or ���~)
//		(else)		: ���[����charaID
// [prm]pComaOffset	: �{���ʒu�I�t�Z�b�g
int CReelManager::GetCharaIDByReelPos(int pReelID, int pComaOffset) const {
	const auto dataPtr = FindReelData(pReelID);
	if (dataPtr == m_reelChaData.end()) return -1;
	return dataPtr->GetReelComaByReelPos(pComaOffset);
}

// [act]���[���̎w��ʒu��charaID��Ԃ�
// [ret]-1			: �G���[����(notFound or ���~)
//		(else)		: ���[����charaID
// [prm]pComaOffset	: �{���ʒu�R�}ID(���ڐݒ�)
int CReelManager::GetCharaIDByFixedPos(int pReelID, int pComaID) const {
	const auto dataPtr = FindReelData(pReelID);
	if (dataPtr == m_reelChaData.end()) return -1;
	return dataPtr->GetReelComaByFixedPos(pComaID);
}

// [act]�S���[���̎w��ʒu��charaID��Ԃ�
// [ret](empty)		: �G���[����
//		(else)		: �e���[����charaID(-1�͊e���[���ł̃G���[or��]��)
// [prm]pOffsetList	: �{���ʒu�I�t�Z�b�g�ꗗ�B���[�����Ɠ���̗v�f�����Ȃ��ƃG���[�Ƃ���
std::vector<int> CReelManager::GetCharaIDList(std::vector<int> pOffsetList) const{
	if (pOffsetList.size() != GetReelNum()) return std::vector<int>(0);
	std::vector<int> ans(GetReelNum());
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		ans.at(it->GetReelID()) = GetCharaIDByReelPos(it->GetReelID(), pOffsetList[it->GetReelID()]);
	}
	return ans;
}

CReelManager::~CReelManager(){
	DxLib::DeleteGraph(m_drawingCanvas);
}
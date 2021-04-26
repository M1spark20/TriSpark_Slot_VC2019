#include "_header/CReelManager.hpp"
#include "_header/CReelDataReaderFromCSV.hpp"
#include "_header/CGameDataManage.h"
#include "_header/CGetSysDataFromCSV.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/ImageSourceManager.hpp"
#include "_header/CImageColorManager.hpp"
#include "_header/CRestoreManager.hpp"
#include "DxLib.h"

bool CReelManager::Init(const CGameDataManage& pDataManager, CGetSysDataFromCSV& pSysData){
	// m_reelChaData読込
	CReelDataReaderFromCSV chaReader;
	chaReader.FileInit(pDataManager.GetDataHandle(pSysData.GetSysDataID("reelCha")));
	std::vector<SReelChaData> reelData;
	if(!chaReader.MakeData(reelData, pDataManager)) return false;

	m_drawingCanvas = DxLib::MakeScreen(reelData[0].reelData[0].w, reelData[0].reelData[0].h * 7, TRUE);

	// CReel作成
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
	if (pDrawData.comaNum >= 4) return false; // 仮の制約
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pDrawData.reelID) continue;
		return it->DrawReel(pDataManager, pDrawData, m_drawingCanvas);
	}
	return false;
}

bool CReelManager::DrawReel(const CGameDataManage& pDataManager, const SReelDrawData pDrawData, unsigned int comaBegin){
	if (pDrawData.comaNum >= 4) return false; // 仮の制約
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pDrawData.reelID) continue;
		return it->DrawReel(pDataManager, pDrawData, m_drawingCanvas, comaBegin);
	}
	return false;
}

bool CReelManager::DrawReel(const CGameDataManage& pDataManager, IImageSourceManager* const pSrcData, CImageColorController& pColorData, const SReelDrawDataFromCSV pData) const{
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pData.reelID) continue;
		return it->DrawReel(pDataManager, pSrcData, pColorData, pData);
	}
	return false;
}

int CReelManager::GetCharaNum(int pReelID) const {
	if (pReelID < 0 || (unsigned int)pReelID >= m_reelChaData.size()) return -1;
	return m_reelChaData[pReelID].GetComaNum();
}

// [act]各リールのうち、eRotating, eSlipingとなっているリール数を取得する
//		数が0->3となることで加速の完了、3->0となることでリールの停止がわかる
int CReelManager::GetRotatingReelNum(){
	int ans = 0;
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelStatus() == EReelStatus::eRotating || it->GetReelStatus() == EReelStatus::eSliping)
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
	// リールがすでに停止制御を行っている場合はtrueを返す
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pStopReelID) continue;
		if (it->GetReelStatus() != EReelStatus::eRotating) return true;
		const int dest = m_controller.GetStopPosition(m_flagID, m_bonusID, pStopReelID, it->GetReelPos());
		return it->ReelStop(dest, false);
	}
	return false;
}

bool CReelManager::StopReel(int pStopReelID, int pForceStopComa){
	if (pStopReelID < 0 || (unsigned int)pStopReelID >= m_reelChaData.size()) return false;
	// リールがすでに停止制御を行っている場合はtrueを返す
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pStopReelID) continue;
		//if (it->GetReelStatus() != eRotating) return true;
		const int dest = m_controller.GetStopPosition(m_flagID, m_bonusID, pStopReelID, pForceStopComa);
		return it->ReelStop(dest, true);
	}
	return false;
}

// [act]指定IDのリールの位置を返す。全状態で現状のリールの位置が返ってくる
int CReelManager::GetReelPos(int pCheckReelID) const {
	if (pCheckReelID < 0 || (unsigned int)pCheckReelID >= m_reelChaData.size()) return false;
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		if (it->GetReelID() != pCheckReelID) continue;
		return it->GetReelPos();
	}
	return -1;
}

// [act]全リールの情報をまとめて返す
std::vector<int> CReelManager::GetReelPos() const {
	std::vector<int> ans(m_reelChaData.size());
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		ans.at(it->GetReelID()) = GetReelPos(it->GetReelID());
	}
	return ans;
}

// [act]リールのcharaIDを返す
// [ret]-1			: エラー発生(notFound or 非停止)
//		(else)		: リールのcharaID
// [prm]pComaOffset	: 閲覧位置オフセット
int CReelManager::GetCharaIDByReelPos(int pReelID, int pComaOffset) const {
	const auto dataPtr = FindReelData(pReelID);
	if (dataPtr == m_reelChaData.end()) return -1;
	return dataPtr->GetReelComaByReelPos(pComaOffset);
}

// [act]リールの指定位置のcharaIDを返す
// [ret]-1			: エラー発生(notFound or 非停止)
//		(else)		: リールのcharaID
// [prm]pComaOffset	: 閲覧位置コマID(直接設定)
int CReelManager::GetCharaIDByFixedPos(int pReelID, int pComaID) const {
	const auto dataPtr = FindReelData(pReelID);
	if (dataPtr == m_reelChaData.end()) return -1;
	return dataPtr->GetReelComaByFixedPos(pComaID);
}

// [act]全リールの指定位置のcharaIDを返す
// [ret](empty)		: エラー発生
//		(else)		: 各リールのcharaID(-1は各リールでのエラーor回転中)
// [prm]pOffsetList	: 閲覧位置オフセット一覧。リール数と同一の要素数がないとエラーとする
std::vector<int> CReelManager::GetCharaIDList(std::vector<int> pOffsetList) const{
	if (pOffsetList.size() != GetReelNum()) return std::vector<int>(0);
	std::vector<int> ans(GetReelNum());
	for (auto it = m_reelChaData.begin(); it != m_reelChaData.end(); ++it){
		ans.at(it->GetReelID()) = GetCharaIDByReelPos(it->GetReelID(), pOffsetList[it->GetReelID()]);
	}
	return ans;
}

// [act]全リールの指定位置のcharaIDを返す
// [ret]-1		: エラー発生
//		(else)	: 各リールの位置(各コマを16分割した際の値)
int CReelManager::GetComaDetailPos(int pReelID) const {
	const auto dataPtr = FindReelData(pReelID);
	if (dataPtr == m_reelChaData.end()) return -1;
	return dataPtr->GetReelDetailPos();
}


bool CReelManager::ReadRestore(CRestoreManagerRead& pReader) {
	int reelCount = 0;
	if (!pReader.ReadNum(reelCount)) return false;
	for (int i = 0; i < reelCount; ++i) {
		int setPos = 0;
		if (!pReader.ReadNum(setPos)) return false;
		m_reelChaData[i].ReelStop(setPos, true);
	}
	return true;
}

bool CReelManager::WriteRestore(CRestoreManagerWrite& pWriter) const {
	if (!pWriter.WriteNum((int)GetReelNum())) return false;
	for (int i = 0; i < GetReelNum(); ++i) {
		if (!pWriter.WriteNum((int)GetReelPos(i))) return false;
	}
	return true;
}

CReelManager::~CReelManager(){
	DxLib::DeleteGraph(m_drawingCanvas);
}

#pragma once
#include "SReelChaData.hpp"
#include "CReel.hpp"
#include "CReelController.hpp"
#include "CReelHistoryManager.hpp"
class CGameDataManage;
class CGetSysDataFromCSV;
class CSlotTimerManager;
class IImageSourceManager;
class CImageColorManager;
class CRestoreManagerRead;
class CRestoreManagerWrite;

class CReelManager {
	std::vector<CReel>	m_reelChaData;	// 各リールデータ
	int					m_drawingCanvas;
	CReelController		m_controller;
	int					m_flagID;
	int					m_bonusID;
	CReelHistoryManager	m_historyManager;
	SReelHistoryData	m_nowHistoryData;

	std::vector<CReel>::const_iterator FindReelData(int pIndex) const;

public:
	bool Init(const CGameDataManage& pDataManager, CGetSysDataFromCSV& pSysData);
	bool Process(CSlotTimerManager& pTimer);
	bool DrawReel(const CGameDataManage& pDataManager, const SReelDrawData pDrawData);
	bool DrawReel(const CGameDataManage& pDataManager, const SReelDrawData pDrawData, unsigned int comaBegin);
	bool DrawReel(const CGameDataManage& pDataManager, IImageSourceManager* const pSrcData, CImageColorController& pColorData, const SReelDrawDataFromCSV pData) const;

	int GetReelNum() const { return m_reelChaData.size(); }
	int GetCharaNum(int pReelID = 0) const;
	int GetRotatingReelNum();

	bool				StartReel(int pFlagID, int pBonusID, int pBetNum);
	bool				StopReel(int pStopReelID);
	bool				StopReel(int pStopReelID, int pForceStopComa);
	int					GetReelPos(int pCheckReelID) const;
	std::vector<int>	GetReelPos() const;
	int					GetCharaIDByReelPos(int pReelID, int pComaOffset) const;
	int					GetCharaIDByFixedPos(int pReelID, int pComaID) const;
	int					GetComaDetailPos(int pReelID) const;
	std::vector<int>	GetCharaIDList(std::vector<int> pOffsetList) const;

	void				SetHistoryData();
	SReelHistoryData	GetHistoryData() const { return m_nowHistoryData; }
	int					GetHistoryNum() const { return m_historyManager.GetHistorySize(); }
	SReelHistoryData	GetHistoryData(int pIndex) const { return m_historyManager.GetData(pIndex); }

	bool				ReadRestore(CRestoreManagerRead& pReader);
	bool				WriteRestore(CRestoreManagerWrite& pWriter) const;
	~CReelManager();
};

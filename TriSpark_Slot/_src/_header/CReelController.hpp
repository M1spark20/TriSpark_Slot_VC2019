#pragma once
#include "SReelControlData.hpp"
#include "EOrderID.hpp"
#include <array>
#include <vector>
class CGameDataManage;

class CReelController{
	SReelControlData				m_ctrlData;		// 制御データ
	EOrderID						m_orderID;		// 停止順識別データ
	int								m_reelNum;		// リール数情報
	std::vector<std::array<char,2>>	m_stopReelNum;	// 過去停止情報[push,stop]

public:
	bool Init(const CGameDataManage& pDataManager, int pFileID, int reelNum);
	int GetStopPosition(int pFlagID, int pBonusID, int pPushReel, int pPushComaID);
	void ResetReelData();
	int GetStopPos(int pPushReel, int pPushPos, int pCharaMax);
	bool SetTableNo(int pFlagID, int pBonusID, std::vector<int>& pOrder, std::vector<int>& pStoppedPos, int pPushReel);
};

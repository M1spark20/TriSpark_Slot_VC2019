#pragma once
#include "SCastData.hpp"
#include <vector>
class CGameDataManage;
struct SSlotGameDataWrapper;
class CSlotInternalDataManager;

class CSlotCastChecker {
	SCastData						m_castData;
	unsigned int					m_checkedBet;
	std::vector<const SPayData*>	m_castList;

public:
	bool	Init(CGameDataManage& pData, int pFileID);
	bool	SetCastData(const SSlotGameDataWrapper& pData);
	void	ResetCastData();
	bool	SetGameMode(CSlotInternalDataManager& pDataManager);
	bool	IsReplay();
	int		GetPayout();
};
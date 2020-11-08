#include "_header/CSlotEffectManager.hpp"
#include "_header/CReadEffectDataFromCSV.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CReelManager.hpp"
#include "_header/CGameDataManage.h"
#include "_header/CSlotInternalDataManager.hpp"

bool CSlotEffectManager::Init(CGameDataManage& pGameData, int pFileID, CSlotTimerManager& pTimer, CReelManager& pReel) {
	if(!mVariableManager.Init()) return false;

	CReadEffectDataFromCSV reader;
	if (!reader.FileInit(pGameData.GetDataHandle(pFileID))) return false;
	if (!reader.MakeData(mEffectData, mVariableManager, pTimer, pReel)) return false;
	return true;
}

bool CSlotEffectManager::Process(CSlotTimerManager& pTimer, CSlotInternalDataManager& pInternal) {
	mVariableManager.Process(pInternal);

	for (auto& condition : mEffectData.conditionData) {
		if(!condition.SetCondition(mVariableManager, pTimer)) return false;
	}

	for (auto it = mEffectData.imgData.begin(); it != mEffectData.imgData.end(); ++it) {
		if (!it->second.pSource->SetTimer(pTimer)) return false;
		if (!it->second.pDest->SetTimer(pTimer)) return false;
	}
	return mEffectData.colorController.SetTimerAll(pTimer);
}

bool CSlotEffectManager::Draw(CGameDataManage& pGameData) {
	for (int orderC = 0; ; ++orderC) {
		if (orderC < 0 || orderC >= mEffectData.conditionData.size()) return false;
		if (!mEffectData.conditionData[orderC].GetCondition()) continue;
		bool isProceed = false;
		for (auto it = mEffectData.imgData.begin(); it != mEffectData.imgData.end(); ++it) {
			if (it->first != orderC) continue;
			it->second.pDest->Draw(&(*it->second.pSource), mEffectData.colorController, pGameData);
			it->second.pSource->ResetTimer();
			it->second.pDest->ResetTimer();
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.clearScreenData.begin(); it != mEffectData.clearScreenData.end(); ++it) {
			if (it->first != orderC) continue;
			mVariableManager.ClearScreen(it->second);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		return false;
	}

	mEffectData.colorController.ResetTimerAll();
	return true;
}
#include "_header/CSlotEffectManager.hpp"
#include "_header/CReadEffectDataFromCSV.hpp"
#include "_header/CGameDataManage.h"
#include "_header/SSlotGameDataWrapper.hpp"
#include "_header/ErrClass.hpp"

bool CSlotEffectManager::Init(CGameDataManage& pGameData, int pFileID, CSlotTimerManager& pTimer, CReelManager& pReel) {
	if(!mVariableManager.Init()) return false;

	CReadEffectDataFromCSV reader;
	if (!reader.FileInit(pGameData.GetDataHandle(pFileID))) return false;
	if (!reader.MakeData(mEffectData, mVariableManager, pTimer, pReel)) return false;
	return true;
}

bool CSlotEffectManager::Process(CSlotTimerManager& pTimer, CSlotInternalDataManager& pInternal, const SSlotGameDataWrapper& pWrapper) {
	mVariableManager.Process(pInternal, pWrapper);

	for (auto it = mEffectData.imgData.begin(); it != mEffectData.imgData.end(); ++it) {
		if (!it->second.pSource->SetTimer(pTimer)) return false;
		if (!it->second.pDest->SetTimer(pTimer)) return false;
	}
	return mEffectData.colorController.SetTimerAll(pTimer);
}

bool CSlotEffectManager::Draw(CGameDataManage& pGameData, CSlotTimerManager& pTimer) {
	for (int orderC = 0; ; ++orderC) {
		if (orderC < 0 || orderC >= mEffectData.conditionData.size()) return false;
		if(!mEffectData.conditionData[orderC].SetCondition(mVariableManager, pTimer)) return false;

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
		for (auto it = mEffectData.colorApplyData.begin(); it != mEffectData.colorApplyData.end(); ++it) {
			if (it->first != orderC) continue;
			mEffectData.colorController.AddActionData(it->second);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.varInsertData.begin(); it != mEffectData.varInsertData.end(); ++it) {
			if (it->first != orderC) continue;
			mVariableManager.InsertVar(it->second);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.varRoleData.begin(); it != mEffectData.varRoleData.end(); ++it) {
			if (it->first != orderC) continue;
			mVariableManager.RoleVar(it->second);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.soundRingData.begin(); it != mEffectData.soundRingData.end(); ++it) {
			if (it->first != orderC) continue;
			mSoundManager.DeclarePlaySound(it->second, mVariableManager);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.soundVolData.begin(); it != mEffectData.soundVolData.end(); ++it) {
			if (it->first != orderC) continue;
			mSoundManager.DeclareVolumeAction(it->second, mVariableManager);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.soundStopData.begin(); it != mEffectData.soundStopData.end(); ++it) {
			if (it->first != orderC) continue;
			mSoundManager.DeclareStopAction(it->second, mVariableManager);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.timerActionData.begin(); it != mEffectData.timerActionData.end(); ++it) {
			if (it->first != orderC) continue;
			pTimer.SetTimerFromTimerHandle(it->second, mVariableManager);
			isProceed = true;	 break;
		}
		if (isProceed) continue;
		for (auto it = mEffectData.timerStopData.begin(); it != mEffectData.timerStopData.end(); ++it) {
			if (it->first != orderC) continue;
			pTimer.ResetTimerFromTimerHandle(it->second, mVariableManager);
			isProceed = true;	 break;
		}
		if (isProceed) continue;

		return false;
	}

	mEffectData.colorController.ResetTimerAll();
	return true;
}

bool CSlotEffectManager::RingSound(CSlotTimerManager& pTimer, CGameDataManage& pGameData) {
	return mSoundManager.RingAction(pTimer, pGameData);
}

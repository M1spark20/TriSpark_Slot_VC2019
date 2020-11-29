#pragma once
#include "IEffectExecuteMakerBase.hpp"
#include <string>
#include <list>
#include <vector>
class CSlotTimerManager;
class CEffectVariableManager;
class CGameDataManage;

struct SSlotSoundRingData {		// intデータはすべて変数ID
	int dataID;
	std::string timerName;
	int beginTime;
	int stopTime;
	int loopStartTime;
	bool isLoop;
};

struct SSlotSoundVolumeData {	// intデータはすべて変数ID
	int dataID;
	std::string timerName;
	int applyTime;
	int volume;
	int pan;
};

typedef int	SSlotSoundStopData;	// データIDを指す変数ID

class CSlotSoundManager {
	std::list<std::pair<SSlotSoundRingData, bool>>	mRingData;
	std::list<SSlotSoundVolumeData>					mVolumeData;
	std::vector<SSlotSoundStopData>					mStopData;

public:
	void DeclarePlaySound(const SSlotSoundRingData& pData);
	void DeclareVolumeAction(const SSlotSoundVolumeData& pData);
	void DeclareStopAction(const SSlotSoundStopData& pData);
	bool RingAction(CSlotTimerManager& pTimer, const CEffectVariableManager& pVar, const CGameDataManage& pDataManager);
};

class CSlotSoundRingDataMaker : public IEffectExecuteMakerBase {
	SSlotSoundRingData mRingData;
	SSlotSoundStopData mStopData;
public:
	bool MakeRingData(std::vector<std::string> pData, CEffectVariableManager& pVar);
	bool MakeStopData(std::vector<std::string> pData, CEffectVariableManager& pVar);
	SSlotSoundRingData ExtractRing() { return mRingData; }
	SSlotSoundStopData ExtractStop() { return mStopData; }
};

class CSlotSoundVolumeDataMaker : public IEffectExecuteMakerBase {
	SSlotSoundVolumeData mData;
public:
	bool MakeData(std::vector<std::string> pData, CEffectVariableManager& pVar);
	SSlotSoundVolumeData Extract() { return mData; }
};
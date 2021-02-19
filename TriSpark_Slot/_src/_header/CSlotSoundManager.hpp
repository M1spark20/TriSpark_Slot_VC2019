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
	// 鳴動データとして保存するとき、変数IDを値に直してから保存する(後から変換すると定義後の変数代入に対応できない)
	std::list<std::pair<SSlotSoundRingData, bool>>	mRingData;
	std::list<SSlotSoundVolumeData>					mVolumeData;
	std::vector<SSlotSoundStopData>					mStopData;

public:
	bool DeclarePlaySound(const SSlotSoundRingData& pData, const CEffectVariableManager& pVar);
	bool DeclareVolumeAction(const SSlotSoundVolumeData& pData, const CEffectVariableManager& pVar);
	bool DeclareStopAction(const SSlotSoundStopData& pData, const CEffectVariableManager& pVar);
	bool RingAction(CSlotTimerManager& pTimer, const CGameDataManage& pDataManager);
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

#pragma once
#include "IEffectExecuteMakerBase.hpp"
#include <string>
#include <list>
#include <vector>
class CSlotTimerManager;
class CEffectVariableManager;
class CGameDataManage;

struct SSlotSoundRingData {		// int�f�[�^�͂��ׂĕϐ�ID
	int dataID;
	std::string timerName;
	int beginTime;
	int stopTime;
	int loopStartTime;
	bool isLoop;
};

struct SSlotSoundVolumeData {	// int�f�[�^�͂��ׂĕϐ�ID
	int dataID;
	std::string timerName;
	int applyTime;
	int volume;
	int pan;
};

typedef int	SSlotSoundStopData;	// �f�[�^ID���w���ϐ�ID

class CSlotSoundManager {
	// ���f�[�^�Ƃ��ĕۑ�����Ƃ��A�ϐ�ID��l�ɒ����Ă���ۑ�����(�ォ��ϊ�����ƒ�`��̕ϐ�����ɑΉ��ł��Ȃ�)
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
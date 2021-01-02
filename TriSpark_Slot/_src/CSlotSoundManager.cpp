#include "_header/CSlotSoundManager.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CGameDataManage.h"
#include "_header/ErrClass.hpp"
#include "DxLib.h"
#include <stdexcept>

bool CSlotSoundManager::DeclarePlaySound(const SSlotSoundRingData& pData, const CEffectVariableManager& pVar) {
	// �ϐ�ID �� �����l�ϊ����s���Ă���ۑ�
	try {
		SSlotSoundRingData convData;
		convData.dataID = pVar.GetVal(pData.dataID);
		convData.timerName = pData.timerName;
		convData.beginTime = pVar.GetVal(pData.beginTime);
		convData.stopTime = pVar.GetVal(pData.stopTime);
		convData.loopStartTime = pVar.GetVal(pData.loopStartTime);
		convData.isLoop = pData.isLoop;
		mRingData.push_back(std::pair<SSlotSoundRingData, bool>(convData, false));
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}

}

bool CSlotSoundManager::DeclareVolumeAction(const SSlotSoundVolumeData& pData, const CEffectVariableManager& pVar) {
	// �ϐ�ID �� �����l�ϊ����s���Ă���ۑ�
	try {
		SSlotSoundVolumeData convData;
		convData.dataID = pVar.GetVal(pData.dataID);
		convData.timerName = pData.timerName;
		convData.applyTime = pVar.GetVal(pData.applyTime);
		convData.volume = pVar.GetVal(pData.volume);
		convData.pan = pVar.GetVal(pData.pan);
		mVolumeData.push_back(convData);
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}

bool CSlotSoundManager::DeclareStopAction(const SSlotSoundStopData& pData, const CEffectVariableManager& pVar) {
	// �ϐ�ID �� �����l�ϊ����s���Ă���ۑ�
	try {
		mStopData.push_back(pVar.GetVal(pData));
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}

bool CSlotSoundManager::RingAction(CSlotTimerManager& pTimer, const CGameDataManage& pDataManager) {
	try {
		// stopData
		for (auto stopID : mStopData) {
			const int soundDataID = stopID;
			const int dataHandle = pDataManager.GetDataHandle(soundDataID);
			const auto ringStatus = DxLib::CheckSoundMem(dataHandle);
			if (ringStatus == -1) {	// error
				DxLib::ErrorLogFmtAdd("Sound Error - DataID: %d / handle: %d", soundDataID, dataHandle);
				return false;
			}
			DxLib::StopSoundMem(dataHandle);
		}
		mStopData.clear();

		// ringData
		bool isDelete = false;
		for (auto it = mRingData.begin(); it != mRingData.end();) {
			isDelete = false;
			const auto timerHandle = pTimer.GetTimerHandle(it->first.timerName);
			long long timerValue;
			const bool timerEnable = pTimer.GetTimeFromTimerHandle(timerValue, timerHandle);

			const int soundDataID = it->first.dataID;
			const int dataHandle = pDataManager.GetDataHandle(soundDataID);
			const auto ringStatus = DxLib::CheckSoundMem(dataHandle);
			if (ringStatus == -1) {	// error
				DxLib::ErrorLogFmtAdd("Sound Error - DataID: %d / handle: %d", soundDataID, dataHandle);
				return false;
			}

			if (it->second) {	// ���Y��`�ł��łɉ���炵���ꍇ
				if (ringStatus == 0) {	// not playing
					// ��`���X�e�[�W���珜������
					it = mRingData.erase(it);	isDelete = true;
				} else {
					// �I������
					if (timerEnable) {
						// �J�n���ԂƏI�����Ԃ������ꍇ�͖���ςȂ�
						if (it->first.beginTime == it->first.stopTime) { ++it; continue; }
						if (timerValue < it->first.stopTime) { ++it; continue; }
					}
					DxLib::StopSoundMem(dataHandle);
					// ��`���X�e�[�W���珜������
					it = mRingData.erase(it);	isDelete = true;
				}
			} else {			// ���Y��`�ł܂�����炵�Ă��Ȃ��ꍇ
				// �J�n����
				if (!timerEnable) { ++it; continue; }
				if (timerValue < it->first.beginTime) { ++it; continue; }
				DxLib::StopSoundMem(dataHandle);
				if (it->first.isLoop) DxLib::SetLoopPosSoundMem(it->first.loopStartTime, dataHandle);
				DxLib::PlaySoundMem(dataHandle, it->first.isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
				it->second = true;
			}
			if (!isDelete) ++it;	// List�v�f���������Ȃ������ꍇ�̂݃C�e���[�^�𓮂���
		}

		// volumeData
		for (auto it = mVolumeData.cbegin(); it != mVolumeData.cend();) {
			const auto timerHandle = pTimer.GetTimerHandle(it->timerName);
			long long timerValue;
			if (!pTimer.GetTimeFromTimerHandle(timerValue, timerHandle)) { ++it; continue; }
			if (timerValue < it->applyTime) { ++it; continue; }

			const int soundDataID = it->dataID;
			const int dataHandle = pDataManager.GetDataHandle(soundDataID);
			const auto ringStatus = DxLib::CheckSoundMem(dataHandle);
			if (ringStatus == -1) {	// error
				DxLib::ErrorLogFmtAdd("Sound Error - DataID: %d / handle: %d", soundDataID, dataHandle);
				return false;
			}
			DxLib::SetPanSoundMem(it->pan, dataHandle);
			DxLib::SetVolumeSoundMem(it->volume, dataHandle);
			it = mVolumeData.erase(it);
		}

		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}


bool CSlotSoundRingDataMaker::MakeRingData(std::vector<std::string> pData, CEffectVariableManager& pVar) {
	try {
		mRingData.dataID = GetVariableID(pData.at(1), false, pVar);
		mRingData.timerName = pData.at(2);
		mRingData.beginTime = GetVariableID(pData.at(3), false, pVar);
		mRingData.stopTime = GetVariableID(pData.at(4), false, pVar);
		mRingData.isLoop = pData.at(5) == "T";
		if(mRingData.isLoop)	mRingData.loopStartTime = GetVariableID(pData.at(6), false, pVar);
		else					mRingData.loopStartTime = GetVariableID("0", false, pVar);
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	catch (std::out_of_range e) {
		return false;
	}
	catch (std::invalid_argument e) {
		return false;
	}
}

bool CSlotSoundRingDataMaker::MakeStopData(std::vector<std::string> pData, CEffectVariableManager& pVar) {
	try {
		mStopData = GetVariableID(pData.at(1), false, pVar);
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	catch (std::out_of_range e) {
		return false;
	}
	catch (std::invalid_argument e) {
		return false;
	}
}


bool CSlotSoundVolumeDataMaker::MakeData(std::vector<std::string> pData, CEffectVariableManager& pVar) {
	try {
		mData.dataID = GetVariableID(pData.at(1), false, pVar);
		mData.timerName = pData.at(2);
		mData.applyTime = GetVariableID(pData.at(3), false, pVar);
		mData.volume = GetVariableID(pData.at(4), false, pVar);
		mData.pan = GetVariableID(pData.at(4), false, pVar);
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	catch (std::out_of_range e) {
		return false;
	}
	catch (std::invalid_argument e) {
		return false;
	}
}
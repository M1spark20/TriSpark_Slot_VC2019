#include "_header/CSlotSoundManager.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CGameDataManage.h"
#include "_header/ErrClass.hpp"
#include "DxLib.h"
#include <stdexcept>

void CSlotSoundManager::DeclarePlaySound(const SSlotSoundRingData& pData) {
	mRingData.push_back(std::pair<SSlotSoundRingData, bool>(pData, false));
}

void CSlotSoundManager::DeclareVolumeAction(const SSlotSoundVolumeData& pData) {
	mVolumeData.push_back(pData);
}

void CSlotSoundManager::DeclareStopAction(const SSlotSoundStopData& pData) {
	mStopData.push_back(pData);
}

bool CSlotSoundManager::RingAction(CSlotTimerManager& pTimer, const CEffectVariableManager& pVar, const CGameDataManage& pDataManager) {
	try {
		// stopData
		for (auto stopID : mStopData) {
			const int soundDataID = pVar.GetVal(stopID);
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

			const int soundDataID = pVar.GetVal(it->first.dataID);
			const int dataHandle = pDataManager.GetDataHandle(soundDataID);
			const auto ringStatus = DxLib::CheckSoundMem(dataHandle);
			if (ringStatus == -1) {	// error
				DxLib::ErrorLogFmtAdd("Sound Error - DataID: %d / handle: %d", soundDataID, dataHandle);
				return false;
			}

			if (ringStatus == 0) {	// not playing
				// すでに開始させた音声データの再生が止まっていればデータを削除する
				if (it->second) {
					it = mRingData.erase(it);	isDelete = true;
				}
				else {
					// 開始判定
					if (!timerEnable) { ++it; continue; }
					if (timerValue < pVar.GetVal(it->first.beginTime)) { ++it; continue; }
					if (it->first.isLoop) DxLib::SetLoopPosSoundMem(pVar.GetVal(it->first.loopStartTime), dataHandle);
					DxLib::PlaySoundMem(dataHandle, it->first.isLoop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
					it->second = true;
				}
			}
			else {	// playing
				if (timerEnable)
					if (timerValue < pVar.GetVal(it->first.stopTime)) { ++it; continue; }
				DxLib::StopSoundMem(dataHandle);
				// 時間経過で音声を止める場合はデータを即時消去
				it = mRingData.erase(it);	isDelete = true;
			}
			if (!isDelete) ++it;	// List要素を消去しなかった場合のみイテレータを動かす
		}

		// volumeData
		for (auto it = mVolumeData.cbegin(); it != mVolumeData.cend();) {
			const auto timerHandle = pTimer.GetTimerHandle(it->timerName);
			long long timerValue;
			if (!pTimer.GetTimeFromTimerHandle(timerValue, timerHandle)) { ++it; continue; }
			if (timerValue < pVar.GetVal(it->applyTime)) { ++it; continue; }

			const int soundDataID = pVar.GetVal(it->dataID);
			const int dataHandle = pDataManager.GetDataHandle(soundDataID);
			const auto ringStatus = DxLib::CheckSoundMem(dataHandle);
			if (ringStatus == -1) {	// error
				DxLib::ErrorLogFmtAdd("Sound Error - DataID: %d / handle: %d", soundDataID, dataHandle);
				return false;
			}
			DxLib::SetPanSoundMem(pVar.GetVal(it->pan), dataHandle);
			DxLib::SetVolumeSoundMem(pVar.GetVal(it->volume), dataHandle);
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
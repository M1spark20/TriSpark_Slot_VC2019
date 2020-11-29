#include "_header/CReadEffectDataFromCSV.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/CReelManager.hpp"
#include "_header/ErrClass.hpp"
#include "DxLib.h"
#include <stdexcept>

bool CReadEffectDataFromCSV::FileInit(int pFileID) {
	// [act]DxLib側で開いたファイルからデータを読み出す
	// [ret]ファイルオープンに成功したかどうか
	while (!DxLib::FileRead_eof(pFileID)) {
		TCHAR str[1024];
		DxLib::FileRead_gets(str, 1024, pFileID);
		m_ReadDataAll.append(str).append("\n");
	}
	return !m_ReadDataAll.empty();
}

void CReadEffectDataFromCSV::PushImgData(SSlotEffectData& pData, std::unique_ptr<IImageSourceManager>& sourcePtr, std::unique_ptr<IImageDestinationManager>& destPtr) {
	pData.conditionData.push_back(mConditionData);
	pData.colorController.CreateActionData(sourcePtr->GetEffectDataName());	// 空の時は何も作らずスルーする
	pData.imgData.push_back(std::pair<int, SImageEffectData>());
	auto pushData = pData.imgData.end() - 1;
	pushData->first = mOrderCounter++;
	pushData->second.pSource.swap(sourcePtr);
	pushData->second.pDest.swap(destPtr);
	sourcePtr.reset(nullptr);
	destPtr.reset(nullptr);
}

void CReadEffectDataFromCSV::PushColorData(SSlotEffectData& pData, std::unique_ptr<CImageColorManager>& colorPtr) {
	pData.colorController.AddColorData(*colorPtr);
	colorPtr.reset(nullptr);
}

bool CReadEffectDataFromCSV::MakeData(SSlotEffectData& pData, CEffectVariableManager& pVar, CSlotTimerManager& pTimer, CReelManager& pReel) {
	typedef unsigned int UINT;
	std::unique_ptr<IImageSourceManager> sourcePtr(nullptr);
	std::unique_ptr<CImageColorManager> colorPtr(nullptr);
	std::unique_ptr<IImageDestinationManager> destPtr(nullptr);
	std::unique_ptr<CEffectVariableRoleMaker> rolePtr(nullptr);

	try {
		int rowCount = 0;
		while (!DataEOF()) {
			++rowCount;
			StringArr NowGetStr;
			GetStrSplitByComma(NowGetStr);
			if (NowGetStr.at(0)[0] == ';') continue;
			if (NowGetStr.at(0) == "#IF") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				if(!mConditionData.CreateCondition(NowGetStr)) throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#ENDIF") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				mConditionData.DeleteLastCondition();
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#CLEARIF") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				mConditionData.DeleteAllCondition();
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#TIMING") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				if(!mConditionData.CreateTiming(NowGetStr, pVar)) throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#TIMINGCLR") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				mConditionData.ClearTimer();
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#makeVar") {
				// バグ防止のためIF文等の条件に関係なく変数を生成する
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				pVar.CreateNewVar(NowGetStr[1], std::stoi(NowGetStr[2]));
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#setVal") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				CEffectVariableInsertMaker maker;
				if(!maker.MakeData(NowGetStr, pVar))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));

				pData.conditionData.push_back(mConditionData);
				pData.varInsertData.push_back(std::pair<int, SEffectVariableInsertData>(mOrderCounter++, maker.Extract()));
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#role") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				if (rolePtr == nullptr) rolePtr.reset(new CEffectVariableRoleMaker);
				if (!rolePtr->MakeData(NowGetStr, pVar))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));

				mReadStatus = EReadStatus::eVarSetting;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#endRole") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);
				if (rolePtr == nullptr)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));

				if (!rolePtr->FinalizeData())
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				pData.conditionData.push_back(mConditionData);
				pData.varRoleData.push_back(std::pair<int, SEffectVariableRoleData>(mOrderCounter++, rolePtr->Extract()));
				rolePtr.reset(nullptr);

				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}

			if (NowGetStr.at(0) == "#imgSrc") {
				if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eImgSrc)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);
				if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceDefault(pVar));

				mReadStatus = EReadStatus::eSource;
				mHeading = ENowReadingHead::eImgSrc;
				if (!sourcePtr->Init(NowGetStr, pTimer))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
			}
			if (NowGetStr.at(0) == "#numSrc") {
				if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eNumSrc)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);
				if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceNumber(pVar));

				mReadStatus = EReadStatus::eSource;
				mHeading = ENowReadingHead::eNumSrc;
				if (!sourcePtr->Init(NowGetStr, pTimer))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
			}
			if (NowGetStr.at(0) == "#reelSrc") {
				if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eReelSrc)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);
				if (sourcePtr == nullptr) sourcePtr.reset(new CImageSourceReel(pVar));

				mReadStatus = EReadStatus::eSource;
				mHeading = ENowReadingHead::eReelSrc;
				if (!sourcePtr->Init(NowGetStr, pTimer))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
			}
			if (NowGetStr.at(0) == "#imgColorMap") {
				if (mReadStatus == EReadStatus::eSource) 
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);
				if (colorPtr == nullptr) colorPtr.reset(new CImageColorManager(pVar));

				mReadStatus = EReadStatus::eColorMap;
				mHeading = ENowReadingHead::eImgCM;
				if (!colorPtr->Init(NowGetStr, pTimer))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
			}
			if (NowGetStr.at(0) == "#setColorMap") {
				if (colorPtr != nullptr) PushColorData(pData, colorPtr);
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#imgDst") {
				if (mReadStatus == EReadStatus::eDestination && mHeading != ENowReadingHead::eImgDst)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eSource) {
					if (mHeading != ENowReadingHead::eImgSrc && mHeading != ENowReadingHead::eNumSrc)
						throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				}
				if (mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (destPtr == nullptr) destPtr.reset(new CImageDestinationDefault(pVar));

				mReadStatus = EReadStatus::eDestination;
				mHeading = ENowReadingHead::eImgDst;
				if (!destPtr->Init(NowGetStr, pTimer))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
			}
			if (NowGetStr.at(0) == "#reelDst") {
				if (mReadStatus == EReadStatus::eDestination && mHeading != ENowReadingHead::eReelDst)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eSource && mHeading != ENowReadingHead::eReelSrc)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (destPtr == nullptr) destPtr.reset(new CImageDestinationReel(pVar, pReel));

				mReadStatus = EReadStatus::eDestination;
				mHeading = ENowReadingHead::eReelDst;
				if (!destPtr->Init(NowGetStr, pTimer))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
			}
			if (NowGetStr.at(0) == "#makeScr") {
				pVar.MakeScreenID(NowGetStr);
			}
			if (NowGetStr.at(0) == "#clearScr") {
				pData.conditionData.push_back(mConditionData);
				pData.clearScreenData.push_back(std::pair<int, std::string>(mOrderCounter++, NowGetStr.at(1)));
			}
			if (NowGetStr.at(0) == "#applyCM") {
				CImageColorApplyDataBuilder builder;
				if(!builder.Init(NowGetStr))
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				pData.conditionData.push_back(mConditionData);
				pData.colorApplyData.push_back(std::pair<int, SImageColorApplyData>(mOrderCounter++, builder.Extract()));
			}
			if (NowGetStr.at(0) == "#sndRing") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				CSlotSoundRingDataMaker maker;
				if(!maker.MakeRingData(NowGetStr, pVar)) throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				pData.soundRingData.push_back(std::pair<int, SSlotSoundRingData>(mOrderCounter++, maker.ExtractRing()));
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#sndStop") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				CSlotSoundRingDataMaker maker;
				if(!maker.MakeStopData(NowGetStr, pVar)) throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				pData.soundStopData.push_back(std::pair<int, SSlotSoundStopData>(mOrderCounter++, maker.ExtractStop()));
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}
			if (NowGetStr.at(0) == "#sndVol") {
				if (mReadStatus == EReadStatus::eSource || mReadStatus == EReadStatus::eColorMap || mReadStatus == EReadStatus::eVarSetting)
					throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				if (mReadStatus == EReadStatus::eDestination) PushImgData(pData, sourcePtr, destPtr);

				CSlotSoundVolumeDataMaker maker;
				if(!maker.MakeData(NowGetStr, pVar)) throw ErrIllegalCSVDefinition(rowCount, NowGetStr.at(0));
				pData.soundVolData.push_back(std::pair<int, SSlotSoundVolumeData>(mOrderCounter++, maker.Extract()));
				mReadStatus = EReadStatus::eInitial;
				mHeading = ENowReadingHead::eNone;
			}

		}

		if (sourcePtr != nullptr && destPtr != nullptr) PushImgData(pData, sourcePtr, destPtr);
		return true;
	}
	catch (ErrIllegalCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}
	catch (std::invalid_argument e) {
		return false;
	}
}
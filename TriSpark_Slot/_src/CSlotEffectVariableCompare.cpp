#include "_header/CSlotEffectVariableCompare.hpp"
#include "_header/ErrClass.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotTimerManager.hpp"

bool CSlotEffectVariableCompareMaker::CreateCondition(std::vector<std::string> pData) {
	try {
		const int dataSize = 4;
		if (pData.size() < 2) throw ErrLessCSVDefinition(pData, 2);
		int definitionSize;
		StrToNum(definitionSize, pData[1]);
		if (pData.size() < 2 + dataSize * definitionSize) throw ErrLessCSVDefinition(pData, 2 + dataSize * definitionSize);

		std::deque<SSlotEffectVariableCondition> addData;
		for (auto i = 0; i < definitionSize; ++i) {
			SSlotEffectVariableCondition data;
			if (pData[2+dataSize*i][0] != '$') throw ErrUndeclaredVar("Undefined Condition varName: " + pData[2+dataSize*i]);
			data.variableName = pData[2 + dataSize * i];

			// 範囲未定義時はそれぞれデフォルト値: INT_MINおよびINT_MAXが使用される
			if (pData[3 + dataSize * i][0] == '$') data.lowerLimit.second = pData[3 + dataSize * i];
			else if (pData[3 + dataSize * i] != "") data.lowerLimit.first = std::stoi(pData[3 + dataSize * i]);
			
			if (pData[4 + dataSize * i][0] == '$') data.upperLimit.second = pData[4 + dataSize * i];
			else if (pData[4 + dataSize * i] != "") data.upperLimit.first = std::stoi(pData[4 + dataSize * i]);

			data.invertFlag = pData[5 + dataSize * i] == "T";

			addData.push_back(data);
		}
		mVariableCondData.push_back(addData);
		return true;
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}

bool CSlotEffectVariableCompareMaker::CreateTiming(std::vector<std::string> pData, CEffectVariableManager& pVar) {
	try {
		if (pData.size() < 3) throw ErrLessCSVDefinition(pData, 3);
		if (pData[1][0] != '@') throw ErrUndeclaredVar("Undefined Condition timeName: " + pData[1]);
		mTimingData = std::pair<std::string, int>(pData[1], GetVariableID(pData[2], false, pVar));
		return true;
	}
	catch (ErrLessCSVDefinition e) {
		e.WriteErrLog();
		return false;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}

void CSlotEffectVariableCompareMaker::DeleteLastCondition() {
	mVariableCondData.pop_back();
}
void CSlotEffectVariableCompareMaker::DeleteAllCondition() {
	mVariableCondData.clear();
	ClearTimer();
}
void CSlotEffectVariableCompareMaker::ClearTimer() {
	mTimingData = std::pair <std::string, int>("", -1);
}



CSlotEffectVariableCompare::CSlotEffectVariableCompare(CSlotEffectVariableCompareMaker& pMaker) {
	mVariableCondData = pMaker.mVariableCondData;
	mTimingData = pMaker.mTimingData;

	mConditionQualified = false;
	mIsAlreadyTimePassed = false;
	mLastTime = LLONG_MAX;
}

// [ret]条件達成が正常に設定できたかどうか
bool CSlotEffectVariableCompare::SetCondition(const CEffectVariableManager& pVar, const CSlotTimerManager& pTimer) {
	try {
		mConditionQualified = true;
		// タイマー条件が設定されている場合は変数より先に評価
		if (mTimingData.first != "") {
			long long timer;
			const bool timerAvailable = pTimer.GetTimeFromTimerHandle(timer, pTimer.GetTimerHandle(mTimingData.first));
			if (!timerAvailable) {
				mIsAlreadyTimePassed = false; 
				mConditionQualified = false;
				mLastTime = LLONG_MAX;
				return true;
			}
			if (timer >= pVar.GetVal(mTimingData.second)) {
				mConditionQualified = !mIsAlreadyTimePassed;
				// 時間が前回取得より巻き戻っていれば書き換え
				if (timer < mLastTime) mConditionQualified = true;
				mIsAlreadyTimePassed = true;
			} else {
				mConditionQualified = false;
				mIsAlreadyTimePassed = false;
			}
			mLastTime = timer;
		}

		// 各条件を比較
		auto conditionsAND = mVariableCondData;
		while (!conditionsAND.empty() && mConditionQualified) {
			auto conditionsOR = conditionsAND.front();
			bool orCheck = false;
			while (!conditionsOR.empty() && !orCheck) {
				auto checkCond = conditionsOR.front();
				// 変数を展開
				const int checkVal = pVar.GetVal(pVar.GetValIDFromName(checkCond.variableName));
				if (checkCond.lowerLimit.second != "")
					checkCond.lowerLimit.first = pVar.GetVal(pVar.GetValIDFromName(checkCond.lowerLimit.second));
				if (checkCond.upperLimit.second != "")
					checkCond.upperLimit.first = pVar.GetVal(pVar.GetValIDFromName(checkCond.upperLimit.second));

				// 比較を実施
				orCheck = (checkVal >= checkCond.lowerLimit.first && checkVal <= checkCond.upperLimit.first) ^ checkCond.invertFlag;
				conditionsOR.pop_front();
			}
			mConditionQualified = orCheck;
			conditionsAND.pop_front();
		}

		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}
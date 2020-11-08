#pragma once
#include "SSlotEffectVariableCondition.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <deque>
#include <algorithm>

class CSlotTimerManager;
class CSlotEffectVariableCompareMaker;
class CEffectVariableManager;
class CSlotTimerManager;

class CSlotEffectVariableCompare {
	std::deque<std::deque<SSlotEffectVariableCondition>>	mVariableCondData;	// [and][or]
	std::pair<std::string, long long>						mTimingData;		// [timerName, checkTime]

	bool													mConditionQualified;
	bool													mIsAlreadyTimePassed;

public:
	CSlotEffectVariableCompare(CSlotEffectVariableCompareMaker& pMaker);

	bool SetCondition(const CEffectVariableManager& pVar, const CSlotTimerManager& pTimer);
	bool GetCondition() { return mConditionQualified; }
};


class CSlotEffectVariableCompareMaker {
	std::deque<std::deque<SSlotEffectVariableCondition>>	mVariableCondData;	// [and][or]
	std::pair<std::string, long long>						mTimingData;		// [timerName, checkTime]

	template<class T> void StrToNum(T& InputFor, std::string& p_Data) {
		// [act]テンプレート型、1つ目の引数に2つ目の文字列から抽出した数字を代入します
		//		bool型の場合は整数で判別し、0がfalse、それ以外がtrueとなる模様です
		std::istringstream Data(p_Data);
		Data >> InputFor;
	};

	// データを作って受け渡しする用のクラス → 使用クラスをfriend登録してよりセーフティに運用を行う
	friend CSlotEffectVariableCompare;

public:
	CSlotEffectVariableCompareMaker() : mTimingData("", -1) {};

	bool CreateCondition(std::vector<std::string> pData);
	bool CreateTiming(std::vector<std::string> pData);

	void DeleteLastCondition();
	void DeleteAllCondition();
};
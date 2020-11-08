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
		// [act]�e���v���[�g�^�A1�ڂ̈�����2�ڂ̕����񂩂璊�o���������������܂�
		//		bool�^�̏ꍇ�͐����Ŕ��ʂ��A0��false�A����ȊO��true�ƂȂ�͗l�ł�
		std::istringstream Data(p_Data);
		Data >> InputFor;
	};

	// �f�[�^������Ď󂯓n������p�̃N���X �� �g�p�N���X��friend�o�^���Ă��Z�[�t�e�B�ɉ^�p���s��
	friend CSlotEffectVariableCompare;

public:
	CSlotEffectVariableCompareMaker() : mTimingData("", -1) {};

	bool CreateCondition(std::vector<std::string> pData);
	bool CreateTiming(std::vector<std::string> pData);

	void DeleteLastCondition();
	void DeleteAllCondition();
};
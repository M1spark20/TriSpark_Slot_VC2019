#pragma once
#include <string>
#include <vector>
#include <algorithm>

class CSlotInternalDataManager;

class CEffectVariableManager {
	std::vector<int>							mVariablePool;
	std::vector<std::pair<std::string, int>>	mVariableName;
public:
	bool Init();
	bool Process(const CSlotInternalDataManager& pIntData);
	int CreateNewConstant(int pVal);
	int CreateNewVar(std::string pValName, int pInitVal);
	void SetVarVal(std::string pValName, int pSetVal);
	int GetVal(int pValID);
	int GetVal(std::string pValName);
};
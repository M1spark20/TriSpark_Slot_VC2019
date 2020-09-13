#pragma once
#include <string>
#include <vector>
#include <algorithm>

class CSlotInternalDataManager;

class CEffectVariableManager {
	std::vector<int>							mVariablePool;
	std::vector<std::pair<std::string, int>>	mVariableName;
	std::vector<std::pair<std::string, int>>	mScreenData;

	int CreateNewConstant(int pVal);
	int CreateNewVar(std::string pValName, int pInitVal);

public:
	bool Init();
	bool Process(const CSlotInternalDataManager& pIntData);
	int MakeValID(std::string pValName);
	void SetVarVal(std::string pValName, int pSetVal);
	int GetVal(int pValID);

	void MakeScreenID(std::string pValName);
	int GetScreenID(std::string pValName);

	~CEffectVariableManager();
};
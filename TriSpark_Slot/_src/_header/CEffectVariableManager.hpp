#pragma once
#include <string>
#include <vector>
#include <algorithm>

class CSlotInternalDataManager;

class CEffectVariableManager {
	std::vector<int>							mVariablePool;
	std::vector<std::pair<std::string, int>>	mVariableName;
	std::vector<std::pair<std::string, int>>	mScreenData;

	typedef std::vector<std::string> StringArr;

	int CreateNewConstant(int pVal);
	int CreateNewVar(std::string pValName, int pInitVal);

public:
	bool Init();
	bool Process(const CSlotInternalDataManager& pIntData);
	int MakeValID(std::string pValName);
	void SetVarVal(std::string pValName, int pSetVal);
	int GetVal(int pValID);

	void MakeScreenID(StringArr pData);
	int GetScreenID(std::string pValName);
	bool ClearScreen(std::string pValName);

	~CEffectVariableManager();
};
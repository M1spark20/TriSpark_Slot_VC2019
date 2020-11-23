#pragma once
#include "IEffectExecuteMakerBase.hpp"
#include <string>
#include <vector>
#include <random>
#include <algorithm>

class CSlotInternalDataManager;

struct SEffectVariableInsertData {
	enum class EOperand { eAdd, eSub, eMul, eDiv, eMod };
	// �ϐ��͕ϐ�ID�ŊǗ�����
	int										inputFor;	// �ϐ�ID
	int										defNum;		// �ϐ�ID
	std::vector<std::pair<EOperand, int>>	calcData;	// [���Z����, �ϐ�ID]
};

struct SEffectVariableRoleData {
	// �ϐ��͕ϐ�ID�ŊǗ�����
	int												randMax;		// ������
	std::vector<int>								inputForList;	// �ϐ�ID
	std::vector<std::pair<int, std::vector<int>>>	roleData;		// [�����l(������), <eachInputData(�ϐ�ID)>]
};

class CEffectVariableManager {
	std::vector<int>							mVariablePool;
	std::vector<std::pair<std::string, int>>	mVariableName;
	std::vector<std::pair<std::string, int>>	mScreenData;
	std::mt19937								mRandomizer;

	typedef std::vector<std::string> StringArr;

	int CreateNewConstant(int pVal);

public:
	CEffectVariableManager();
	bool Init();
	bool Process(const CSlotInternalDataManager& pIntData);
	int MakeValID(std::string pValName);
	void SetVarVal(std::string pValName, int pSetVal);
	int GetVal(int pValID) const;
	int GetValIDFromName(std::string pValName) const;
	int CreateNewVar(std::string pValName, int pInitVal);

	void MakeScreenID(StringArr pData);
	int GetScreenID(std::string pValName) const;
	bool ClearScreen(std::string pValName);

	bool InsertVar(const SEffectVariableInsertData& pData);
	bool RoleVar(const SEffectVariableRoleData& pData);

	~CEffectVariableManager();
};


class CEffectVariableInsertMaker : public IEffectExecuteMakerBase {
	SEffectVariableInsertData	mData;

public:
	bool MakeData(std::vector<std::string> pCSVData, CEffectVariableManager& pManager);
	SEffectVariableInsertData Extract() const { return mData; }
};

class CEffectVariableRoleMaker : public IEffectExecuteMakerBase {
	SEffectVariableRoleData	mData;
	bool mNoChangeFlag;
	int mRandNumTotal;
public:
	CEffectVariableRoleMaker() : mNoChangeFlag(false), mRandNumTotal(0) { mData.randMax = -1; }
	bool MakeData(std::vector<std::string> pCSVData, CEffectVariableManager& pManager);
	bool FinalizeData();
	SEffectVariableRoleData Extract() const { return mData; }
};
#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotInternalDataManager.hpp"
#include "_header/ErrClass.hpp"
#include <stdexcept>
#include "DxLib.h"


CEffectVariableManager::CEffectVariableManager() {
	std::random_device rd;
	mRandomizer.seed(rd());
}

// [act]�V�X�e���ϐ��̏��������s��
// [ret]������������ɍs��ꂽ���ǂ���
bool CEffectVariableManager::Init() {
	// �`��e�X�g�̂��߂Ƃ肠������`
	CreateNewVar("credit", 0);
	CreateNewVar("payout", 0);
	CreateNewVar("betNum", 0);
	CreateNewVar("flagID", 0);
	CreateNewVar("bonusID", 0);
	return true;
}

// [act]�V�X�e���ϐ��̍X�V���s��
// [prm]pIntData	: �X�V�Ɏg�p����������Ǘ��N���X
// [ret]�X�V������ɍs��ꂽ���ǂ���
bool CEffectVariableManager::Process(const CSlotInternalDataManager& pIntData) {
	const auto data = pIntData.GetData();
	SetVarVal("credit", data.credit);
	SetVarVal("payout", data.payout);
	SetVarVal("betNum", data.betNum);
	SetVarVal("flagID", data.flag.first);
	SetVarVal("bonusID", data.flag.second);
	return true;
}

// [act]�萔(�܂��͕ϐ�)�̐V�K�o�^���s��
// [prm]pVal	: �ϐ��l
// [ret]�ϐ�ID
int CEffectVariableManager::CreateNewConstant(int pVal) {
	const auto ans = mVariablePool.size();
	mVariablePool.push_back(pVal);
	return ans;
}

// [act]�ϐ��̐V�K�o�^���s��
// [prm]pValName	: �ϐ���(�ϐ�������$�}�[�N�͂����Ă��Ȃ��Ă�OK) �܂��� ������ŕ\�������l
// [ret]�ϐ�ID
int CEffectVariableManager::CreateNewVar(std::string pValName, int pInitVal) {
	if (pValName.empty()) return -1;
	const std::string registName = pValName[0] == '$' ? pValName.substr(1) : pValName;
	if (registName.empty()) return -1;
	const auto ans = mVariableName.size();
	mVariableName.push_back(std::pair<std::string, int>(registName, CreateNewConstant(pInitVal)));
	return ans;
}

// [act]�ϐ��̐V�K�o�^���s��
//		<Throwable>ErrUndeclaredVar
// [prm]pValName	: �ϐ���(�ϐ�������$�}�[�N���K�{) �܂��� ������ŕ\�������l
// [ret]�ϐ�ID
int CEffectVariableManager::MakeValID(std::string pValName) {
	if (pValName.empty()) throw ErrUndeclaredVar("Name: <Empty>");
	if (pValName[0] == '$') {
		const std::string registName = pValName.substr(1);
		if (registName.empty()) throw ErrUndeclaredVar("Name: <Empty>");

		for (auto it = mVariableName.begin(); it != mVariableName.end(); ++it) {
			if (it->first != registName) continue;
			return it->second;
		}
	}
	else {
		// �萔��o�^
		return CreateNewConstant(std::stoi(pValName));
	}
	throw ErrUndeclaredVar(pValName);
}

// [act]�ϐ�������ID�̎擾���s��
//		<Throwable>ErrUndeclaredVar
// [prm]pValName	: �ϐ���(�ϐ�������$�}�[�N���K�{) �܂��� ������ŕ\�������l
// [ret]�ϐ�ID
int CEffectVariableManager::GetValIDFromName(std::string pValName) const{
	if (pValName.empty()) throw ErrUndeclaredVar("Name: <Empty>");
	if (pValName[0] == '$') {
		const std::string registName = pValName.substr(1);
		if (registName.empty()) throw ErrUndeclaredVar("Name: <Empty>");

		for (auto it = mVariableName.begin(); it != mVariableName.end(); ++it) {
			if (it->first != registName) continue;
			return it->second;
		}
	}
	throw ErrUndeclaredVar(pValName);
}

// [act]�ϐ��̒l���X�V����
//		<Throwable>ErrUndeclaredVar
// [prm]pValName	: �ϐ����A�ϐ�������$�}�[�N�͂����Ă��Ȃ��Ă��悢
//		pInitVal	: �ϐ��̏����l
void CEffectVariableManager::SetVarVal(std::string pValName, int pSetVal) {
	if (pValName.empty()) return;
	const std::string registName = pValName[0] == '$' ? pValName.substr(1) : pValName;
	if (registName.empty()) return;
	
	for (auto it = mVariableName.begin(); it != mVariableName.end(); ++it) {
		if (it->first != registName) continue;
		mVariablePool[it->second] = pSetVal;
		return;
	}
	throw ErrUndeclaredVar(pValName);
}

// [act]�ϐ��̒l���擾����
//		<Throwable>ErrUndeclaredVar
// [prm]pValID		: �ϐ�ID
int CEffectVariableManager::GetVal(int pValID) const{
	if (pValID < 0 || pValID >= mVariablePool.size()) throw ErrUndeclaredVar("id: " + std::to_string(pValID));
	return mVariablePool[pValID];
}

// [act]DxLib�`���ʂ�V�K��������
// [prm]pValName	: �ϐ�ID
//		<�����͉�>
void CEffectVariableManager::MakeScreenID(StringArr pData) {
	if (pData.size() < 5) return;
	if (pData[1].empty()) return;
	const std::string registName = pData[1][0] == '*' ? pData[1].substr(1) : pData[1];
	if (registName.empty()) return;

	std::pair<std::string, int> data;
	data.first = registName;
	try {
		const int screenSize[] = { GetVal(MakeValID(pData[2])), GetVal(MakeValID(pData[3]))	};
		const bool isUseTrans = pData[4] == "T";
		data.second = DxLib::MakeScreen(screenSize[0], screenSize[1], isUseTrans ? TRUE : FALSE);
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
	}
	mScreenData.push_back(data);
}

int CEffectVariableManager::GetScreenID(std::string pValName) const{
	if (pValName.empty()) throw ErrUndeclaredVar("Screen: <Empty>");
	if (pValName[0] == '*') {
		const std::string registName = pValName.substr(1);
		if (registName.empty()) throw ErrUndeclaredVar("Screen: <Empty>");

		/* constant */ {
			if (registName == "mainScr") return DX_SCREEN_BACK;
		}

		for (auto it = mScreenData.begin(); it != mScreenData.end(); ++it) {
			if (it->first != registName) continue;
			return it->second;
		}
	}
	throw ErrUndeclaredVar("Screen: " + pValName);
}

bool CEffectVariableManager::ClearScreen(std::string pValName) {
	try {
		DxLib::SetDrawScreen(GetScreenID(pValName));
		DxLib::ClearDrawScreen();
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
}

bool CEffectVariableManager::InsertVar(const SEffectVariableInsertData& pData) {
	try {
		typedef SEffectVariableInsertData::EOperand tdOperand;

		int putData = GetVal(pData.defNum);
		for (auto calc : pData.calcData) {
			const int rightVal = GetVal(calc.second);
			if (rightVal == 0 && (calc.first == tdOperand::eDiv || calc.first == tdOperand::eMod))
				throw ErrDivideBy0();
			switch (calc.first)
			{
				case tdOperand::eAdd:	putData = putData + rightVal;	break;
				case tdOperand::eSub:	putData = putData - rightVal;	break;
				case tdOperand::eMul:	putData = putData * rightVal;	break;
				case tdOperand::eDiv:	putData = putData / rightVal;	break;
				case tdOperand::eMod:	putData = putData % rightVal;	break;
				default:	return false;	break;
			}
		}
		// ���ڑ������
		mVariablePool[pData.inputFor] = putData;
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	catch (ErrDivideBy0 e) {
		e.WriteErrLog();
		return false;
	}
}

bool CEffectVariableManager::RoleVar(const SEffectVariableRoleData& pData) {
	try {
		if (pData.randMax < 0) throw ErrInternalVarUndeclared("RandMax : under 0");
		std::uniform_int_distribution<> rnd(0, pData.randMax - 1);
		const int randVal = rnd(mRandomizer);
		int seedTotal = 0;
		size_t lookingID;
		
		for (lookingID = 0; lookingID < pData.roleData.size(); ++lookingID) {
			seedTotal += pData.roleData[lookingID].first;
			if (seedTotal > randVal) break;
		}
		if (seedTotal <= randVal) return false;

		// �������v�m�F�����
		if (pData.roleData[lookingID].second.size() != pData.inputForList.size()) return false;
		for (size_t i = 0; i < pData.inputForList.size(); ++i) {
			const auto putVal = GetVal(pData.roleData[lookingID].second[i]);
			// ���ڑ������
			mVariablePool[pData.inputForList[i]] = putVal;
		}
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	catch (ErrInternalVarUndeclared e) {
		e.WriteErrLog();
		return false;
	}
}


CEffectVariableManager::~CEffectVariableManager() {
	for (auto it = mScreenData.begin(); it != mScreenData.end(); ++it) {
		DxLib::DeleteGraph(it->second);
	}
}


bool CEffectVariableInsertMaker::MakeData(std::vector<std::string> pCSVData, CEffectVariableManager& pManager) {
	if (pCSVData.size() <= 3) return false;

	try {
		mData.inputFor	= GetVariableID(pCSVData[1], true, pManager);
		mData.defNum	= GetVariableID(pCSVData[2], false, pManager);

		// �v�Z����
		typedef SEffectVariableInsertData::EOperand					tdOperand;
		typedef std::pair<SEffectVariableInsertData::EOperand, int> tdOperateData;
		for (size_t i = 3; i+1 < pCSVData.size(); i+=2) {
			if (pCSVData[i] == "") break;
			tdOperateData addData;
			if		(pCSVData[i] == "+") addData.first = tdOperand::eAdd;
			else if (pCSVData[i] == "-") addData.first = tdOperand::eSub;
			else if (pCSVData[i] == "*") addData.first = tdOperand::eMul;
			else if (pCSVData[i] == "/") addData.first = tdOperand::eDiv;
			else if (pCSVData[i] == "%") addData.first = tdOperand::eMod;
			else						 return false;
			addData.second = GetVariableID(pCSVData[i + 1], false, pManager);
			mData.calcData.push_back(addData);
		}
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	catch (std::invalid_argument e) {
		return false;
	}
}


bool CEffectVariableRoleMaker::MakeData(std::vector<std::string> pCSVData, CEffectVariableManager& pManager) {
	typedef std::pair<int, std::vector<int>> tdRoleData;
	if (mNoChangeFlag) return false;
	try {
		if (mData.randMax < 0) {
			if (pCSVData.size() < 3) return false;
			mData.randMax = std::stoi(pCSVData[1]);
			for (size_t i = 2; i < pCSVData.size(); ++i) {
				if (pCSVData[i] == "") break;
				mData.inputForList.push_back(GetVariableID(pCSVData[i], true, pManager));
			}
		}
		else {
			tdRoleData addData;
			if (pCSVData.size() < 2 + mData.inputForList.size()) return false;
			addData.first = std::stoi(pCSVData[1]);
			for (size_t i = 0; i < mData.inputForList.size(); ++i) {
				if (pCSVData[i+2] == "") return false;
				addData.second.push_back(GetVariableID(pCSVData[i+2], false, pManager));
			}
			mData.roleData.push_back(addData);
			mRandNumTotal += addData.first;
		}
		return true;
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
		return false;
	}
	catch (std::invalid_argument e) {
		return false;
	}
}

bool CEffectVariableRoleMaker::FinalizeData() {
	if (mRandNumTotal == mData.randMax) {
		mNoChangeFlag = true;
		return true;
	}
	return false;
}
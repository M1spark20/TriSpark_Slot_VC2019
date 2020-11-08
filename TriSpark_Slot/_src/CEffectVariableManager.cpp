#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotInternalDataManager.hpp"
#include "_header/ErrClass.hpp"
#include "DxLib.h"

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
	mVariableName.push_back(std::pair<std::string, int>(pValName, CreateNewConstant(pInitVal)));
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

CEffectVariableManager::~CEffectVariableManager() {
	for (auto it = mScreenData.begin(); it != mScreenData.end(); ++it) {
		DxLib::DeleteGraph(it->second);
	}
}
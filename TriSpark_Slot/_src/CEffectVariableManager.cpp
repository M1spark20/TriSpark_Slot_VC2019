#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotInternalDataManager.hpp"
#include "_header/ErrClass.hpp"

// [act]�V�X�e���ϐ��̏��������s��
// [ret]������������ɍs��ꂽ���ǂ���
bool CEffectVariableManager::Init() {
	// �`��e�X�g�̂��߂�credit��payout�݂̂Ƃ肠������`
	CreateNewVar("credit", 0);
	CreateNewVar("payout", 0);
	return true;
}

// [act]�V�X�e���ϐ��̍X�V���s��
// [prm]pIntData	: �X�V�Ɏg�p����������Ǘ��N���X
// [ret]�X�V������ɍs��ꂽ���ǂ���
bool CEffectVariableManager::Process(const CSlotInternalDataManager& pIntData) {
	const auto data = pIntData.GetData();
	SetVarVal("credit", data.credit);
	SetVarVal("payout", data.payout);
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
// [prm]pValName	: �ϐ����A�ϐ�������$�}�[�N�͂����Ă��Ȃ��Ă��悢
//		pInitVal	: �ϐ��̏����l
// [ret]�ϐ�ID
int CEffectVariableManager::CreateNewVar(std::string pValName, int pInitVal) {
	if (pValName.empty()) return -1;
	const std::string registName = pValName[0] == '$' ? pValName.substr(1) : pValName;
	if (registName.empty()) return -1;
	const auto ans = mVariableName.size();
	mVariableName.push_back(std::pair<std::string, int>(pValName, CreateNewConstant(pInitVal)));
	return ans;
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
int CEffectVariableManager::GetVal(int pValID) {
	if (pValID < 0 || pValID >= mVariablePool.size()) throw ErrUndeclaredVar("id: " + std::to_string(pValID));
	return mVariablePool[pValID];
}

// [act]�ϐ��̒l���擾����
//		<Throwable>ErrUndeclaredVar
// [prm]pValName	: �ϐ����A�ϐ�������$�}�[�N�͂����Ă��Ȃ��Ă��悢
int CEffectVariableManager::GetVal(std::string pValName) {
	if (pValName.empty()) throw ErrUndeclaredVar("Name: <Empty>");
	const std::string registName = pValName[0] == '$' ? pValName.substr(1) : pValName;
	if (registName.empty()) throw ErrUndeclaredVar("Name: <Empty>");
	
	for (auto it = mVariableName.begin(); it != mVariableName.end(); ++it) {
		if (it->first != registName) continue;
		return GetVal(it->second);
	}
	throw ErrUndeclaredVar(pValName);
}
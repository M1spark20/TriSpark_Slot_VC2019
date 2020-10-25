#include "_header/CImageColorController.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/ErrClass.hpp"


CImageColorManager& CImageColorController::GetColorDataByID(std::string pColorDataName) {
	if (!CheckAvailableName(pColorDataName)) throw ErrUndeclaredVar("(No ColorData Name)");
	for (auto& colorData : mColorData) {
		if (!colorData.MatchDataName(pColorDataName)) continue;
		return colorData;
	}
	throw ErrUndeclaredVar("colorDataName: " + pColorDataName);
}

std::vector<CImageColorManager*>& CImageColorController::GetActionDataByID(std::string pUseEffectName) {
	if (!CheckAvailableName(pUseEffectName)) throw ErrUndeclaredVar("(No ActionData Name)");
	for (auto& checkData : mActionData) {
		if (checkData.first != pUseEffectName) continue;
		return checkData.second;
	}
	throw ErrUndeclaredVar("actionDataName: " + pUseEffectName);
}

bool CImageColorController::CheckAvailableName(std::string pData) {
	return !(pData == "" || pData == "-");
}

void CImageColorController::CreateActionData(std::string pUseEffectName) {
	if (!CheckAvailableName(pUseEffectName)) return;
	mActionData.push_back(std::pair<std::string, std::vector<CImageColorManager*>>(
		pUseEffectName, std::vector<CImageColorManager*>()
	));
}

void CImageColorController::AddActionData(std::string pUseEffectName, std::string pColorDataName) {
	try {
		auto colorData = GetColorDataByID(pColorDataName);
		auto actionData = GetActionDataByID(pUseEffectName);
		actionData.push_back(&colorData);
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
	}
}

void CImageColorController::ResetActionData(std::string pUseEffectName) {
	try {
		auto actionData = GetActionDataByID(pUseEffectName);
		actionData.clear();
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
	}
}

// [act]�`��Ɏg�p����color�f�[�^�����肷��
// [prm]pSrcColorID	:src����colorData�Ăяo���pID���w��
//		pIndex		:�Ăяo���pindex(�͈͊O�̏ꍇnullptr���Ԃ�) �� �S�f�[�^��I���\
// [ret]nullptr		:�f�[�^�����݂��Ȃ��ꍇ
//		else		:�f�[�^�����݂���ꍇ�A���̃|�C���^
const CImageColorManager* const CImageColorController::GetColorData(std::string pUseEffectName, int pIndex) {
	if (pIndex < 0) return nullptr;
	if (!CheckAvailableName(pUseEffectName)) return nullptr;
	try {
		auto actionData = GetActionDataByID(pUseEffectName);
		if ((size_t)pIndex >= actionData.size()) return nullptr;
		return actionData[pIndex];
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
	}
	return nullptr;
}

bool CImageColorController::SetTimerAll(CSlotTimerManager& pTimer) {
	bool ans = true;
	for (auto actionData : mColorData) ans &= actionData.SetTimer(pTimer);
	return ans;
}

void CImageColorController::ResetTimerAll() {
	for (auto actionData : mColorData) actionData.ResetTimer();
}

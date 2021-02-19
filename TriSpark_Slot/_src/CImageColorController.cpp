﻿#include "_header/CImageColorController.hpp"
#include "_header/CSlotTimerManager.hpp"
#include "_header/ErrClass.hpp"
#include <stdexcept>

bool CImageColorApplyDataBuilder::Init(std::vector<std::string> pStringArr) {
	try {
		mApplyData.applyTo = pStringArr.at(1);
		mApplyData.addFlag = pStringArr.at(2) == "T";
		const int addSize = std::stoi(pStringArr.at(3));
		for (int i = 0; i < addSize; ++i) mApplyData.addCMName.push_back(pStringArr.at(4 + i));
		return true;
	}
	catch (std::out_of_range e) {
		return false;
	}
	catch (std::invalid_argument e) {
		return false;
	}
}


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

void CImageColorController::AddActionData(const SImageColorApplyData& pData) {
	try {
		auto& actionData = GetActionDataByID(pData.applyTo);
		if (!pData.addFlag) ResetActionData(pData.applyTo);
		for (auto addData : pData.addCMName) {
			actionData.push_back(&GetColorDataByID(addData));
		}
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
	}
}

void CImageColorController::ResetActionData(std::string pUseEffectName) {
	try {
		auto& actionData = GetActionDataByID(pUseEffectName);
		actionData.clear();
	}
	catch (ErrUndeclaredVar e) {
		e.WriteErrLog();
	}
}

// [act]描画に使用するcolorデータを決定する
// [prm]pSrcColorID	:src側のcolorData呼び出し用IDを指定
//		pIndex		:呼び出し用index(範囲外の場合nullptrが返る) → 全データを選択可能
// [ret]nullptr		:データが存在しない場合
//		else		:データが存在する場合、そのポインタ
const CImageColorManager* const CImageColorController::GetColorData(std::string pUseEffectName, int pIndex) {
	if (pIndex < 0) return nullptr;
	if (!CheckAvailableName(pUseEffectName)) return nullptr;
	try {
		auto& actionData = GetActionDataByID(pUseEffectName);
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
	for (auto& actionData : mColorData) ans &= actionData.SetTimer(pTimer);
	return ans;
}

void CImageColorController::ResetTimerAll() {
	for (auto& actionData : mColorData) actionData.ResetTimer();
}

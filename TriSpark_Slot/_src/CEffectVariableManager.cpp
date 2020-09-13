#include "_header/CEffectVariableManager.hpp"
#include "_header/CSlotInternalDataManager.hpp"
#include "_header/ErrClass.hpp"
#include "DxLib.h"

// [act]システム変数の初期化を行う
// [ret]初期化が正常に行われたかどうか
bool CEffectVariableManager::Init() {
	// 描画テストのためにcreditとpayoutのみとりあえず定義
	CreateNewVar("credit", 0);
	CreateNewVar("payout", 0);
	return true;
}

// [act]システム変数の更新を行う
// [prm]pIntData	: 更新に使用する内部情報管理クラス
// [ret]更新が正常に行われたかどうか
bool CEffectVariableManager::Process(const CSlotInternalDataManager& pIntData) {
	const auto data = pIntData.GetData();
	SetVarVal("credit", data.credit);
	SetVarVal("payout", data.payout);
	return true;
}

// [act]定数(または変数)の新規登録を行う
// [prm]pVal	: 変数値
// [ret]変数ID
int CEffectVariableManager::CreateNewConstant(int pVal) {
	const auto ans = mVariablePool.size();
	mVariablePool.push_back(pVal);
	return ans;
}

// [act]変数の新規登録を行う
// [prm]pValName	: 変数名(変数を示す$マークはあってもなくてもOK) または 文字列で表した数値
// [ret]変数ID
int CEffectVariableManager::CreateNewVar(std::string pValName, int pInitVal) {
	if (pValName.empty()) return -1;
	const std::string registName = pValName[0] == '$' ? pValName.substr(1) : pValName;
	if (registName.empty()) return -1;
	const auto ans = mVariableName.size();
	mVariableName.push_back(std::pair<std::string, int>(pValName, CreateNewConstant(pInitVal)));
	return ans;
}

// [act]変数の新規登録を行う
//		<Throwable>ErrUndeclaredVar
// [prm]pValName	: 変数名(変数を示す$マークが必須) または 文字列で表した数値
// [ret]変数ID
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
		// 定数を登録
		return CreateNewConstant(std::stoi(pValName));
	}
	throw ErrUndeclaredVar(pValName);
}

// [act]変数の値を更新する
//		<Throwable>ErrUndeclaredVar
// [prm]pValName	: 変数名、変数を示す$マークはあってもなくてもよい
//		pInitVal	: 変数の初期値
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

// [act]変数の値を取得する
//		<Throwable>ErrUndeclaredVar
// [prm]pValID		: 変数ID
int CEffectVariableManager::GetVal(int pValID) {
	if (pValID < 0 || pValID >= mVariablePool.size()) throw ErrUndeclaredVar("id: " + std::to_string(pValID));
	return mVariablePool[pValID];
}

// [act]DxLib描画画面を新規生成する
// [prm]pValName	: 変数ID
//		<引数は仮>
void CEffectVariableManager::MakeScreenID(std::string pValName) {
	if (pValName.empty()) return;
	const std::string registName = pValName[0] == '*' ? pValName.substr(1) : pValName;
	if (registName.empty()) return;

	std::pair<std::string, int> data;
	data.first = pValName;
	// secondはcsvで定義を作ってから
}

int CEffectVariableManager::GetScreenID(std::string pValName) {
	if (pValName.empty()) throw ErrUndeclaredVar("Time: <Empty>");
	if (pValName[0] == '*') {
		const std::string registName = pValName.substr(1);
		if (registName.empty()) throw ErrUndeclaredVar("Time: <Empty>");

		/* constant */ {
			if (registName == "mainScr") return DX_SCREEN_BACK;
		}

		for (auto it = mScreenData.begin(); it != mScreenData.end(); ++it) {
			if (it->first != registName) continue;
			return it->second;
		}
	}
	throw ErrUndeclaredVar("Time: " + pValName);
}

CEffectVariableManager::~CEffectVariableManager() {
	for (auto it = mScreenData.begin(); it != mScreenData.end(); ++it) {
		DxLib::DeleteGraph(it->second);
	}
}
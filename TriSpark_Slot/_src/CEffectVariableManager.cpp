#include "_header/CEffectVariableManager.hpp"
#include "_header/SSlotGameDataWrapper.hpp"
#include "_header/ErrClass.hpp"
#include <stdexcept>
#include "DxLib.h"


CEffectVariableManager::CEffectVariableManager() {
	std::random_device rd;
	mRandomizer.seed(rd());
}

// [act]システム変数の初期化を行う
// [ret]初期化が正常に行われたかどうか
bool CEffectVariableManager::Init() {
	CreateNewVar("set", 0);
	CreateNewVar("flagID", 0);
	CreateNewVar("bonusID", 0);
	CreateNewVar("gameMode", 0);
	CreateNewVar("countOnlyJac", 0);
	CreateNewVar("nowRTMode", 0);
	CreateNewVar("nowRTLimit", 0);
	CreateNewVar("nextRTMode", 0);
	CreateNewVar("nextRTLimit", 0);
	CreateNewVar("canOverwriteRT", 0);
	CreateNewVar("betNum", 0);
	CreateNewVar("inCount", 0);
	CreateNewVar("outCount", 0);
	CreateNewVar("nowMedalCount", 0);
	CreateNewVar("isReplay", 0);
	CreateNewVar("credit", 0);
	CreateNewVar("payout", 0);
	CreateNewVar("gameStatus", 0);
	CreateNewVar("payoutFreeze", 0);
	CreateNewVar("payoutEffect", 0);
	CreateNewVar("payoutLine", 0);
	CreateNewVar("reachSound", -1);
	CreateNewVar("betFreeze", 50);
	CreateNewVar("dcTotalG", 0);
	CreateNewVar("dcStartG", 0);
	CreateNewVar("dcPayoutX10", 0);

	for (int i = 0; i < 2; ++i) {
		CreateNewVar("modeLim[" + std::to_string(i) + "]", 0);
		CreateNewVar("isGameLim[" + std::to_string(i) + "]", 0);
		CreateNewVar("nextGame[" + std::to_string(i) + "]", 0);
		CreateNewVar("dcBonusCount[" + std::to_string(i) + "]", 0);
		CreateNewVar("dcBonusRateX10[" + std::to_string(i) + "]", 1000);
	}
	for (int i = 0; i < 3; ++i)
		CreateNewVar("reelDetailPos[" + std::to_string(i) + "]", 0);

	for(int i=0; i<8; ++i){
		CreateNewVar("dcHistGame[" + std::to_string(i) + "]", -1);
		CreateNewVar("dcHistType[" + std::to_string(i) + "]", -1);
		CreateNewVar("dcHistGet[" + std::to_string(i) + "]", -1);
	}

	return true;
}

// [act]システム変数の更新を行う
// [prm]pIntData	: 更新に使用する内部情報管理クラス
// [ret]更新が正常に行われたかどうか
bool CEffectVariableManager::Process(CSlotInternalDataManager& pIntData, const SSlotGameDataWrapper& pDataWrapper) {
	const auto& pCastChecker = pDataWrapper.castChecker;
	const auto& pReelManager = pDataWrapper.reelManager;
	const auto& pDataCounter = pDataWrapper.dataCounter;
	const auto data = pIntData.GetData();

	SetVarVal("set", data.set);
	SetVarVal("flagID", data.flag.first);
	SetVarVal("bonusID", data.flag.second);
	SetVarVal("gameMode", data.gameMode);
	SetVarVal("modeLim[0]", data.gameModeLimit.first);
	SetVarVal("modeLim[1]", data.gameModeLimit.second);
	SetVarVal("isGameLim[0]", data.isGameLimit.first ? 1 : 0);
	SetVarVal("isGameLim[1]", data.isGameLimit.second ? 1 : 0);
	SetVarVal("countOnlyJac", data.isGetCountOnlyJac ? 1 : 0);
	SetVarVal("nextGame[0]", data.gameModeAtEnd.first);
	SetVarVal("nextGame[1]", data.gameModeAtEnd.second);
	SetVarVal("nowRTMode", data.rtMode.first);
	SetVarVal("nowRTLimit", data.rtMode.second);
	SetVarVal("nextRTMode", data.rtModeAtModeEnd.second);
	SetVarVal("nextRTLimit", data.rtModeAtModeEnd.second);
	SetVarVal("canOverwriteRT", data.isRtOverrideEnable ? 1 : 0);
	SetVarVal("betNum", data.betNum);
	SetVarVal("inCount", data.inCount);
	SetVarVal("outCount", data.outCount);
	SetVarVal("nowMedalCount", data.outCount - data.inCount);
	SetVarVal("isReplay", data.isReplay ? 1 : 0);
	SetVarVal("credit", data.credit);
	SetVarVal("payout", data.payout);
	SetVarVal("payoutEffect", pCastChecker.GetPayoutEffect());
	SetVarVal("payoutLine", pCastChecker.GetPayoutLineID());
	SetVarVal("reachSound", pCastChecker.GetReachSoundID());

	for (int i = 0; i < 3; ++i)
		SetVarVal("reelDetailPos[" + std::to_string(i) + "]", pReelManager.GetComaDetailPos(i));

	/* dataCounter */ {
		const auto basicData = pDataCounter.GetBasicData();
		SetVarVal("dcTotalG", basicData.totalGame);
		SetVarVal("dcStartG", basicData.startGame);
		SetVarVal("dcPayoutX10", basicData.payoutRateX10);

		for (int i = 0; i < 2; ++i) {
			SetVarVal("dcBonusCount[" + std::to_string(i) + "]", pDataCounter.GetBonusCount(i + 1));
			SetVarVal("dcBonusRateX10[" + std::to_string(i) + "]", pDataCounter.GetBonusRateX10(i + 1));
		}
		
		for (int i = 0; i < 8; ++i) {
			const auto hist = pDataCounter.GetBonusHistory(i);
			SetVarVal("dcHistGame[" + std::to_string(i) + "]", hist.startGame);
			SetVarVal("dcHistType[" + std::to_string(i) + "]", hist.getPayoutEffect);
			if(hist.isSetGet)	SetVarVal("dcHistGet[" + std::to_string(i) + "]", hist.medalAfter - hist.medalBefore);
			else				SetVarVal("dcHistGet[" + std::to_string(i) + "]", -1);
		}

	}

	// pIntDataへの登録
	pIntData.SetPayoutFreezeTime(GetVal(GetValIDFromName("$payoutFreeze")));
	pIntData.SetBetFreezeTime(GetVal(GetValIDFromName("$betFreeze")));
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
	mVariableName.push_back(std::pair<std::string, int>(registName, CreateNewConstant(pInitVal)));
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

// [act]変数名からIDの取得を行う
//		<Throwable>ErrUndeclaredVar
// [prm]pValName	: 変数名(変数を示す$マークが必須) または 文字列で表した数値
// [ret]変数ID
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
int CEffectVariableManager::GetVal(int pValID) const{
	if (pValID < 0 || pValID >= mVariablePool.size()) throw ErrUndeclaredVar("id: " + std::to_string(pValID));
	return mVariablePool[pValID];
}

// [act]DxLib描画画面を新規生成する
// [prm]pValName	: 変数ID
//		<引数は仮>
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
		// 直接代入する
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

		// 代入数一致確認→代入
		if (pData.roleData[lookingID].second.size() != pData.inputForList.size()) return false;
		for (size_t i = 0; i < pData.inputForList.size(); ++i) {
			const auto putVal = GetVal(pData.roleData[lookingID].second[i]);
			// 直接代入する
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

bool CEffectVariableManager::ReadRestore(CRestoreManagerRead& pReader) {
	size_t registNum = 0;
	if (!pReader.ReadNum(registNum)) return false;
	for (size_t i = 0; i < registNum; ++i) {
		std::string name;
		int value = 0;
		if (!pReader.ReadStr(name)) return false;
		if (!pReader.ReadNum(value)) return false;
		try {
			SetVarVal(name, value);
		}
		catch (ErrUndeclaredVar e) { return false; }
	}
	return true;
}

bool CEffectVariableManager::WriteRestore(CRestoreManagerWrite& pWriter) const {
	if (!pWriter.WriteNum(mVariableName.size())) return false;
	for (const auto& data : mVariableName) {
		if (!pWriter.WriteStr(data.first)) return false;
		try {
			if (!pWriter.WriteNum(GetVal(data.second))) return false;
		} catch (ErrUndeclaredVar e) { return false; }
	}
	return true;
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

		// 計算部分
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

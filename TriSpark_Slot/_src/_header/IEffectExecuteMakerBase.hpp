#pragma once
#include <string>
class CEffectVariableManager;

// データ作成用クラス
class IEffectExecuteMakerBase {
protected:
	int GetVariableID(std::string pData, bool pIsOnlyVar, CEffectVariableManager& pManager);
};

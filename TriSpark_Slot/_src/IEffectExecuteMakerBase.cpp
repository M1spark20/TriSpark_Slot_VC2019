#include "_header/IEffectExecuteMakerBase.hpp"
#include "_header/CEffectVariableManager.hpp"
#include "_header/ErrClass.hpp"

int IEffectExecuteMakerBase::GetVariableID(std::string pData, bool pIsOnlyVar, CEffectVariableManager& pManager) {
	if (pData[0] == '$') {
		return pManager.GetValIDFromName(pData);
	}
	if (pIsOnlyVar) throw ErrUndeclaredVar("Variable Updating - " + pData);
	if (pData[0] >= '0' && pData[0] <= '9' || pData[0] == '-') {
		return pManager.MakeValID(pData);
	}
	throw ErrUndeclaredVar("Variable Updating - " + pData);
	return false;
}
#pragma once
#include <string>
class CEffectVariableManager;

// �f�[�^�쐬�p�N���X
class IEffectExecuteMakerBase {
protected:
	int GetVariableID(std::string pData, bool pIsOnlyVar, CEffectVariableManager& pManager);
};

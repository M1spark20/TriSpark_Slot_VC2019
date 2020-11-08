#pragma once
#include "CImageColorManager.hpp"
#include <string>
#include <vector>
class CSlotTimerManager;

struct SImageColorApplyData {
	std::string applyTo;
	std::vector<std::string> addCMName;
	bool addFlag;
};

class CImageColorApplyDataBuilder {
	SImageColorApplyData mApplyData;
public:
	CImageColorApplyDataBuilder() { mApplyData.addFlag = false; }
	bool Init(std::vector<std::string> pStringArr);
	SImageColorApplyData Extract() { return mApplyData; };
};

class CImageColorController {
	std::vector<CImageColorManager>											mColorData;		// color�f�[�^�ꗗ
	std::vector<std::pair<std::string,std::vector<CImageColorManager*>>>	mActionData;	// ����f�[�^��colorImage���R�s�[

	CImageColorManager&					GetColorDataByID(std::string pColorDataName);
	std::vector<CImageColorManager*>&	GetActionDataByID(std::string pUseEffectName);
	bool								CheckAvailableName(std::string pData);

public:
	// [act]csv�ǂݍ��ݎ��ɂ�����colorData��`�ǉ�
	void							AddColorData(CImageColorManager& pData) { mColorData.push_back(pData); }
	// [act]�A�N�V�����f�[�^ID�V�K�쐬
	void							CreateActionData(std::string pUseEffectName);
	// [act]ID����A�N�V�����f�[�^��ǉ�
	void							AddActionData(const SImageColorApplyData& pData);
	// [act]�w��ID�̃A�N�V�����f�[�^���N���A
	void							ResetActionData(std::string pUseEffectName);
	// [act]�`��Ɏg�p����color�f�[�^���Ăяo��
	const CImageColorManager *const	GetColorData(std::string pUseEffectName, int pIndex);
	// [act]�w�肵��ID�̑S�f�[�^�ɑ΂��ă^�C�}�[���Z�b�g����
	bool							SetTimerAll(CSlotTimerManager& pTimer);
	// [act]�w�肵��ID�̑S�f�[�^�ɑ΂��ă^�C�}�[�����Z�b�g����
	void							ResetTimerAll();
};
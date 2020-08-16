#pragma once
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class IImageDestinationManager {
private:
	EDrawModeForDST  GetDrawEnum(std::string pIndex);
	EBlendModeForDST GetBlendEnum(std::string pIndex);

protected:
	std::vector<SImageDestCSVCommonData> mCommonData;		// csv����ǂ݂�����dst�f�[�^�A������`�\
	std::string mTimerID;									// �ǂݏo���A�j���[�V�����E�������������^�C�}ID
	const int* m_pLoopTime;									// �^�C�}ID�̃��[�v�_ (-1�Ń��[�v����)
	const long long* (* const mTimerReader)(std::string);	// �^�C�}�l�Ăяo���p�֐��|�C���^
	int (* const mScreenManager)(std::string);				// �`����ʌĂяo���p�֐��|�C���^

	typedef std::vector<std::string> StringArr;

	// [act]���[�v�_���l����������Ɏg�p���鎞�Ԃ�����o��
	long long GetCheckTime(const long long pNowCount);
	// [act]�^�C�}�[�󋵂���ǂݏo���^�C�~���O�Ŏg�p����`���`�����肷��
	int GetDefinitionIndex();

	int GetDxDrawModeByEnum(EDrawModeForDST pData);
	int GetDxBlendModeByEnum(EBlendModeForDST pData);
	bool GetCanDrawDirectly(EBlendModeForDST pData);

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	IImageDestinationManager(const long long* (* const pTimerReader)(std::string), int (* const pScreenManager)(std::string));
	// [act]������z��"pReadData"����src�f�[�^���擾����
	virtual bool	Init(StringArr pReadData, int* (* const pVariableManager)(std::string));
	// [act]
	virtual void	Draw(SDrawImageSourceData(* const pSourceGetter)(int, int)) = 0;
};

class CImageDestinationDefault : public IImageDestinationManager {
	int* m_pDrawNum;
	int  *m_pDiffX, *m_pDiffY;

	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageDestinationDefault(const long long* (* const pTimerReader)(std::string), int (* const pScreenManager)(std::string));
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool	Init(StringArr pReadData, int* (* const pVariableManager)(std::string)) override;
	// [act]�`����s��
	void	Draw(SDrawImageSourceData (*const pSourceGetter)(int, int)) override;
};
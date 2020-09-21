#pragma once
#include "CEffectImageCommonComponent.hpp"
#include "SDrawImageSourceData.hpp"
#include "SImageDrawCSVData.hpp"
#include <string>
#include <vector>

class CEffectVariableManager;
class CSlotTimerManager;
class IImageSourceManager;
class CImageColorManager;
class CGameDataManage;
class CReelManager;

class IImageDestinationManager : public CEffectImageCommonComponent {
private:
	EDrawModeForDST  GetDrawEnum(std::string pIndex);
	EBlendModeForDST GetBlendEnum(std::string pIndex);

protected:
	std::vector<SImageDestCSVCommonData> mCommonData;		// csv����ǂ݂�����dst�f�[�^�A������`�\

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
	IImageDestinationManager(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	virtual bool	Init(StringArr pReadData, CSlotTimerManager& pTimerData);
	// [act]
	virtual void	Draw(IImageSourceManager *const pSourceData, CImageColorManager* pColorData, CGameDataManage& pDataManager) = 0;
};

class CImageDestinationDefault : public IImageDestinationManager {
	int mDrawNum;
	int mDiffX, mDiffY;

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageDestinationDefault(CEffectVariableManager& pVarManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool	Init(StringArr pReadData, CSlotTimerManager& pTimerData) override;
	// [act]�`����s��
	void	Draw(IImageSourceManager *const pSourceData, CImageColorManager* pColorData, CGameDataManage& pDataManager) override;
};

class CImageDestinationReel : public IImageDestinationManager {
	const CReelManager& cReelManager;
	struct {
		int preDrawScreenID;
		int blewNum, blewTime;
		int srcX, srcY, srcW, srcH;
		int reelID;
		int comaNum, originComa, comaBegin, comaIndexMax;
	} mExtraData;

public:
	// [act]�ϐ��̏������ƃ^�C�}�l�Ăяo���p�֐��|�C���^�̐ݒ���s��
	CImageDestinationReel(CEffectVariableManager& pVarManager, const CReelManager& pReelManager);
	// [act]������z��"pReadData"����src�f�[�^���擾����
	bool	Init(StringArr pReadData, CSlotTimerManager& pTimerData) override;
	// [act]�`����s��
	void	Draw(IImageSourceManager *const pSourceData, CImageColorManager* pColorData, CGameDataManage& pDataManager) override;
};
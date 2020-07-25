#pragma once
#include "IGameStateBase.hpp"
#include "CReelManager.hpp"
#include "CReelController.hpp"
#include "SCastData.hpp"
#include "CRandManager.hpp"
#include "SReelControlData.hpp"
class CGameDataManage;

class CGameState_ControlTestMain : public IGameStateBase{
// [act]����e�X�g�E���[�`�ڊm�F���[�h���C������
	typedef std::vector<std::vector<std::vector<std::vector<char>>>> tStopMatrix;

	CReelManager		m_reelManager;
	CRandManager		m_randManager;

	static const int	STOP_LIST_NUM_PER_PAGE = 20;

	// ���������o
	std::vector<int>	m_pushPos;			// �f�[�^�\�����̃{�^�������ʒu
	std::vector<int>	m_pushOrder;		// �f�[�^��́E�������̉�������`
	std::vector<int>	m_checkedFlagID;	// ��͂��s���t���OID�ꗗ
	std::vector<short>	m_ctrlResult;		// �O�������E�S��~�`�̒�~�ʒu����
	std::vector<int>	m_stopPos;			// �f�[�^�\�����̃��[����~�ʒu�����o����
	std::vector<int>	m_reachCount;		// �e���[��1st���̃��[�`�ڐ�
	int					m_selectingReel;	// �f�[�^�\�����̑I�𒆃��[����`
	int					m_selectingFlag;	// �f�[�^�\�����̑I�𒆃t���O
	int					m_orderID;			// �f�[�^��͎��̉������t���O
	int					m_orderIDMax;		// �������̐�(���[�����̊K��)
	tStopMatrix			m_stopMatrix;		// �S��~�`�̒�~�󋵕ۑ�(���[�`�ړ�) [order][1st][col][row]
	std::vector<int>	m_stopList;			// �����~�`���������މ����ʒu���L�^����ϐ�(�v�����̂ݍ쐬)
	unsigned int		m_stopListPage;		// ��~�`�\�����̃y�[�W�ԍ�
	bool				m_stopListPushStop;	// ��~�`�\�����̈ʒu���(true:stop,false:push)

	// internal method
	void ReelMove(bool isUpper);
	void SelectReelChange(bool isRight);
	unsigned int PowInteger(unsigned int v1, unsigned int v2){
		if (v2 == 0) return 1;
		unsigned int defVal = v1;
		for (unsigned int i = 0; i < v2-1; ++i) v1 *= defVal;
		return v1;
	}

public:
	bool Init(CGameDataManage& pDataManageIns) override;
	EChangeStateFlag Process(CGameDataManage& pDataManageIns) override;
	bool Draw(CGameDataManage& pDataManageIns) override;
};
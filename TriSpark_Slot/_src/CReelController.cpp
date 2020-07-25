#include "_header\CReelController.hpp"
#include "_header\CGameDataManage.h"
#include "_header\CReelControlReaderFromCSV.hpp"

bool CReelController::Init(const CGameDataManage& pDataManager, int pFileID, int reelNum){
	m_orderID = e___;
	m_reelNum = reelNum;
	CReelControlReaderFromCSV ctrlReader;
	const SMainReadFileIndex *const fileData = pDataManager.GetDataIndex(pFileID);
	if (fileData == nullptr) return false;
	ctrlReader.FileInit(fileData);
	ctrlReader.MakeData(m_ctrlData, reelNum);
	return true;
}

int CReelController::GetStopPosition(int pFlagID, int pBonusID, int pPushReel, int pPushComaID){
//	[prm]pPushReel	: ���������[����ID
	const int comaNum = 21;
	if (m_stopReelNum.size() >= (unsigned int)m_reelNum) return -1;
	unsigned long long dataBuf;
	unsigned int ctrlID, ctrlType;
	bool confirm = false;

	// ���Y�t���O�f�[�^�T��
	for (ctrlID = 0; ctrlID < m_ctrlData.header.size(); ++ctrlID){
		if (m_ctrlData.header[ctrlID].flagID != pFlagID) continue;
		if (m_ctrlData.header[ctrlID].bonusID != pBonusID && m_ctrlData.header[ctrlID].bonusID != -1)
			continue;
		ctrlType = m_ctrlData.header[ctrlID].ctrlType;
		confirm = true;	break;
	}
	if (!confirm) return -1;

	// 1st or ctrlType=4
	if (m_stopReelNum.size() == 0 || ctrlType == 4){
		dataBuf = m_ctrlData.command[ctrlID].slipData[pPushReel];
		if(m_stopReelNum.size()==0) m_orderID = (EOrderID)(e1__ + pPushReel);
	} else {
		// 2nd�Ȃ�܂����������m�肳����
		if (m_stopReelNum.size() == 1){
			for (int i = 0; i < m_reelNum; ++i){
				if (i + e1__ == m_orderID) continue;
				m_orderID = (EOrderID)(e123 + (m_orderID - e1__) * 2);
				if (i != pPushReel) m_orderID = (EOrderID)(m_orderID + 1);
				break;
			}
		}
		// type=3 -> 1st�̃��[���ƒ�~�ʒu�Ŕ���
		if (ctrlType == 3){
			confirm = false;
			for (unsigned int slipNo = 0; slipNo < m_ctrlData.command[ctrlID].spotData.size(); ++slipNo){
				// ������������(1st�����v����΂悢 -> m_orderID��2�Ŋ����Č���)
				const SReelControlSpot& spotData = m_ctrlData.command[ctrlID].spotData[slipNo];
				if ((spotData.spot >> 5 & 0x07) != m_orderID / 2) continue;
				// 1st�̒�~�ʒu������
				if ((spotData.spot & 0x1F) != m_stopReelNum[0][1] && (spotData.spot & 0x1F) != 0x1F) continue;
				// �I�t�Z�b�g����
				unsigned int offset = spotData.slipStart;
				for (int i = 0; i < m_reelNum; ++i){
					if (i + e123 == m_orderID/2) continue;	// 1st�ƈقȂ�
					if (i != pPushReel) ++offset;			// ���������[���ƈقȂ�ꍇoffset��1���₷
					break;
				}
				dataBuf = m_ctrlData.command[ctrlID].slipData[offset];
				confirm = true;	break;
			}
			if (!confirm) return -1;
		}
		// type=0 or type=1��2nd -> ���������[����1st�̉����ʒu�Ŕ���
		else if (ctrlType == 0 || ctrlType == 1 && m_stopReelNum.size() == 1){
			const unsigned int slipNo = (m_stopReelNum.size() == 1) ? 
				3 + m_orderID * comaNum + m_stopReelNum[0][0] : 
				3 + comaNum*6 + comaNum*comaNum*m_orderID + comaNum*m_stopReelNum[0][0] + m_stopReelNum[1][0] ;
			dataBuf = m_ctrlData.command[ctrlID].slipData[slipNo];
		}
		// type=2 or type=1��3rd -> order��������������̃f�[�^�ɂď���,2nd������s��
		else {
			confirm = false;
			bool check2nd;
			for (unsigned int slipNo = 0; slipNo < m_ctrlData.command[ctrlID].spotData.size(); ++slipNo){
				const SReelControlSpot& spotData = m_ctrlData.command[ctrlID].spotData[slipNo];
				// ��2��~ or ��3��~�̎��ʂ��s��
				check2nd = (spotData.spot & 0x8000) == 0x8000;	// MSB�t���O: 2nd�p����
				check2nd ^= !(m_stopReelNum.size() == 1);		// ��~��2nd�ł͂Ȃ�
				if (!check2nd) continue;						// ��L2���������Ⴕ���ꍇcontinue (=XOR)
				// ������������
				if ((spotData.spot >> 10 & 0x07) != m_orderID) continue;
				// 1st�̒�~�ʒu������
				if ((spotData.spot >> 5 & 0x1F) != m_stopReelNum[0][1] && (spotData.spot >> 5 & 0x1F) != 0x1F) continue;
				// 2nd�̒�~�ʒu������(3rd�̐�������肷��ꍇ�̂�)
				if (m_stopReelNum.size() == 2){ if ((spotData.spot & 0x1F) != m_stopReelNum[1][1] && (spotData.spot & 0x1F) != 0x1F) continue; }
				dataBuf = m_ctrlData.command[ctrlID].slipData[spotData.slipStart];
				confirm = true;	break;
			}
			if (!confirm) return -1;
		}
	}

	// dataBuf���炷�ׂ�R�}����ǂݏo��
	const int slipVal = dataBuf >> (3 * (comaNum - pPushComaID - 1)) & 0x07;
	const int ans = (comaNum + pPushComaID - slipVal) % comaNum;
	std::array<char, 2> addData;
	addData[0] = pPushComaID;	addData[1] = ans;
	m_stopReelNum.push_back(addData);
	return ans;
}

void CReelController::ResetReelData(){
	m_orderID = e___;
	m_stopReelNum.clear();
}

int CReelController::GetStopPos(int pPushReel, int pPushPos, int pCharaMax){

	return 0;
}

bool CReelController::SetTableNo(int pFlagID, int pBonusID, std::vector<int>& pOrder, std::vector<int>& pStoppedPos, int pPushReel){
	return true;
}
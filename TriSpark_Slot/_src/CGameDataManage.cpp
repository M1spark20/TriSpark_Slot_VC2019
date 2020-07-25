#include "DxLib.h"
#include "_header\CGameDataManage.h"
#include <cstdlib>
#include <sstream>
bool CGameDataManage::StartReadFile(ArrayIndex &InputData){
// [prm]p1;�f�[�^�C���f�b�N�X�̎Q��
// [act]�f�[�^�C���f�b�N�X���R�s�[���A���ꂼ��̃t�@�C���̓ǂݍ��݂��J�n����
//		�Ȃ������̓t�@�C���ǂݍ��ݒ��ɍs���ƃG���[���Ԃ�悤�Ȃ̂ŁA
//		�f�[�^�`�F�b�N���ɂ܂Ƃ߂ĕ������s���܂��B
	bool Ans=true;
	for(unsigned int MainC=0;MainC<InputData.size();MainC++){
		m_MainData.push_back(InputData[MainC]);
		const int GetSyncFlag=(m_MainData[MainC].DataReadSyncFlag == 0) ? FALSE:TRUE;
		// 0��false�A����ȊO�Ȃ�true�B�񓯊��ǂݍ��ݐݒ�
		DxLib::SetUseASyncLoadFlag(GetSyncFlag);
		const std::string nowReadFileName = m_MainData[MainC].FileName;
		m_MainData[MainC].FileSize = DxLib::FileRead_size(nowReadFileName.c_str());
		if (m_MainData[MainC].FileSize == 0){
			ErrorLogFmtAdd("IndexNo: %d, Name: \"%s\"���󂩌�����܂���B", MainC, nowReadFileName.c_str());
		}
		switch(m_MainData[MainC].FileType){
		case Graphic:
			// �v���O�������Ŏ��R�ɏ��������\
			m_MainData[MainC].DataHandle = 
				DxLib::LoadGraph(nowReadFileName.c_str());
			break;
		case Sound:
			SetCreateSoundDataType(m_MainData[MainC].SoundStreamingFlag);
			m_MainData[MainC].DataHandle = 
				DxLib::LoadSoundMem(nowReadFileName.c_str());
			break;
		case Material:
			m_MainData[MainC].DataHandle = 
				DxLib::MV1LoadModel(nowReadFileName.c_str());
			break;
		case Text:
		case Binary:
			// Async�̃t���O��TRUE�Ƃ���ƃ��������[�N�����������̂ŏȗ�
			// SetUseASyncLoadFlag()�Ŋ��ɔ񓯊��ǂݍ��݂̐ݒ�͊������Ă��邩��?
			m_MainData[MainC].DataHandle = 
				DxLib::FileRead_open(nowReadFileName.c_str());
			break;
		case Copy:
			// �����͓ǂݍ��݌�Ɏ��s���邱�ƂɂȂ�܂�
			break;
		default:
			ErrorLogFmtAdd("IndexNo.%d��Type�l���ُ�ł��B",MainC);
			Ans = false;
		}
	}
	DxLib::SetUseASyncLoadFlag(FALSE);
	return Ans;
}
bool CGameDataManage::CheckReadFile(){
// [act]�S�Ă̔񓯊��t�@�C���ǂݍ��݂��I�������Ɋ֐����Ăяo���B
//		�t�@�C��������ɓǂݍ��߂����`�F�b�N����B
//		�����w�肳�ꂽ�t�@�C���𕡐�����B
// [ret]�S�Ă̔񓯊��ǂݍ��݃t�@�C��������ɓǂݍ��߂����ǂ���
	bool Ans=true;
	for(unsigned int MainC=0;MainC<m_MainData.size();MainC++){
		if(m_MainData[MainC].FileType!=Copy){
			if(m_MainData[MainC].DataReadSyncFlag == 0) continue;
			const int Handle = m_MainData[MainC].DataHandle;
			if(CheckHandleASyncLoad(Handle) == -1){
				DxLib::ErrorLogFmtAdd("IndexNo.%d,�t�@�C����\"%s\"�̓ǂݍ��݂Ɏ��s���܂����B",
					MainC, m_MainData[MainC].FileName.c_str());
				m_MainData[MainC].DataHandle = -1;
				Ans = false;
			}
		} else {
			int ForkID;
			/* ForkID��` */{
				std::stringstream Temp(m_MainData[MainC].FileName);
				Temp>>ForkID;
			}
			if(ForkID<0||ForkID>=(signed)MainC){
				DxLib::ErrorLogFmtAdd("IndexNo.%d��Copy��ID���ُ�ł��B",MainC);
				Ans = false;
				break;
			}
			switch(m_MainData[ForkID].FileType){
			case Graphic: //�����@�\���Ȃ����߃n���h�����R�s�[
				m_MainData[MainC].DataHandle = m_MainData[ForkID].DataHandle;
				DxLib::ErrorLogFmtAdd("IndexNo.%d��Copy�������ł��B�����n���h����n���܂��B",MainC);
				break;
			case Sound:
				// ����ᔽ�ŕ����Ɏ��s������-1���Ԃ��Ă�����Ď��M���Ă�(((
				m_MainData[MainC].DataHandle = 
					DxLib::DuplicateSoundMem(m_MainData[ForkID].DataHandle);
				if(m_MainData[MainC].DataHandle == -1){
					m_MainData[MainC].DataHandle = m_MainData[ForkID].DataHandle;
					DxLib::ErrorLogFmtAdd("IndexNo.%d��Copy�Ɏ��s���܂����B�����n���h����n���܂��B",MainC);
				}
				break;
			case Material:
				m_MainData[MainC].DataHandle =
					DxLib::MV1DuplicateModel(m_MainData[ForkID].DataHandle);
				break;
			case Text:
			case Binary: //�����@�\���Ȃ����߃n���h�����R�s�[
				m_MainData[MainC].DataHandle = m_MainData[ForkID].DataHandle;
				DxLib::ErrorLogFmtAdd("IndexNo.%d��Copy�������ł��B�����n���h����n���܂��B",MainC);
				break;
			default:
				DxLib::ErrorLogFmtAdd("IndexNo.%d��Copy��ID���ُ�ł��B",MainC);
				Ans = false;
				break;
			}
		}
	}
	return Ans;
}
int CGameDataManage::GetDataHandle(unsigned int DataID) const{
// [act]�w�肳�ꂽ�f�[�^ID�̃f�[�^�n���h����Ԃ�
// [ret]�w�肳�ꂽID�̃f�[�^�n���h��
//		�������͈͊O�A�܂��͓ǂݍ��݂Ɏ��s�����t�@�C���Ȃ�-1��Ԃ�
	if(DataID>=m_MainData.size()) return -1;
	return m_MainData[DataID].DataHandle;
}
const SMainReadFileIndex* CGameDataManage::GetDataIndex(unsigned int DataID) const{
// [act]�w�肳�ꂽ�f�[�^ID�̃f�[�^�C���f�b�N�X��Ԃ�
// [ret]�w�肳�ꂽID�̃f�[�^�C���f�b�N�X�̃|�C���^
//		�������͈͊O�Ȃ�NULL��Ԃ�
	if(DataID>=m_MainData.size()) return nullptr;
	return &(m_MainData[DataID]);
}
bool CGameDataManage::DeleteData(unsigned int DeletePos,EMainReadFileType DataType){
// [act]�w��̃^�C�v�ɑ΂��A1�Ԗڂ̈����Ɋi�[����Ă���f�[�^���������
//		�������R�s�[�̏ꍇ�͌Ăяo���O�ɑΏۂ̃f�[�^�^�C�v���i�[���邱��
//		�R�s�[�^�C�v�������Ɏw�肳�ꂽ�ꍇ�͏����Ɏ��s�����Ƃ݂Ȃ��B
	if(DeletePos >= m_MainData.size()){
		const char* Err = "�t�@�C��������ɕs���ȃn���h�����󂯎��܂����B\n";
		DxLib::ErrorLogAdd(Err);
		return false;
	}
	const int DataHandle = m_MainData.at(DeletePos).DataHandle;
	switch(DataType){
	case Graphic:
		DxLib::DeleteGraph(DataHandle);
		break;
	case Sound:
		DxLib::DeleteSoundMem(DataHandle);
		break;
	case Material:
		DxLib::MV1DeleteModel(DataHandle);
		break;
	case Text:
	case Binary:
		DxLib::FileRead_close(DataHandle);
		break;
	case Copy:
		const char* Err = "Copy�t���O��DeleteData�ł͉���ł��܂���B\n";
		DxLib::ErrorLogAdd(Err);
		return false;
		break;
	}
	return true;
}
CGameDataManage::~CGameDataManage(){
// [act]���C���œǂݍ��܂ꂽ�f�[�^�̉�����s��
	DxLib::SetUseASyncLoadFlag(FALSE);
	for(unsigned int MainC=0;MainC<m_MainData.size();MainC++){
		if(m_MainData.at(MainC).FileType == Copy){
			std::stringstream Temp(m_MainData.at(MainC).FileName);
			int ForkID;
			Temp>>ForkID;
			DeleteData(MainC,m_MainData.at(ForkID).FileType);
		} else {
			DeleteData(MainC,m_MainData.at(MainC).FileType);
		}
	}
}
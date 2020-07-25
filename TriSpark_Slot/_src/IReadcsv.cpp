#include <fstream>
#include <iostream>
#include "_header\IReadcsv.h"
IReadCSVBase::IReadCSVBase(){
	m_ReadDataNowCurrent = 0;
}
bool IReadCSVBase::StartReadFile(const char* FileName){
// [act]�����o�ϐ�m_ReadDataAll�Ƀt�@�C���̕���������ׂēǂݍ���
// [ret]�t�@�C�����ǂݍ��߂����ǂ���
	const int ReadLineStrMax = 16384;// 1�s����ǂݍ��ލő�̕�����
	std::ifstream File(FileName);
	if(!File){// �t�@�C�����ǂݍ��߂Ȃ������Ƃ�
		std::cout << "CSV�t�@�C��\"" << FileName <<"\"���ǂݍ��߂܂���ł����B" << std::endl;
		return false;
	}
	while(!File.eof()){
		m_ReadDataAll += (char)File.get();
	}
	m_ReadDataAll += '\0';
	return true;
}
void IReadCSVBase::GetStrSplitByComma(StringArr& StrArray){
// [prm]p1;�߂�l�Ƃ��Ĉ����B�e�z��ɍ�����R���}�ŋ�؂�ꂽ�����񂪓���B
// [act]����������̍s�܂ŃR���}�ŋ�؂��āA������e�z��ɑ�����ĕԂ��B
//		�R�����g�A�E�g�͋L�q���ꂸ�Ɏ��̍s�܂œǂݔ�΂����B
//		�R�����g�s�͋�̕�����̕���1��push_back�����B
	bool PastSlash=false;
	std::string NowStr;
	while(true){
		if(PastSlash&&m_ReadDataAll[m_ReadDataNowCurrent]!='/'){
			NowStr += '/';
			PastSlash=false;
		}
		switch(m_ReadDataAll[m_ReadDataNowCurrent]){
		case '\n':	// ���s
		case '\0':	// �f�[�^�̏I���̏ꍇ�������ŏ���
			StrArray.push_back(NowStr);
			m_ReadDataNowCurrent++;
			return;
		case '/':
			if(PastSlash){
				StrArray.push_back(NowStr);
				do { 
					m_ReadDataNowCurrent++;
					if(DataEOF())break;
				}while(m_ReadDataAll[m_ReadDataNowCurrent-1]!='\n');
				return;
			}
			PastSlash = true;
			break;
		case ',':
			StrArray.push_back(NowStr);
			NowStr="";
			break;
		default:
			NowStr += m_ReadDataAll[m_ReadDataNowCurrent];
			break;
		}
		m_ReadDataNowCurrent++;
	}
}
int IReadCSVBase::GetNowCurrent() const{
	return m_ReadDataNowCurrent;
}
void IReadCSVBase::SetNowCurrent(int NewCurrent){
	m_ReadDataNowCurrent = NewCurrent;
}
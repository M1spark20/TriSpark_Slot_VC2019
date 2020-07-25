#pragma once
#include <sstream>
#include <vector>
typedef std::vector<std::string> StringArr;
class IReadCSVBase {
// [act]csv�ǂݍ��ݐ���N���X�쐬���̊��N���X
protected:
// �ǂݍ��񂾃t�@�C���̕�������i�[����
	std::string m_ReadDataAll;
// ���݂̓ǂݍ��݃J�����g�̏ꏊ
	unsigned int m_ReadDataNowCurrent;
	IReadCSVBase();
	bool StartReadFile(const char* FileName);
	void GetStrSplitByComma(StringArr& StrArray);
	int GetNowCurrent() const;
	void SetNowCurrent(int NewCurrent);
	bool DataEOF() const{
		return (m_ReadDataNowCurrent>=m_ReadDataAll.size()-1);
	}
	template<class T> void StrToNum(T& InputFor,std::string &p_Data){
// [act]�e���v���[�g�^�A1�ڂ̈�����2�ڂ̕����񂩂璊�o���������������܂�
//		bool�^�̏ꍇ�͐����Ŕ��ʂ��A0��false�A����ȊO��true�ƂȂ�͗l�ł�
		std::istringstream Data(p_Data);
		Data>>InputFor;
	};

	virtual ~IReadCSVBase(){};
};
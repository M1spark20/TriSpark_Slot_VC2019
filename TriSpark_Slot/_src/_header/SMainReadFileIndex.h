#pragma once
#include <string>
enum EMainReadFileType{
// [act]�Q�[���{�̂œǂݍ��ރt�@�C���̎�ނ��w��
//		�ǂ�œǂݍ��ނ���CSV��Type�Ŏw��
	Graphic, Sound, Material, Text, Binary, Copy
};
struct SMainReadFileIndex{
// [act]�Q�[���{�̂œǂݍ��ރt�@�C���̏��Q�̊��\����
	EMainReadFileType FileType;
	std::string FileName;
	int DataReadSyncFlag;
	int DataHandle;
	int SoundStreamingFlag;
	unsigned long long FileSize;
};
#pragma once
#include <vector>

struct SReelChaUnit{
	int charaID;
	int imageID;
	int x, y, w, h;
};

struct SReelChaData{
	std::vector<SReelChaUnit>	reelData;	// ���[���؂�o���ʒu(���ڕ`��p)
	int							reelID;		// ���[��ID
	int							rpm;		// ���[����]���x
	int							accTime;	// ���[����������
	std::vector<int>			arrayData;	// ���[���z��f�[�^
};
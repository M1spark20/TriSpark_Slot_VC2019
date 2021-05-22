#include "_header/CMenuReadBonusTypeFromCSV.hpp"
#include "DxLib.h"

bool CMenuReadBonusTypeFromCSV::FileInit(int pFileID) {
	DxLib::FileRead_seek(pFileID, 0, SEEK_SET);
	while (!DxLib::FileRead_eof(pFileID)) {
		TCHAR str[1024];
		DxLib::FileRead_gets(str, 1024, pFileID);
		m_ReadDataAll.append(str).append("\n");
	}
	return !m_ReadDataAll.empty();
}

bool CMenuReadBonusTypeFromCSV::GetImagePos(std::vector<SMenuReadBonusTypePos>& pData) {
	m_ReadDataNowCurrent = 0;
	int pageCount = -1;

	while (!DataEOF()) {
		StringArr NowGetStr;
		GetStrSplitByComma(NowGetStr);
		if (NowGetStr.size() < 7) continue;
		if (NowGetStr[0] != "#bonusType") continue;
		SMenuReadBonusTypePos ans;
		StrToNum(ans.payoutID, NowGetStr[1]);
		StrToNum(ans.resID, NowGetStr[2]);
		StrToNum(ans.x, NowGetStr[3]);
		StrToNum(ans.y, NowGetStr[4]);
		StrToNum(ans.w, NowGetStr[5]);
		StrToNum(ans.h, NowGetStr[6]);
		pData.push_back(ans);
	}
	return true;
}

#include "_header/CMenuReadHowtoFromCSV.hpp"
#include "DxLib.h"

bool CMenuReadHowtoFromCSV::FileInit(int pFileID) {
	DxLib::FileRead_seek(pFileID, 0, SEEK_SET);
	while (!DxLib::FileRead_eof(pFileID)) {
		TCHAR str[1024];
		DxLib::FileRead_gets(str, 1024, pFileID);
		m_ReadDataAll.append(str).append("\n");
	}
	return !m_ReadDataAll.empty();
}

int CMenuReadHowtoFromCSV::GetImageID(int pageNo) {
	m_ReadDataNowCurrent = 0;
	int pageCount = -1;

	while (!DataEOF()) {
		StringArr NowGetStr;
		GetStrSplitByComma(NowGetStr);
		if (NowGetStr.size() < 2) continue;
		if (NowGetStr[0] != "#howto") continue;
		if (++pageCount == pageNo) {
			int ans;
			StrToNum(ans, NowGetStr[1]);
			return ans;
		}
	}
	return -1;
}

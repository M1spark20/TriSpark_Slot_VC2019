#include "_header/CRestoreManager.hpp"
 
std::string IRestoreManager::GetDefaultFilePath() const {
	char filePath[512] = "\0";
#ifdef __ANDROID__
	GetInternalDataPath(filePath, sizeof(filePath));
#elif __APPLE__
	GetLibraryPreferencesDirPath(filePath, sizeof(filePath));
#else
	strcpy_s(filePath, ".");
#endif
	return std::string(filePath);
}

bool CRestoreManagerRead::StartRead() {
	mIfs.open(GetDefaultFilePath() + cFileName, std::ios::binary);
	if (!mIfs) return false;

	// チェックサム確認
	unsigned char checkSum = 0x00;
	while (mIfs.eof()) {
		unsigned char data;
		ReadNum(data);
		checkSum = (checkSum + data) & 0xFF;
	}
	if (checkSum != 0x00) return false;

	// mDataVersion読み込み, 初期位置設定
	mIfs.clear();
	mIfs.seekg(0);
	ReadNum(mDataVersion);

	return true;
}

bool CRestoreManagerRead::ReadStr(std::string& pInputFor, unsigned int pSize) {
	char data;
	for (unsigned int i = 0; i < pSize; ++i) {
		if(!ReadNum(data)) return false;
		if(data != '\0') pInputFor += data;
	}
	return true;
}

void CRestoreManagerRead::CloseRead() {
	mIfs.close();
	mIfs.clear();
}


bool CRestoreManagerWrite::IsActivate() {
	if (mActivateFlag && !mResetFlag) return false;
	mResetFlag = true;
	mActivateFlag = false;
}

void CRestoreManagerWrite::ActivateFlagReset() {
	mResetFlag = false;
}

void CRestoreManagerWrite::SetActivate() {
	mActivateFlag = true;
}

bool CRestoreManagerWrite::StartWrite() {
	mOfs.open(GetDefaultFilePath() + cFileName, std::ios::binary|std::ios::trunc);
	if (!mOfs) return false;

	mCheckSum = 0x00;
	WriteNum(VERSION);
	return true;
}

bool CRestoreManagerWrite::WriteStr(std::string pStr, unsigned int pSize) {
	char data;
	for (unsigned int i = 0; i < pSize; ++i) {
		if (i >= pStr.size()) data = '\0';
		else data = pStr[i];
		if(!WriteNum(data)) return false;
	}
	return true;
}

bool CRestoreManagerWrite::Flush() {
	if (!WriteNum(~mCheckSum + 0x01)) return false;
	if (mCheckSum != 0x00) return false;
	mOfs.close();
	mOfs.clear();
	return true;
}
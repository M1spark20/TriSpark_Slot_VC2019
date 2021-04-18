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

	// �`�F�b�N�T���m�F
	unsigned char checkSum = 0x00;
	while (mIfs.eof()) {
		unsigned char data;
		ReadNum(data);
		checkSum = (checkSum + data) & 0xFF;
	}
	if (checkSum != 0x00) return false;

	// mDataVersion�ǂݍ���, �����ʒu�ݒ�
	mIfs.clear();
	mIfs.seekg(0);
	ReadNum(mDataVersion);

	return true;
}

bool CRestoreManagerRead::ReadStr(std::string& pInputFor, unsigned int pSize) {
	char data;
	for (unsigned int i = 0; i < pSize; ++i) {
		if(!ReadNum(data)) return false;
		pInputFor += data;
	}
	return true;
}

template<class T>
bool CRestoreManagerRead::ReadNum(T& pInputFor) {
	mIfs.read((char*)&pInputFor, sizeof(pInputFor));
	return (bool)mIfs;
}

void CRestoreManagerRead::CloseRead() {
	mIfs.close();
	mIfs.clear();
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

template<class T>
bool CRestoreManagerWrite::WriteNum(T pValue) {
	mOfs.write((char*)&pValue, sizeof(pValue));
	if(!mOfs) return false;

	// �`�F�b�N�T�����Z
	const char* dataPtr = (char*)&pValue;
	for (unsigned int i = 0; i < sizeof(pValue); ++i) {
		if(i>0) ++dataPtr;
		char data = *dataPtr & 0xFF;
		mCheckSum = (mCheckSum + data) & 0xFF;
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
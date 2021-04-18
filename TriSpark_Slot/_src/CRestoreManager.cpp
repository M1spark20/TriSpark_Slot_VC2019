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
	mIfs.read((char*)&pInputFor, sizeof(T));
	return (bool)mIfs;
}

void CRestoreManagerRead::CloseRead() {
	mIfs.close();
	mIfs.clear();
}

bool CRestoreManagerWrite::StartWrite() {
	mOfs.open(GetDefaultFilePath() + cFileName, std::ios::binary|std::ios::trunc);
	return (bool)mOfs;
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
	mOfs.write((char*)&pValue, sizeof(T));
	return (bool)mOfs;
}

void CRestoreManagerWrite::CloseWrite() {
	mOfs.close();
	mOfs.clear();
}
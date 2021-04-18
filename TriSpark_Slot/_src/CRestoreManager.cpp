#include "_header/CRestoreManager.hpp"
 
std::string CRestoreManager::GetDefaultFilePath() const {
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

bool CRestoreManager::StartRead() {
	mIfs.open(GetDefaultFilePath() + cFileName, std::ios::binary);
	if (!mIfs) return false;
	// チェックサム確認
	return true;
}

bool CRestoreManager::ReadStr(std::string& pInputFor, unsigned int pSize) {
	char data;
	for (unsigned int i = 0; i < pSize; ++i) {
		if(!ReadNum(data)) return false;
		pInputFor += data;
	}
	return true;
}

template<class T>
bool CRestoreManager::ReadNum(T& pInputFor) {
	mIfs.read((char*)&pInputFor, sizeof(T));
	return (bool)mIfs;
}

void CRestoreManager::CloseRead() {
	mIfs.close();
	mIfs.clear();
}

bool CRestoreManager::StartWrite() {
	mOfs.open(GetDefaultFilePath() + cFileName, std::ios::binary|std::ios::trunc);
	return (bool)mOfs;
}

bool CRestoreManager::WriteStr(std::string pStr, unsigned int pSize) {
	char data;
	for (unsigned int i = 0; i < pSize; ++i) {
		if (i >= pStr.size()) data = '\0';
		else data = pStr[i];
		if(!WriteNum(data)) return false;
	}
	return true;

}

template<class T>
bool CRestoreManager::WriteNum(T pValue) {
	mOfs.write((char*)&pValue, sizeof(T));
	return (bool)mOfs;
}

void CRestoreManager::CloseWrite() {
	mOfs.close();
	mOfs.clear();
}
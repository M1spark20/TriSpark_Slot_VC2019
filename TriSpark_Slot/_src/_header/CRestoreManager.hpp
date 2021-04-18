#pragma once
#include <string>
#include <fstream>
#include <vector>

class IRestoreManager {
protected:
	static const int VERSION = 1;
	const std::string cFileName;
	std::string GetDefaultFilePath() const;

public:
	IRestoreManager() : cFileName("/playData.sav") {}
};

class CRestoreManagerRead : public IRestoreManager {
	std::ifstream mIfs;
	int mDataVersion;

public:
	bool StartRead();
	bool ReadStr(std::string& pInputFor, unsigned int pSize);
	template<class T> bool ReadNum(T& pInputFor) {
		mIfs.read((char*)&pInputFor, sizeof(pInputFor));
		return (bool)mIfs;
	}
	void CloseRead();
	bool IsSameDataVersion() { return mDataVersion == VERSION; }
};

class CRestoreManagerWrite : public IRestoreManager {
	std::ofstream mOfs;
	unsigned char mCheckSum;

public:
	bool StartWrite();
	bool WriteStr(std::string pStr, unsigned int pSize);
	template<class T> bool WriteNum(T pValue) {
		mOfs.write((char*)&pValue, sizeof(pValue));
		if (!mOfs) return false;

		// チェックサム加算
		const char* dataPtr = (char*)&pValue;
		for (unsigned int i = 0; i < sizeof(pValue); ++i) {
			if (i > 0) ++dataPtr;
			char data = *dataPtr & 0xFF;
			mCheckSum = (mCheckSum + data) & 0xFF;
		}
		return true;
	}
	bool Flush();
};
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
	template<class T> bool ReadNum(T& pInputFor);
	void CloseRead();
	bool IsSameDataVersion() { return mDataVersion == VERSION; }
};

class CRestoreManagerWrite : public IRestoreManager {
	std::ofstream mOfs;
	unsigned char mCheckSum;

public:
	bool StartWrite();
	bool WriteStr(std::string pStr, unsigned int pSize);
	template<class T> bool WriteNum(T pValue);
	bool Flush();
};
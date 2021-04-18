#pragma once
#include <string>
#include <fstream>
#include <vector>

class IRestoreManager {
protected:
	const std::string cFileName;
	std::string GetDefaultFilePath() const;

public:
	IRestoreManager() : cFileName("/playData.sav") {}
};

class CRestoreManagerRead : public IRestoreManager {
	std::ifstream mIfs;

public:
	bool StartRead();
	bool ReadStr(std::string& pInputFor, unsigned int pSize);
	template<class T> bool ReadNum(T& pInputFor);
	void CloseRead();

};

class CRestoreManagerWrite : public IRestoreManager {
	std::ofstream mOfs;

public:
	bool StartWrite();
	bool WriteStr(std::string pStr, unsigned int pSize);
	template<class T> bool WriteNum(T pValue);
	void CloseWrite();
};
#pragma once
#include <string>
#include <fstream>
#include <vector>

class CRestoreManager {
	const std::string cFileName;
	std::ifstream mIfs;
	std::ofstream mOfs;

	std::string GetDefaultFilePath() const;

public:
	CRestoreManager() : cFileName("/playData.sav") {}

	bool StartRead();
	bool ReadStr(std::string& pInputFor, unsigned int pSize);
	template<class T> bool ReadNum(T& pInputFor);
	void CloseRead();

	bool StartWrite();
	bool WriteStr(std::string pStr, unsigned int pSize);
	template<class T> bool WriteNum(T pValue);
	void CloseWrite();
};
#pragma once
#include <vector>
#include <string>

class IErrClassBase {
public:
	virtual void WriteErrLog() = 0;
};

class ErrUndeclaredVar : public IErrClassBase {
	const std::string mInvalidVarName;
public:
	ErrUndeclaredVar(std::string varName);
	void WriteErrLog() override;
};

class ErrLessCSVDefinition : public IErrClassBase {
	const std::vector<std::string> mErrArray;
	const int requiredNum;
public:
	ErrLessCSVDefinition(std::vector<std::string> data, int required);
	void WriteErrLog() override;
};

class ErrInternalVarUndeclared : public IErrClassBase {
	const std::string mInvalidVarName;
public:
	ErrInternalVarUndeclared(std::string varName);
	void WriteErrLog() override;
};
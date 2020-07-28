#pragma once
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
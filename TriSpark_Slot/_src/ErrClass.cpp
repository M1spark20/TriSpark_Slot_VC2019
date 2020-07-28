#include "_header/ErrClass.hpp"
#include "DxLib.h"

ErrUndeclaredVar::ErrUndeclaredVar(std::string varName) : mInvalidVarName(varName) {
}

void ErrUndeclaredVar::WriteErrLog() {
	const std::string str =
		"Invalid Effect Variable name: \"" + mInvalidVarName + "\" has called.";
	DxLib::ErrorLogFmtAdd(str.c_str());
}
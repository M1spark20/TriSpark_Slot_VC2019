#pragma once
#include <limits>
#include <string>

struct SSlotEffectVariableCondition {
	std::string variableName;
	std::pair<int, std::string> lowerLimit, upperLimit;
	bool invertFlag;

	SSlotEffectVariableCondition() : lowerLimit(INT_MIN,""), upperLimit(INT_MAX,""), invertFlag(false) {};
};

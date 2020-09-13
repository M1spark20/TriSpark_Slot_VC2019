#pragma once
#include <memory>
#include <algorithm>
#include <vector>
#include "_header/ImageSourceManager.hpp"
#include "_header/ImageDestinationManager.hpp"

struct SImageEffectData {
	std::unique_ptr<IImageSourceManager> pSource;
	std::unique_ptr<IImageDestinationManager> pDest;
};

struct SSlotEffectData {
	std::vector<std::pair<int, SImageEffectData>> imgData;
};
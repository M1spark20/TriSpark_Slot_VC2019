#pragma once
#include <memory>
#include <algorithm>
#include <vector>
#include "ImageSourceManager.hpp"
#include "ImageDestinationManager.hpp"
#include "CImageColorManager.hpp"

struct SImageEffectData {
	std::unique_ptr<IImageSourceManager> pSource;
	std::unique_ptr<CImageColorManager> pColor;
	std::unique_ptr<IImageDestinationManager> pDest;
};

struct SSlotEffectData {
	std::vector<std::pair<int, SImageEffectData>> imgData;
};
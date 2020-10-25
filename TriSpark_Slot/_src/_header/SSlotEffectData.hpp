#pragma once
#include <memory>
#include <algorithm>
#include <vector>
#include "ImageSourceManager.hpp"
#include "ImageDestinationManager.hpp"
#include "CImageColorController.hpp"

struct SImageEffectData {
	std::unique_ptr<IImageSourceManager> pSource;
	std::unique_ptr<IImageDestinationManager> pDest;
};

struct SSlotEffectData {
	std::vector<std::pair<int, SImageEffectData>>	imgData;
	CImageColorController							colorController;
	std::vector<std::pair<int, std::string>>		clearScreenData;	// [order, screenName]
};
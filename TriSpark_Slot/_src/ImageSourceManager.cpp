#include "_header/ImageSourceManager.hpp"

IImageSourceManager::IImageSourceManager() {
	mPosX = -1; mPosY = -1; mPosW = -1; mPosH = -1;
	mCountX = 0; mCountY = 0;
	mDirectionFlag = false;
}
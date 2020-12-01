#pragma once

struct SDrawImageSourceData {
	int imageID;
	int x, y, w, h;
	int r, g, b;
	bool isImageFromScreen;

	SDrawImageSourceData() {
		imageID = -1;
		x = 0; y = 0; w = 0; h = 0;
		r = 0; g = 0; b = 0;
		isImageFromScreen = false;
	}
};
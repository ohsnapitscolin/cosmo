#include "platformManager.h"
#include <fstream>

PlatformManager::PlatformManager() {

}

PlatformManager::~PlatformManager() {
	free();
}

bool PlatformManager::loadMedia() {
	bool success = true;

	fstream map("resources/platforms/header.txt");

	if (!map) {
		printf("Unable to resources/platforms/header.txt!\n");
		success = false;
	}
	else {
		int platformCount;
		map >> platformCount;

		mPlatformTextures.resize(platformCount);

		for (int i = 0; i < platformCount; i++) {
			string fileName;
			map >> fileName;

			if (!mPlatformTextures[i].loadFromFile("resources/platforms/" + fileName)) {
				printf("Unable to load platform texture: %s\n", fileName.c_str());
				success = false;
				break;
			}

			if (map.fail()) {
				printf("Error reading resources/platforms/header.txt!\n");
				success = false;
				break;
			}
		}
	}

	return success;
}

void PlatformManager::free() {
	for (int i = 0; i < int(mPlatformTextures.size()); i++) {
		mPlatformTextures[i].free();
	}
}

LTexture* PlatformManager::getPlatformTexture(int index) {
	if (index >= 0 && index < int(mPlatformTextures.size())) {
		return &mPlatformTextures[index];
	}
	else {
		return NULL;
	}
}
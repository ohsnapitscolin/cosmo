#include "platformset.h"
#include <fstream>

PlatformSet::PlatformSet(std::string filename) {
	mFilename = filename;
}

PlatformSet::~PlatformSet() {
	void free();
}

bool PlatformSet::loadPlatforms() {

	//Success flag
	bool platformsLoaded = true;

	mPlatforms.resize(0);

	//Open the map
	std::ifstream map(mFilename);

	if (!map) {
		printf("No platforms available for %s\n", mFilename.c_str());
		return platformsLoaded;
	}

	int mPlatformCount;
	map >> mPlatformCount;

	mPlatforms.resize(mPlatformCount);

	//Initialize the Blocks
	for (int i = 0; i < mPlatformCount; i++)
	{
		//Determines what kind of Block will be made
		int x, y, tileType, platformType;
		int start, end, direction, speed;

		//Read Block from map file
		map >> tileType;
		map >> platformType;
		map >> x;
		map >> y;
		
		map >> start;
		map >> end;
		map >> direction;
		map >> speed;

		//If the was a problem in reading the map
		if (map.fail()) {
			//Stop loading map
			printf("Error loading map: Unexpected end of file!\n");
			platformsLoaded = false;
			break;
		}

		mPlatforms[i] = new Platform(x, y, tileType, platformType);
		mPlatforms[i]->setMotion(start, end, direction, speed);
	}

	//Close the file
	map.close();

	//If the map was loaded fine
	return platformsLoaded;
}

bool PlatformSet::findPlatformCollisions(SDL_Rect box, int direction, int& distance, int& index) {

	int topA = box.y;
	int bottomA = box.y + box.h;
	int leftA = box.x;
	int rightA = box.x + box.w;

	for (int i = 0; i < int(mPlatforms.size()); i++)
	{
		SDL_Rect platformBox = mPlatforms[i]->getBox();

		int topB = platformBox.y;
		int bottomB = platformBox.y + platformBox.h;
		int leftB = platformBox.x;
		int rightB = platformBox.x + platformBox.w;

		int currentDistance = distance;

		if (leftA >= rightB || rightA <= leftB || topA >= bottomB) {
			continue;
		}
		if (direction == DOWN) {
			if (bottomA <= topB) {
				currentDistance = topB - bottomA;
			}
		}
		if (currentDistance < distance) {
			distance = currentDistance;
			index = i;
		}
	}

	distance--;
	return index != -1;
}

int PlatformSet::getSpeed(int index, int direction) {
	if (direction != mPlatforms[index]->getDirection()) {
		return 0;
	}
	return mPlatforms[index]->getSpeed();
}


void PlatformSet::render(SDL_Rect& camera) {
	for (int i = 0; i < int(mPlatforms.size()); i++) {
		mPlatforms[i]->render(camera);
	}
}

void PlatformSet::setAlpha(int alpha) {
	/*for (int i = 0; i < int(mPlatforms.size()); i++) {
		mPlatforms[i]->setAlpha(alpha);
	}*/
}

void PlatformSet::update() {
	for (int i = 0; i < int(mPlatforms.size()); i++) {
		mPlatforms[i]->updatePosition();
	}
}

void PlatformSet::free() {
	for (int i = 0; i < int(mPlatforms.size()); i++) {
		if (mPlatforms[i] != NULL) {
			delete mPlatforms[i];
			mPlatforms[i] = NULL;
		}
	}
}
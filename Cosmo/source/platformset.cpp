#include "platformset.h"
#include <fstream>

PlatformSet::PlatformSet(std::string filename) {
	mFilename = filename;
}

PlatformSet::~PlatformSet() {
	void free();
}

bool PlatformSet::loadPlatforms() 
{
	bool platformsLoaded = true;

	mPlatforms.resize(0);

	ifstream map(mFilename);

	if (!map) {
		printf("No platforms available for %s\n", mFilename.c_str());
		return platformsLoaded;
	}

	int mPlatformCount;
	map >> mPlatformCount;

	mPlatforms.resize(mPlatformCount);

	for (int i = 0; i < mPlatformCount; i++)
	{
		string id;
		int x, y, tileType, platformType;
		int start, end, direction, speed;
		bool moving;

		map >> id;
		map >> tileType;
		map >> platformType;
		map >> x;
		map >> y;
		
		map >> start;
		map >> end;
		map >> direction;
		map >> speed;
		map >> moving;

		if (map.fail()) {
			printf("Error loading map: Unexpected end of file!\n");
			platformsLoaded = false;
			break;
		}

		mPlatforms[i] = new Platform(id, x, y, tileType, platformType);
		mPlatforms[i]->setMotion(start, end, direction, speed, moving);
	}

	map.close();
	return platformsLoaded;
}

Platform* PlatformSet::findPlatformCollisions(SDL_Rect box, int direction, int& distance) 
{
	int topA = box.y;
	int bottomA = box.y + box.h;
	int leftA = box.x;
	int rightA = box.x + box.w;

	int index = -1;

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

	if (index == -1) {
		return NULL;
	}
	else {
		return mPlatforms[index];
	}
}

vector<Platform*> PlatformSet::getPlatforms() {
	return mPlatforms;
}

void PlatformSet::render(SDL_Rect& camera) 
{
	for (int i = 0; i < int(mPlatforms.size()); i++) {
		mPlatforms[i]->render(camera);
	}
}

void PlatformSet::update() 
{
	for (int i = 0; i < int(mPlatforms.size()); i++) {
		mPlatforms[i]->updatePosition();
	}
}

void PlatformSet::free() 
{
	for (int i = 0; i < int(mPlatforms.size()); i++) {
		if (mPlatforms[i] != NULL) {
			delete mPlatforms[i];
			mPlatforms[i] = NULL;
		}
	}
}
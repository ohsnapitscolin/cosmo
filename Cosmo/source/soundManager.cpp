#include "soundManager.h"
#include <string>
#include <fstream>

SoundManager::SoundManager() 
{
};

SoundManager::~SoundManager() 
{
	void free();
}

bool SoundManager::loadMedia() 
{
	bool success = true;

	ifstream map("resources/sounds/header.txt");

	if (!map) {
		printf("Unable to load sounds!\n");
		success = false;
	}
	else {
		int totalSounds;
		map >> totalSounds;

		mSounds.resize(totalSounds);

		for (int i = 0; i < totalSounds; i++) {
			string soundName;
			map >> soundName;

			mSounds[i] = Mix_LoadWAV(("resources/sounds/" + soundName).c_str());
			if (mSounds[i] == NULL)
			{
				printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
				return false;
			}
		}

		int totalMusic;
		map >> totalMusic;

		mMusic.resize(totalMusic);

		for (int i = 0; i < totalMusic; i++) {
			string musicName;
			map >> musicName;;

			mMusic[i] = Mix_LoadMUS(("resources/sounds/" + musicName).c_str());
			if (mMusic[i] == NULL)
			{
				printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
				return false;
			}
		}
	}
	return success;
}

void SoundManager::playSound(int sound) 
{
	if (sound >= 0 && sound < int(mSounds.size())) {
		Mix_PlayChannel(-1, mSounds[sound], 0);
	}
}

void SoundManager::playMusic(int music) 
{
	if (music >= 0 && music < int(mMusic.size())) {
		if (Mix_PlayingMusic() == 0) {
			Mix_PlayMusic(mMusic[music], -1);
		}
	}
}

void SoundManager::pauseMusic() 
{
	//If there is music playing
	if (Mix_PlayingMusic() != 0) {
		//If the music is not paused
		if (Mix_PausedMusic() == 0) {
			Mix_PauseMusic();
		}
	}
}

void SoundManager::resumeMusic() 
{
	//If there is music playing
	if (Mix_PlayingMusic() != 0) {
		//If the music is paused
		if (Mix_PausedMusic() != 0) {
			Mix_ResumeMusic();
		}
	}
}

void SoundManager::free() 
{
	for (int i = 0; i < int(mSounds.size()); i++) {
		Mix_FreeChunk(mSounds[i]);
		mSounds[i] = NULL;
	}
	for (int i = 0; i < int(mMusic.size()); i++) {
		Mix_FreeMusic(mMusic[i]);
		mMusic[i] = NULL;
	}
}

#ifndef SOUNDMANAGER_H_DEFINED
#define SOUNDMANAGER_H_DEFINED

#include "common.h"
#include <stdio.h>

class SoundManager 
{
public:
	SoundManager();
	
	~SoundManager();
	void free();
	
	bool loadMedia();

	void playMusic(int music);
	void playSound(int sound);

	void pauseMusic();
	void resumeMusic();

private:
	vector<Mix_Music*> mMusic;
	vector<Mix_Chunk*> mSounds;
};

#endif
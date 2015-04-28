#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string>
#include <vector>
#include <random>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define NONE -1
#define ALPHA_MAX 255

using namespace std;

class DoorManager;
class Level;
class LoadManager;
class ObjectManager;
class PlatformManager;
class SoundManager;

// extern tells the compiler this variable is declared elsewhere
extern SDL_Renderer* gRenderer;

extern bool unlock1;
extern bool unlock2;
extern bool unlock3;

extern DoorManager doorManager;
extern LoadManager loadManager;
extern ObjectManager objectManager;
extern PlatformManager platformManager;
extern SoundManager soundManager;

extern Level* currentLevel;
extern int currentWorldIndex;

extern TTF_Font* gFont;

extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;

extern const float GRAVITY;
extern const float TERMINAL;
extern const int TILE_SIZE;

enum { LEFT, RIGHT, UP, DOWN };
enum { HORIZONTAL, VERTICAL };

#endif
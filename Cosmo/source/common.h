#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <random>

#define NONE -1
#define ALPHA_MAX 255

using namespace std;

class TileSet;
class Level;
class Slope;
class DoorManager;
class LoadManager;
class ObjectManager;
class SoundManager;

extern bool unlock1;
extern bool unlock2;
extern bool unlock3;

// extern tells the compiler this variable is declared elsewhere
extern SDL_Renderer* gRenderer;

extern DoorManager doorManager;
extern LoadManager loadManager;
extern ObjectManager objectManager;
extern SoundManager soundManager;

extern Level* currentLevel;
extern int currentWorldIndex;

extern TTF_Font *gFont;

//values
extern int LEVEL_WIDTH;
extern int LEVEL_HEIGHT;
extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;

extern const float GRAVITY;
extern const float TERMINAL;
extern const int TILE_SIZE;

enum direction { LEFT, RIGHT, UP, DOWN };
enum { WALK, JUMP, IDLE, FALL, CLIMB, LAND };

void initializeSlopes();
Slope* getCurrentSlope(int type);

#endif
#include "common.h"
#include "doorManager.h"
#include "loadManager.h"
#include "objectManager.h"
#include "platformManager.h"
#include "soundManager.h"
#include "slope.h"

SDL_Renderer* gRenderer = NULL;

DoorManager doorManager = DoorManager();
LoadManager loadManager = LoadManager();
ObjectManager objectManager = ObjectManager();
PlatformManager platformManager = PlatformManager();
SoundManager soundManager = SoundManager();

bool unlock1 = false;
bool unlock2 = false;
bool unlock3 = false;

TTF_Font* gFont;

Level* currentLevel = NULL;
int currentWorldIndex = 3;

int SCREEN_HEIGHT= -1;
int SCREEN_WIDTH = -1;

const float GRAVITY = 0.5f;
const float TERMINAL = 8.0f;
const int TILE_SIZE = 16;

#include "common.h"
#include "slope.h"
#include "loadManager.h"
#include "doorManager.h"
#include "objectManager.h"
#include "soundManager.h"
#include "platformManager.h"
#include <algorithm>

//constant values
extern SDL_Renderer* gRenderer = NULL;

extern Level* currentLevel = NULL;
extern int currentWorldIndex = 1;

extern DoorManager doorManager = DoorManager();
extern LoadManager loadManager = LoadManager();
extern ObjectManager objectManager = ObjectManager();
extern PlatformManager platformManager = PlatformManager();
extern SoundManager soundManager = SoundManager();

extern TTF_Font *gFont = NULL;

extern bool unlock1 = true;
extern bool unlock2 = true;
extern bool unlock3 = true;

extern int SCREEN_HEIGHT= -1;
extern int SCREEN_WIDTH = -1;

extern const float GRAVITY = 0.5f;
extern const float TERMINAL = 8.0f;
extern const int TILE_SIZE = 16;

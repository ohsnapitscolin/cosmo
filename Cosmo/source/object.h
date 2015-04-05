#ifndef OBJECT_H_DEFINED
#define OBJECT_H_DEFINED

#include <SDL.h>
#include "common.h"
#include "texture.h"

class Object {
public:
	
	Object();
	Object(string id, int type, LTexture* Texture, int x, int y);

	bool interact();

	void setInteraction(string interaction);
	string getInteraction();
	string getId();

	bool overlap(SDL_Rect mBox);
	void render(int x, int y);
	void renderSprite(int x, int y);
	int getType();
	void setVisible(bool visible);
	SDL_Rect getBox();

private:
	int mFrames;
	LTexture* mTexture;
	int mType;
	SDL_Rect mBox;

	string mId;

	string mInteraction;

	bool mVisible;
};
#endif
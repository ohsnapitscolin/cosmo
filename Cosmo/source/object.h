#ifndef OBJECT_H_DEFINED
#define OBJECT_H_DEFINED

#include "common.h"
#include "texture.h"

class Object 
{
public:
	Object(string id, int type, Texture* Texture, int x, int y);

	bool interact();
	bool overlap(SDL_Rect box);

	void addInteraction(string interaction);
	vector<string> getInteractions();
	
	string getId();

	void render(int x, int y);
	void renderSprite(int x, int y);
	
	void setVisible(bool visible);
	void toggleVisible();

	int getType();

	void setFixed(bool fixed);
	bool isFixed();

	SDL_Rect getBox();

private:
	int mFrames;
	int mType;

	Texture* mTexture;

	SDL_Rect mBox;

	string mId;
	vector<string> mInteractions;

	bool mVisible;
	bool mFixed;
};

#endif
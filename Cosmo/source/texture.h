#ifndef TEXTURE_H_DEFINED
#define TEXTURE_H_DEFINED

#include "common.h"

class Texture
{
public:
	Texture();

	~Texture();
	void free();

	bool loadFromFile(string path);
	bool loadFromRenderedText(TTF_Font* font, string textureText, SDL_Color textColor);

	bool clipSprite(int width, int height);

	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);

	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	void renderSprite(int x, int y, int currentSprite, bool flip);

	void setScale(int scale);
	void setOffset(int x, int y);
	
	int getWidth();
	int getHeight();
	
	int getSpriteCount();
	int getSpriteWidth();
	int getSpriteHeight();

private:
	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;

	int mScale;

	int mOffsetX;
	int mOffsetY;

	vector<SDL_Rect> mSpriteSheet;

	int mSpriteCount;
	int mSpriteWidth;
	int mSpriteHeight;
};

#endif
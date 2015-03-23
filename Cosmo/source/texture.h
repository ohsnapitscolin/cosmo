#ifndef TEXTURE_H_DEFINED
#define TEXTURE_H_DEFINED

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <vector>

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	void clipSprite(int width, int height); 
	void renderSprite(int x, int y, int currentSprite, bool flip);

	void setOffset(int x, int y);
	void setScale(int scale);

	int getSpriteCount();

	//Gets image dimensions
	int getWidth();
	int getHeight();

	int getSpriteWidth();
	int getSpriteHeight();


private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	std::vector<SDL_Rect> mSpriteSheet;

	//Image dimensions
	int mSpriteCount;

	int mSpriteWidth;
	int mSpriteHeight;

	int mWidth;
	int mHeight;

	int mOffsetX;
	int mOffsetY;

	int mScale;
};

#endif
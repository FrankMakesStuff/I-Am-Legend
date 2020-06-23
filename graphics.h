// GRAPHICS header file

#pragma once

#include "main.h"

struct colorStruct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);
void renderTexture( SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h );
void renderTexture( SDL_Texture *tex, SDL_Renderer *ren, int x, int y );
void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);
void renderColorize( SDL_Renderer* ren );
void doDaylightCycle(SDL_Renderer* ren, unsigned int &currentTime, unsigned int cycleDuration );
void drawClock( SDL_Renderer *ren, float angle );
float toRadians( float degrees );

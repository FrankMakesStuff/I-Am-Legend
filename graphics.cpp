#include "graphics.h"

#define CLOCK_ALPHA 180
#define CLOCK_OFFSET_X 30
#define CLOCK_OFFSET_Y 30
#define CLOCK_RADIUS 15

// Graphics globals
colorStruct daylightColor;

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
	std::string fullPath = SDL_GetBasePath() + file;
	SDL_Texture *texture = IMG_LoadTexture(ren, fullPath.c_str());
	if (texture == nullptr){
		printf( "Failed to load image: %s\n", file.c_str() ); 
	}
	
	printf( "Image loaded: %s\n", file.c_str() );
	return texture;
}


void renderTexture( SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h ){
	SDL_Rect dst, src;
	dst.w = src.w = w;
	dst.h = src.h = h;
	dst.x = x; src.x = 0;
	dst.y = y; src.y = 0;
	
	SDL_RenderCopy( ren, tex, &src, &dst );
	
	return;
}


void renderTexture( SDL_Texture *tex, SDL_Renderer *ren, int x, int y ){
	int w, h;
	SDL_QueryTexture( tex, NULL, NULL, &w, &h );
	renderTexture( tex, ren, x, y, w, h );	
	
	return;
}

void renderColorize( SDL_Renderer* ren ) {
	//SDL_Rect r = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // use rect if only partially covering the window

	SDL_SetRenderDrawColor( ren, daylightColor.r, daylightColor.g, daylightColor.b, daylightColor.a );
	SDL_SetRenderDrawBlendMode( ren, SDL_BLENDMODE_MOD ); // Blend using multiply
	SDL_RenderFillRect( ren, NULL );
	SDL_SetRenderDrawBlendMode( ren, SDL_BLENDMODE_BLEND ); // Blend using Alpha
	
	return;
}

void doDaylightCycle( SDL_Renderer* ren, unsigned int &currentTime, unsigned int cycleDuration ){
	// wrap around to zero when value goes over limit
	if( currentTime > cycleDuration )
		currentTime = 0;

	float fTime = (float)currentTime;
	float halfCycle = cycleDuration / 2;
	
	float distance = fabs( halfCycle - fTime ) / halfCycle;
	unsigned char dimness = (unsigned char)(255.f * distance);
	
	daylightColor = { dimness, dimness, 255, 255 };
}

void drawClock( SDL_Renderer *ren, float angle ){
	// Draw a clock
	int centerX = WINDOW_WIDTH - CLOCK_OFFSET_X, centerY = WINDOW_HEIGHT - CLOCK_OFFSET_Y;
	SDL_SetRenderDrawColor( ren, 200, 200, 200, CLOCK_ALPHA ); // clock face color
	DrawCircle( ren, centerX, centerY, CLOCK_RADIUS );
	
	float clockHandX, clockHandY;
	clockHandX = cos( toRadians( angle )) * (CLOCK_RADIUS * 0.7f);
	clockHandY = sin( toRadians( angle )) * (CLOCK_RADIUS * 0.7f);
	
	SDL_SetRenderDrawColor( ren, 255, 255, 255, CLOCK_ALPHA );
	SDL_RenderDrawLine( ren, centerX, centerY, centerX + clockHandX, centerY + clockHandY );
}

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

float toRadians( float degrees ){
	float result = ( degrees / 180.0f ) * 3.141592654f;
	return result;
}




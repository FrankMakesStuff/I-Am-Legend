// MAIN header file

#pragma once

#define SDL_MAIN_HANDLED

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 640
#define WINDOW_LEFT 560
#define WINDOW_TOP 64
#define WINDOW_TITLE "I Am Legend"

#define MAP_DEFAULT_WIDTH 200
#define MAP_DEFAULT_HEIGHT 200


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include "SDL2\SDL.h"
#include "SDL2\SDL_image.h"
#include "SDL2\SDL_mixer.h"

// custom
#include "vividConsole.h"
#include "graphics.h"
#include "objects.h"
#include "input.h"
#include "sound.h"
#include "text.h"

enum class GAMESTATE { GameMode, MenuMode };

int init();
void cleanup();
void doPlayerAction();
void doRender();
void generateNewWorld(unsigned int mapWidth, unsigned int mapHeight);




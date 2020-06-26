#pragma once

#include "main.h"

enum class PLAYERACTION {
	None = 0,
	MoveUp = 1,
	MoveDown = 2,
	MoveLeft = 4,
	MoveRight = 8,
	Inspect = 16,
	Attack = 32,
	ZoomIn = 64,
	ZoomOut = 128,
	ShowPlayerspace = 256,
	HidePlayerspace = 512,
	ToggleNightcolor = 1024,
	CycleInventory = 2048,
	KeyHeld = 4096,
	RequestExit = 8192
	
};

inline PLAYERACTION operator|( PLAYERACTION a, PLAYERACTION b ){
	return static_cast<PLAYERACTION>( static_cast<int>(a) | static_cast<int>(b) );
}
inline PLAYERACTION operator&( PLAYERACTION a, PLAYERACTION b ){
	return static_cast<PLAYERACTION>( static_cast<int>(a) & static_cast<int>(b) );
}

PLAYERACTION getPlayerAction();
bool isAction( PLAYERACTION flags, PLAYERACTION action );

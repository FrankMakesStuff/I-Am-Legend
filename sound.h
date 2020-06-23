// sound header file

#pragma once

#define SOUNDFX_DOOR "doorclose.wav"
#define SOUNDFX_ATTACK "attack.wav"
#define SOUNDFX_DESTROY "destroy.wav"
#define SOUNDFX_PICKUP_SMALL "pickup_small.wav"
#define SOUNDFX_PICKUP_BIG "pickup_big.wav"

#include "main.h"

enum class SOUNDS {
	Door,
	Attack,
	Destroy,
	PickupSmall,
	PickupBig,
	Spooky,
	Footstep
};

int initSound();
void playSound( SOUNDS sound );
void destroySound();

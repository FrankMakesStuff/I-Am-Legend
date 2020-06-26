// sound header file

#pragma once

#define SOUNDFX_DOOR "doorclose.wav"
#define SOUNDFX_ATTACK "attack.wav"
#define SOUNDFX_BIGATTACK "attack_large.wav"
#define SOUNDFX_DESTROY "destroy.wav"
#define SOUNDFX_PICKUP_SMALL "pickup_small.wav"
#define SOUNDFX_PICKUP_BIG "pickup_big.wav"
#define SOUNDFX_REPAIR "repair.wav"
#define SOUNDFX_DIG "dig.wav"
#define SOUNDFX_HARVEST "harvest.wav"
#define SOUNDFX_THROW "throw.wav"
#define SOUNDFX_SQUISH "squish.wav"
#define SOUNDFX_UNDEAD_KILL "undead_kill.wav"
#define SOUNDFX_FOOTSTEP "footstep.wav"
#define SOUNDFX_PLAYERDEATH "playerdeath.wav"

#define MUSIC_DAY1 "mus_day.ogg"
#define MUSIC_NIGHT1 "mus_night.ogg"

#include "main.h"

enum class SOUNDS {
	Door,
	Attack,
	BigAttack,
	Destroy,
	PickupSmall,
	PickupBig,
	Repair,
	Dig,
	Harvest,
	Throw,
	Squish,
	UndeadKill,
	Footstep,
	Playerdeath
};

enum class MUSIC {
	Day1,
	Day2,
	Day3,
	Day4,
	Day5,
	Night1,
	Night2,
	Night3,
	Night4,
	Night5
};

int initSound();
void playSound( SOUNDS sound );
void playMusic( MUSIC music );
void destroySound();

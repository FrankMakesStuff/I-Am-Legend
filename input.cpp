#include "input.h"

bool isSpacebarHeld = false;
bool requestToQuit = false;

void requestQuit() {
	requestToQuit = true;
}

bool getQuit(){
	return requestToQuit;
}

PLAYERACTION getPlayerAction(){
	/* NOTE: Examine funcion SDL_PumpEvents() to force event loop to update if keyboard state is acting weird.
	Note that this function is called internally whenever SDL_PollEvents is called, which it is back in main() */
	const Uint8* state = SDL_GetKeyboardState( NULL );
	
	PLAYERACTION flags = PLAYERACTION::None;

	if( state[SDL_SCANCODE_LEFT] )
		flags = flags | PLAYERACTION::MoveLeft;

	if( state[SDL_SCANCODE_RIGHT] )
		flags = flags | PLAYERACTION::MoveRight;

	if( state[SDL_SCANCODE_UP] )
		flags = flags | PLAYERACTION::MoveUp;

	if( state[SDL_SCANCODE_DOWN] )
		flags = flags | PLAYERACTION::MoveDown;
		
	if( state[SDL_SCANCODE_TAB] )
		flags = flags | PLAYERACTION::CycleInventory;

	if( state[SDL_SCANCODE_EQUALS] )
		flags = flags | PLAYERACTION::ZoomIn;

	if( state[SDL_SCANCODE_MINUS] )
		flags = flags | PLAYERACTION::ZoomOut;
		
	if( state[SDL_SCANCODE_1] )
		flags = flags | PLAYERACTION::ShowPlayerspace;
		
	if( state[SDL_SCANCODE_2] )
		flags = flags | PLAYERACTION::HidePlayerspace;
		
	if( state[SDL_SCANCODE_3] )
		flags = flags | PLAYERACTION::ToggleNightcolor;
		
	if( state[SDL_SCANCODE_ESCAPE] )
		flags = PLAYERACTION::RequestExit;
	
	// Inspect
	if( state[SDL_SCANCODE_SPACE] ){
		if( isSpacebarHeld == false ){
			isSpacebarHeld = true;
			flags = flags | PLAYERACTION::Inspect;
		}
		else {
			// Spacebar is being held, return no action to avoid looping
			flags = PLAYERACTION::KeyHeld;
		}
	}
	
	// Attack
	if( state[SDL_SCANCODE_LCTRL] ){
		flags = flags | PLAYERACTION::Attack;
	}
	
	// if we have flags, return them here
	if( flags != PLAYERACTION::None )
		return flags;
	
		
	// If we're here, no keys are pressed.
	isSpacebarHeld = false;
	return PLAYERACTION::None;
}

bool isAction( PLAYERACTION flags, PLAYERACTION action ){
	if((flags & action) == action ) return true;
	
	return false;
}











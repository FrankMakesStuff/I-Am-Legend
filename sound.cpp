// sound class file

#include "sound.h"

Mix_Chunk* sfx_Door;
Mix_Chunk* sfx_Attack;
Mix_Chunk* sfx_Destroy;
Mix_Chunk* sfx_PickupSmall;


int initSound(){
	vividConsole log;
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
		log.printError( "Sound failed to initialize." );
		printf( "Sound error: %s\n", Mix_GetError() );
		return 0;
	}
	
	sfx_Door = Mix_LoadWAV( SOUNDFX_DOOR );
	if( sfx_Door == nullptr ){
		log.printError( "Couldn't load sfx: door" );
  		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	sfx_Attack = Mix_LoadWAV( SOUNDFX_ATTACK );
	if( sfx_Attack == nullptr ){
		log.printError( "Couldn't load sfx: attack" );
  		printf( "%s\n", Mix_GetError() );
  		return 0;
	}

	sfx_Destroy = Mix_LoadWAV( SOUNDFX_DESTROY );
	if( sfx_Destroy == nullptr ){
		log.printError( "Couldn't load sfx: destroy" );
  		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_PickupSmall = Mix_LoadWAV( SOUNDFX_PICKUP_SMALL );
	if( sfx_PickupSmall == nullptr ){
		log.printError( "Couldn't load sfx: pickup-small" );
  		printf( "%s\n", Mix_GetError() );
  		return 0;
	}

	
	return 1;
}
void playSound( SOUNDS sound ){
	switch( sound ){
		case SOUNDS::Door:
			Mix_PlayChannel( -1, sfx_Door, 0 );
			break;
			
		case SOUNDS::Attack:
			Mix_PlayChannel( -1, sfx_Attack, 0 );
			break;
			
		case SOUNDS::Destroy:
			Mix_PlayChannel( -1, sfx_Destroy, 0 );
			break;
			
		case SOUNDS::PickupSmall:
			Mix_PlayChannel( -1, sfx_PickupSmall, 0 );
			break;
		
		default:
			break;
	}
	
	return;
}
void destroySound(){
	
	// release all sound
	Mix_FreeChunk( sfx_Door );
	Mix_FreeChunk( sfx_Attack );
	Mix_FreeChunk( sfx_Destroy );
	Mix_FreeChunk( sfx_PickupSmall );
	
	Mix_Quit();
	
	return;
}





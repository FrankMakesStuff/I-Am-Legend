// sound class file

#include "sound.h"

Mix_Chunk* sfx_Door;
Mix_Chunk* sfx_Attack;
Mix_Chunk* sfx_BigAttack;
Mix_Chunk* sfx_Destroy;
Mix_Chunk* sfx_PickupSmall;
Mix_Chunk* sfx_PickupBig;
Mix_Chunk* sfx_Repair;
Mix_Chunk* sfx_Dig;
Mix_Chunk* sfx_Harvest;
Mix_Chunk* sfx_Throw;
Mix_Chunk* sfx_Squish;
Mix_Chunk* sfx_UndeadKill;
Mix_Chunk* sfx_Footstep;
Mix_Chunk* sfx_Playerdeath;

Mix_Music* music_Day1;
Mix_Music* music_Night1;


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

	sfx_BigAttack = Mix_LoadWAV( SOUNDFX_BIGATTACK );
	if( sfx_BigAttack == nullptr ){
		log.printError( "Couldn't load sfx: Big attack" );
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
	
	sfx_PickupBig = Mix_LoadWAV( SOUNDFX_PICKUP_BIG );
	if( sfx_PickupBig == nullptr ){
		log.printError( "Couldn't load sfx: pickup-big" );
  		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_Repair = Mix_LoadWAV( SOUNDFX_REPAIR );
	if( sfx_Repair == nullptr ){
		log.printError( "Couldn't load sfx: repair" );
  		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_Dig = Mix_LoadWAV( SOUNDFX_DIG );
	if( sfx_Dig == nullptr ){
		log.printError( "Couldn't load sfx: dig " );
		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_Harvest = Mix_LoadWAV( SOUNDFX_HARVEST );
	if( sfx_Harvest == nullptr ){
		log.printError( "Couldn't load sfx: Harvest" );
		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_Throw = Mix_LoadWAV( SOUNDFX_THROW );
	if( sfx_Throw == nullptr ){
		log.printError( "Couldn't load sfx: throw" );
		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_Squish = Mix_LoadWAV( SOUNDFX_SQUISH );
	if( sfx_Squish == nullptr ){
		log.printError( "Couldn't load sfx: squish" );
		printf( "%s\n", Mix_GetError() );
  		return 0;
	}

	sfx_UndeadKill = Mix_LoadWAV( SOUNDFX_UNDEAD_KILL );
	if( sfx_UndeadKill == nullptr ){
		log.printError( "Couldn't load sfx: undead_kill" );
		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_Footstep = Mix_LoadWAV( SOUNDFX_FOOTSTEP );
	if( sfx_Footstep == nullptr ){
		log.printError( "Couldn't load sfx: footstep" );
		printf( "%s\n", Mix_GetError() );
  		return 0;
	}
	
	sfx_Playerdeath = Mix_LoadWAV( SOUNDFX_PLAYERDEATH );
	if( sfx_Playerdeath == nullptr ){
		log.printError( "Couldn't load sfx: playerdeath" );
		printf( "%s\n", Mix_GetError() );
		return 0;
	}
	
	////////////////////////////////
	//   Music
	////////////////////////////////
	
	music_Day1 = Mix_LoadMUS( MUSIC_DAY1 );
	if( !music_Day1 ){
    	log.printError( "Couldn't load music (Day1)" );
    	printf( "%s\n", Mix_GetError() );
    	return 0;
	}
	
	music_Night1 = Mix_LoadMUS( MUSIC_NIGHT1 );
	if( !music_Night1 ){
		log.printError( "Couldn't load music (Night1)" );
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
			
		case SOUNDS::BigAttack:
			Mix_PlayChannel( -1, sfx_BigAttack, 0 );
			break;
			
		case SOUNDS::Destroy:
			Mix_PlayChannel( -1, sfx_Destroy, 0 );
			break;
			
		case SOUNDS::PickupSmall:
			Mix_PlayChannel( -1, sfx_PickupSmall, 0 );
			break;
			
		case SOUNDS::PickupBig:
			Mix_PlayChannel( -1, sfx_PickupBig, 0 );
			break;
			
		case SOUNDS::Repair:
			Mix_PlayChannel( -1, sfx_Repair, 0 );
			break;
			
		case SOUNDS::Dig:
			Mix_PlayChannel( -1, sfx_Dig, 0 );
			break;
			
		case SOUNDS::Harvest:
			Mix_PlayChannel( -1, sfx_Harvest, 0 );
			break;
			
		case SOUNDS::Throw:
			Mix_PlayChannel( -1, sfx_Throw, 0 );
			break;
			
		case SOUNDS::Squish:
			Mix_PlayChannel( -1, sfx_Squish, 0 );
			break;
			
		case SOUNDS::UndeadKill:
			Mix_PlayChannel( -1, sfx_UndeadKill, 0 );
			break;
			
		case SOUNDS::Footstep:
			Mix_PlayChannel( -1, sfx_Footstep, 0 );
			break;
			
		case SOUNDS::Playerdeath:
			Mix_PlayChannel( -1, sfx_Playerdeath, 0 );
			break;
		
		
		default:
			break;
	}
	
	return;
}

void playMusic( MUSIC music ){
	switch( music ){
		case MUSIC::Day1:
			Mix_PlayMusic( music_Day1, 1 );
			break;
			
		case MUSIC::Night1:
			Mix_PlayMusic( music_Night1, 1 );
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
	Mix_FreeChunk( sfx_BigAttack );
	Mix_FreeChunk( sfx_Destroy );
	Mix_FreeChunk( sfx_PickupSmall );
	Mix_FreeChunk( sfx_Repair );
	Mix_FreeChunk( sfx_Dig );
	Mix_FreeChunk( sfx_Harvest );
	Mix_FreeChunk( sfx_Throw );
	Mix_FreeChunk( sfx_Squish );
	Mix_FreeChunk( sfx_UndeadKill );
	Mix_FreeChunk( sfx_Footstep );
	Mix_FreeChunk( sfx_Playerdeath );
	
	Mix_FreeMusic( music_Day1 );
	Mix_FreeMusic( music_Night1 );
	
	Mix_Quit();
	
	return;
}





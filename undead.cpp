// Undead source file - extra functions to handle "Undead" behavior
#include "objects.h"

MapSet* map_ref = nullptr;
bool isNightTime = true;
bool isDawn = false;

int spawnNewUndead( MapSet* map, int num ){
	int spawnX, spawnY;
	MapTile* t;
	
	SDL_Rect playerRect = { map->getPlayerGuy()->x - 30, map->getPlayerGuy()->y - 30, 61, 61 };
	
	// get a reference pointer to the map
	if( map_ref == nullptr )
		map_ref = map;
	
	for( int i = 0; i < num; ++i ){
		spawnX = rand() % map->getWidth();
		spawnY = rand() % map->getHeight();
		
		// try again if Undead is spawned too close to player
		if( inRect( playerRect, spawnX, spawnY )){
			--i;
			continue;
		}
		
		t = map->getTileAt( spawnX, spawnY );
		if( t == nullptr || !t->isPlayerspace ){	// throw away if tile is invalid or non-playerspace
			// try again
			--i;
			continue;
		}
		else {
			// create new Undead entity
			Entity newUndead( ENTITY_TYPES::Undead, t );
			map->addEntity( newUndead ); 
			
		}
	}
	
	printf( "The dead are rising...\n" );
	return 1;
}

void updateUndead( Entity* undead ){
	
	unsigned int whichWay = rand() % 5; // dice-roll for how to move

	// decide what 'state' the Undead is in by checking it's immediate area
	undeadCheckRect( undead );
	
	// determine what undead wants to do
	switch( undead->flags ){
		
		// Wander - Undead moves aimlessly one tile at a time
		case UNDEAD_FLAGS_WANDER:
			
			moveUndead( undead, whichWay );
			break;
			
		// Persue - Undead will move toward PlayerGuy
		case UNDEAD_FLAGS_PERSUE:
			persuePlayer( undead, whichWay );
			break;
			
		// Dormant - Undead is in a Dormant state during daylight hours -- (do nothing)
		case UNDEAD_FLAGS_DORMANT:
			if( isNightTime ){
				// release Undead from being dormant upon nightfall
				undead->flags = UNDEAD_FLAGS_WANDER;
			}
			
			break;
			
		// Flee - Undead moves away from PlayerGuy, as a result of dawn approaching soon
		case UNDEAD_FLAGS_FLEE:
			undeadFlee( undead, whichWay );
			break;
			
		default:
			break;
	}
	
	return;
}




int undeadCheckRect( Entity* undead ){
	int result = UNDEAD_FLAGS_WANDER;
	
	// Create a Rect area that represents the Undead's "awareness" area
	SDL_Rect undeadArea = {
			undead->x - 20,
			undead->y - 20,
			41, 
			41 };
			
	// Persue player if close enough
	if( inRect( undeadArea, map_ref->getPlayerGuy()->x, map_ref->getPlayerGuy()->y ) && (map_ref->getPlayerGuy()->health > 0) ){
		
		// does player have fresh garlic in their pocket?
		Entity* checkGarlic = map_ref->getPlayerGuy()->checkInventory( ENTITY_TYPES::Garlic );
		if( checkGarlic != nullptr ){
			result = UNDEAD_FLAGS_FLEE;
		} else {
			result = UNDEAD_FLAGS_PERSUE;	
		}
	}
		
	// Change state to flee if any garlic is in rect
	for( int i = 0; i < map_ref->entities.size(); ++i ){
		Entity* e = &map_ref->entities[i];
		if( e->getType() == ENTITY_TYPES::Garlic ){
			if( inRect( undeadArea, e->x, e->y ) ){
				result = UNDEAD_FLAGS_FLEE;
				undead->flagsParam2 = e->x;
				undead->flagsParam3 = e->y;
			}
		}
	}
	
	// Lastly, make Undead dormant if it's daytime
	if( !isNightTime ){
		result = UNDEAD_FLAGS_DORMANT;
		undead->tileset_x = 272;
		undead->tileset_y = 0;
	}
	else {
		undead->tileset_x = 208;
		undead->tileset_y = 0;
	}
	
	// Here, we should have one of three results: Wander, Persue, or Flee
	// set the flag to the Undead pointer, and also return the flag value
	undead->flags = result;
	
	return result;
}




void moveUndead( Entity* undead, int directionFlags ){
	MapTile* t = nullptr;
	bool validMove = false;
	
	// NOTE: About movement, all logic mostly exists in moveEntity() function, so add more
	// there if need be. Here, just send it what we want, it'll figure out if movement is legal or not.
	// also remember, moveEntity() returns false if movement is illegal, so work that in here if needed.
	switch( directionFlags ){
		case UNDEAD_WALK_LEFT:
			t = map_ref->getTileAt( undead->x - 1, undead->y );
			if( t != nullptr )
				validMove = moveEntity( map_ref, undead, t->x, t->y );
			break;
			
		case UNDEAD_WALK_RIGHT:
			t = map_ref->getTileAt( undead->x + 1, undead->y );
			if( t != nullptr )
				validMove = moveEntity( map_ref, undead, t->x, t->y );
			break;	
		
		case UNDEAD_WALK_UP:
			t = map_ref->getTileAt( undead->x, undead->y - 1 );
			if( t != nullptr )
				validMove = moveEntity( map_ref, undead, t->x, t->y );
			break;
			
		case UNDEAD_WALK_DOWN:
			t = map_ref->getTileAt( undead->x, undead->y + 1 );
			if( t != nullptr )	
				validMove = moveEntity( map_ref, undead, t->x, t->y );
			break;
			
		case UNDEAD_WALK_DWELL:
			// do nothing... Undead is "taking a break" :)
			break;
		
		default:
			break;
	}
	
	// if the undead is bumping into trees and stuff, make them move randomly
	// for a set number of moves. This happens when Undead are persuing the player
	// and need to get unstuck from trees
	if( !validMove && (t!=nullptr)){
		
		// is it a wall? try and attack it!
		if( t->baseType == BASE_TYPES::Wall ){
			attackTile( map_ref, undead, t );
		}
		
		// Attack the player!!
		if( t->hasEntity() ){
			
			Entity* tempEnt = (Entity*)t->getEntity();
			
			if( tempEnt->getType() == ENTITY_TYPES::PlayerGuy )
				attackTile( map_ref, undead, t );
				
		}
		
		undead->flagsParam1 = 5;
	}
}




void undeadFlee( Entity* undead, int directionFlags ){
	int distanceX = abs( undead->x - undead->flagsParam2 );
	int distanceY = abs( undead->y - undead->flagsParam3 );
	
	int whichWay = UNDEAD_WALK_DWELL;
	
	// Move away from whichever distance is shorter
	if( distanceX < distanceY ){ // move horizontally
		if( undead->flagsParam2 < undead->x )
			whichWay = UNDEAD_WALK_RIGHT;
		else
			whichWay = UNDEAD_WALK_LEFT;
	}
	else { // move vertically
		if( undead->flagsParam3 < undead->y )
			whichWay = UNDEAD_WALK_DOWN;
		else
			whichWay = UNDEAD_WALK_UP;
	}
	
	// flagsParam1 has value greater than 0 whenever Undead bumps into a blockable tile
	if( undead->flagsParam1 > 0 ){
		whichWay = directionFlags;
		undead->flagsParam1--;
		if( undead->flagsParam1 < 0 )
			undead->flagsParam1 = 0;
	}
	
	moveUndead( undead, whichWay );
}




void persuePlayer( Entity* undead, int directionFlags ){
	Entity* p = map_ref->getPlayerGuy();
	
	int distanceX = abs( undead->x - p->x );
	int distanceY = abs( undead->y - p->y );
	
	int whichWay = UNDEAD_WALK_DWELL;
	
	// reduce whichever distance is greater
	if( distanceX > distanceY ){ // move horizontally
		if( p->x < undead->x )
			whichWay = UNDEAD_WALK_LEFT;
		else
			whichWay = UNDEAD_WALK_RIGHT;
	}
	else { // move vertically
		if( p->y < undead->y )
			whichWay = UNDEAD_WALK_UP;
		else
			whichWay = UNDEAD_WALK_DOWN;
	}
	
	// flagsParam1 has value greater than 0 whenever Undead bumps into a blockable tile
	if( undead->flagsParam1 > 0 ){
		whichWay = directionFlags;
		undead->flagsParam1--;
		if( undead->flagsParam1 < 0 )
			undead->flagsParam1 = 0;
	}
	
	moveUndead( undead, whichWay );
}




void setUndeadTime( unsigned int currentTime, unsigned int maxTime ){
	// considering a value of maxTime == midnight
	
	int quarterTime = maxTime / 4;
	int eighthTime = quarterTime / 2;
	
	if( (currentTime <= quarterTime) || (maxTime-currentTime <= quarterTime) ){
		
		// Night becomes day
		if( isNightTime == true ){
			plantGarlic( map_ref, 3 );
			dropApples( map_ref, 3 );
			playMusic( MUSIC::Day1 );
			
		}
		isNightTime = false;	
	}
	else {
		
		// Day becomes night
		if( isNightTime == false ){
			spawnNewUndead( map_ref, 20 );
			addQueueMessage( "The dead are rising..." );
			playMusic( MUSIC::Night1 );
			
		}
		isNightTime = true;	
	}
		
	if( (currentTime < quarterTime) && (quarterTime - currentTime < eighthTime) )
		isDawn = true;
	else
		isDawn = false; 
}










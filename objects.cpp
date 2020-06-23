#include "objects.h"

// Global variables

const std::string LockDescriptors[] = {	"rusty",
										"brass",
										"silver",
										"golden",
										"ancient" };

// As buildings are constructed (with the call to MapSet::putBuilding() )
// a variable is incremented by one, and whatever that value becomes during
// construction of a new building will also be the unique ID of a
// lock-and-key pair. The lock is the LockableBarrier placed atop the
// door tile of the building. It is then up to the system of where to place
// the key.
unsigned int buildingIterator = 0;




////////////////////////////////////
// BaseObject
////////////////////////////////////
std::string BaseObject::locationToString(){
	std::string sval = "(" + std::to_string( this->x ) + ", " + std::to_string( this->y ) + ")";
	return sval;
}

////////////////////////////////////
// MapTile
////////////////////////////////////
MapTile::MapTile(){
	this->x = 0;
	this->y = 0;
	this->baseType = BASE_TYPES::Void;
	this->tileset_x = 0;
	this->tileset_y = 0;
	this->entitySittingOnTile = nullptr;
	this->holdingEntity = false;
	this->isPlayerspace = false;
}

MapTile::MapTile( int x, int y, int tileset_x, int tileset_y, BASE_TYPES type ){
	this->x = x;
	this->y = y;
	this->tileset_x = tileset_x;
	this->tileset_y = tileset_y;
	this->baseType = type;
	this->holdingEntity = false;
	this->entitySittingOnTile = nullptr;
	this->isPlayerspace = false;
}

void MapTile::setToGrass(){
	this->tileset_y = 0;
	this->tileset_x = ( rand() % NUM_GRASS_TILES ) * TILEWIDTH;
	this->baseType = BASE_TYPES::Grass;
	
	return;
}

void MapTile::setType( BASE_TYPES type ){
	switch( type ){
		
		case BASE_TYPES::Grass:
			setToGrass();
			break;
			
		case BASE_TYPES::Wall:
			this->baseType = type;
			this->tileset_y = 0;
			this->tileset_x = 112;
			this->health = 150;
			break;
			
		case BASE_TYPES::WoodPlanks:
			this->baseType = type;
			this->tileset_y = 0;
			this->tileset_x = 176;
			break;
		
		case BASE_TYPES::Tree:
			this->baseType = type;
			this->tileset_x = 224;
			this->tileset_y = 0;
			break;
			
		case BASE_TYPES::Door:
			this->baseType = type;
			this->tileset_x = 192;
			this->tileset_y = 0;
			break;
			
		case BASE_TYPES::GrassHighlight:
			this->baseType = type;
			this->tileset_x = 0;
			this->tileset_y = 16;
			break;
			
		case BASE_TYPES::Rubble:
			this->baseType = type;
			this->tileset_x = 160;
			this->tileset_y = 0;
			break;
		
		default:
			break;
	}
}

bool MapTile::hasEntity(){
	return this->holdingEntity;
}

// note: this overwrites!
bool MapTile::addEntity( BaseObject* e ){
	this->entitySittingOnTile = e;
	this->holdingEntity = true;
	
	return true;
}

bool MapTile::removeEntity(){
	if( this->holdingEntity = false )
		return false;

	this->entitySittingOnTile = nullptr;
	this->holdingEntity = false;
	
	return true;
}

BaseObject* MapTile::getEntity(){
	if( this->holdingEntity ){
		return this->entitySittingOnTile;
	}
	
	return nullptr;
}

std::string MapTile::toString(){
	std::string typeString;

	switch( this->baseType ){
		case BASE_TYPES::Grass:
			typeString = "grass";
			break;
			
		case BASE_TYPES::WoodPlanks:
			typeString = "a hardwood floor";
			break;
			
		case BASE_TYPES::Wall:
			typeString = "a wall";
			break;

		case BASE_TYPES::Tree:
			typeString = "a tree";
			break;
			
		case BASE_TYPES::Door:
			typeString = "a door";
			break;

		default:
			typeString = "[undefined BASE_TYPE" + std::to_string((int)this->baseType) + "]";
			break;
	}
	
	return typeString;
}

////////////////////////////////////
// Entity
////////////////////////////////////
Entity::Entity( int x, int y, ENTITY_TYPES type, MapTile* currentTile ){
	this->x = x;
	this->y = y;
	this->currentTile = currentTile;
	this->baseType = BASE_TYPES::Entity;
	this->facing = DIRECTION::East;
	this->index = -1; // non-assigned entity, until it is put into the map
	
	switch( type ){
		case ENTITY_TYPES::PlayerGuy:
			this->health = 100;
			this->strength = 10;	// bare fist strength - unequipped
			this->entityType = type;
			this->tileset_x = 96;
			this->tileset_y = 0;
			this->drawable = true;
			break;
			
		case ENTITY_TYPES::TreeBuilder:
			this->health = 50;
			this->strength = 5000;
			this->entityType = type;
			this->tileset_x = 224;
			this->tileset_y = 0;
			this->drawable = false;
			break;
			
		case ENTITY_TYPES::Key:
			this->health = 999999;
			this->strength = 5;
			this->entityType = type;
			this->tileset_x = 240;
			this->tileset_y = 0;
			this->drawable = true;
			this->flags = ENTITY_FLAGS_OBTAINABLE;
			break;
			
		case ENTITY_TYPES::LockableBarrier:
			this->health = 999999;
			this->strength = 9000;
			this->entityType = type;
			this->tileset_x = -1;
			this->tileset_y = -1;
			this->drawable = false;
			this->flags = ENTITY_FLAGS_LOCKED;
			break;
		
		// Undead
		case ENTITY_TYPES::Undead:
			this->health = 50;
			this->strength = 10;
			this->entityType = type;
			this->tileset_x = 208;
			this->tileset_y = 0;
			this->drawable = true;
			this->flags = UNDEAD_FLAGS_WANDER;
			this->flagsParam1 = 0;
			this->flagsParam2 = 0;
		
		default:
			break;
	}
}

Entity::Entity(){
	this->entityType = ENTITY_TYPES::Undefined;
	this->drawable = false;
}

Entity::~Entity(){
	//this->currentTile->removeEntity();
}

ENTITY_TYPES Entity::getType(){
	return this->entityType;	
}

void Entity::setType( ENTITY_TYPES type ){
	this->entityType = type;
}

int Entity::addToInventory( Entity e ){
	std::string pickup_msg;
	bool alreadyHas = false;
	
	// if inventory already has this type of item, increment the 'count' property
	// otherwise, add it into the inventory list
	if( this->inventory.size() == 0 ){
		alreadyHas = false;
	} else {
		for( int i = 0; i < this->inventory.size(); ++i ){
			if( this->inventory[i].entityType == e.entityType ){ // already has item
				this->inventory[i].count++;
				alreadyHas = true;
				break;
			}
		}
	}
	
	// add item if not in inventory yet
	if( !alreadyHas ){
		this->inventory.push_back( e );
		this->inventory.back().index = this->inventory.size() - 1;
		this->inventory.back().owner = this;
	}
	
	// At this point, one way or another, item has been added
	// into inventory, so print a message declaring so.
	printf( "%s picked up %s.\n", this->toString().c_str(), e.toString().c_str() );
	pickup_msg = this->toString() + " picked up " + e.toString() + ".";
	addQueueMessage( pickup_msg );
	
	// play pickup sound (only if PlayerGuy )
	if( this->entityType == ENTITY_TYPES::PlayerGuy )
		playSound( SOUNDS::PickupSmall );
	
	return this->inventory.size();
}

void Entity::deleteFromInventory( unsigned int index ){
	// is 'index' out of range?
	if( index >= this->inventory.size() ){
		printf( "Error: Inventory index out-of-range for Entity: %s\n", this->toString().c_str() );
		return;
	}
	
	// erase the item from the inventory list
	this->inventory.erase( this->inventory.begin() + index );
	
	// go thru vector list and re-assign ALL indexes (because we do not
	// allocate/reserve memory for inventory vector lists, they could
	// be quite volatile and so just to be sure, re-assign the whole thing.
	// It shouldn't be a large list, anyway.)
	for( int i = 0; i < this->inventory.size(); ++i ){
		this->inventory[i].index = i;
	}
}

bool Entity::hasMatchingKey( Entity* lock, bool deleteKey ){
	for( int i = 0; i < this->inventory.size(); ++i ){ // scan thru the inventory
		Entity* e = &this->inventory[i];
		if( e->getType() == ENTITY_TYPES::Key ){ // is it a key?
			if( e->flagsParam1 == lock->flagsParam1 ){ // if they match, then it's a valid pair
				if( deleteKey ){
					this->deleteFromInventory( e->index );
				}
				
				return true;
			}
		}
	}
}



// Entity Update
void Entity::update(){
	
	// update according to entity type
	switch( this->entityType ){
		
		case ENTITY_TYPES::Undead:
			updateUndead( this );
			break;
		
		default:
			break;
	}
	
	
}




std::string Entity::getActionString(){
	std::string sVal;
	
	switch( this->action ){
		case ENTITY_ACTIONS::Move:
			sVal = "move";
			break;
		
		default:
			break;
	}
	
}

void Entity::setAction( ENTITY_ACTIONS action ){
	// update here if any logic needs to be
	// implemented for changing actions,
	// otherwise the current action is simply
	// overwritten...
	
	this->action = action;
}

ENTITY_ACTIONS Entity::getAction(){
	return this->action;
}

MapTile* Entity::getCurrentTile(){
	return this->currentTile;
}

bool Entity::setCurrentTile( MapTile* t ){
	if( t == nullptr )
		return false;
		
		this->currentTile = t;
		
	return true;
}

void Entity::getLookingAt( int &x, int &y ){
	x = this->x;
	y = this->y;
	
	switch( this->facing ){
		case DIRECTION::North:
			y--;
			break;

		case DIRECTION::South:
			y++;
			break;
			
		case DIRECTION::East:
			x++;
			break;

		case DIRECTION::West:
			x--;
			break;
			
		
		default:
			break;
	}
	
	return;
}

std::string Entity::toString(){
	std::string typeString;

	switch( this->entityType ){
		case ENTITY_TYPES::PlayerGuy:
			typeString = "You";
			break;
			
		case ENTITY_TYPES::Key:
			if( this->flagsParam1 > 0 ){ // this isn't a zombie key
				typeString = "a " + LockDescriptors[this->flagsParam1-1] + " key";
			}
			break;
			
		case ENTITY_TYPES::LockableBarrier:
			if( (this->flags & ENTITY_FLAGS_LOCKED) == ENTITY_FLAGS_LOCKED ) //it's locked
				typeString = "a " + LockDescriptors[this->flagsParam1-1] + " lock";
				
			if( (this->flags & ENTITY_FLAGS_UNLOCKED) == ENTITY_FLAGS_UNLOCKED )
				typeString = "It's unlocked.";
				
			break;
			
		case ENTITY_TYPES::Undead:
			typeString = "an Undead";
			break;

		default:
			typeString = "[undefined ENTITY" + std::to_string((int)this->entityType ) + "]";
			break;
	}

	return typeString;
}

int Entity::getIndex(){
	return this->index;
}

bool Entity::isDrawable(){
	return this->drawable;
}

////////////////////////////////////
// MapSet
////////////////////////////////////
MapSet::MapSet(){
	this->szWidth = 0;
	this->szHeight = 0;
	this->tileCount = 0;
	this->ren = nullptr;	
}

void MapSet::init( int szWidth, int szHeight, SDL_Renderer *ren, std::string tileset_path ){
	this->szWidth = szWidth;
	this->szHeight = szHeight;
	this->tileCount = szWidth * szHeight;
	this->ren = ren;
	
	// Load tile-set image (image containing a 'palette' of tiles)
	printf( "Loading tileset... " );
	this->tile_image = loadTexture( tileset_path, ren );
	
	// populate the 'tileset' vector list -- first time
	int tileX = 0, tileY = 0;
	for( int i = 0; i < this->tileCount; ++i ){
		MapTile t;
		t.x = tileX;
		t.y = tileY;
		t.setToGrass(); // default is grass
		this->tileset.push_back( t );
		
		tileX++;
		if( tileX >= szWidth ){
			tileX = 0;
			tileY++;
		}
	}
	
	// Draw scale default is 2
	this->zoomLevel = DEFAULT_DRAWSCALE;
	
	// Allocate entities list with a pre-designated
	// maximum number of entities.
	this->entities.reserve( MAX_ENTITIES );
	this->entities.clear();
	
	// the 'player' is always entity at index 0
	int playerX, playerY;
	playerX = szWidth / 2; playerY = szHeight / 2;
	this->addEntity( createNewPlayer( playerX, playerY, this->getTileAt(playerX, playerY) ));
	
	return;
}

MapSet::MapSet( int szWidth, int szHeight, SDL_Renderer *ren, std::string tileset_path ){
	init( szWidth, szHeight, ren, tileset_path );
}

MapSet::~MapSet(){
	SDL_DestroyTexture( this->tile_image );
}

//////////////
Entity* MapSet::getPlayerGuy(){
	return &this->entities[0];	
}

Entity* MapSet::getEntityAt( int x, int y ){
	// return null if out of bounds
	if( x < 0 || x >= this->szWidth ) return nullptr;
	if( y < 0 || y >= this->szHeight ) return nullptr;

	MapTile* t = this->getTileAt( x, y );
	if( t->hasEntity() ){
		Entity* e = (Entity*)t->getEntity();
		return e;
	}
	
	// if the tile doesn't have an entity, return null
	return nullptr;
}

int MapSet::getWidth(){
	return this->szWidth;	
}

int MapSet::getHeight(){
	return this->szHeight;
}

// fillTrees
void MapSet::fillTrees( unsigned int density ){
	int x, y, i;
	
	unsigned int treeCount = 0;
	
	printf( "* Planting trees...\n" );
	for( i = 0; i < density; ++i ){
		x = rand() % this->szWidth;
		y = rand() % this->szHeight;
		if( this->getEntityAt( x, y ) == nullptr ){ // nothing there
			Entity e( x, y, ENTITY_TYPES::TreeBuilder, this->getTileAt( x, y ));
			this->addEntity( e );
			treeCount++;
		}
	}
	
	// spawn neighbors
	for( int j = 0; j < density; ++j ){
		for( i = 0; i < this->entities.size(); ++i ){   // Entire entities list, but only checking for trees
			Entity* p = &this->entities[i];
			if( p->getType() == ENTITY_TYPES::TreeBuilder ){ // proceed only if its a tree
				x = (rand() % 3) - 1;
				y = (rand() % 3) - 1;
				if( x!=0 && y!=0 ){ // If x is non-zero AND y is non-zero, avoids base location and puts on diagonal
					x += p->x;
					y += p->y;
					
					// proceed only if within the bounds of the map
					if( x >= 0 && x < this->szWidth && y >= 0 && y < this->szHeight ){
						MapTile* t = this->getTileAt( x, y );

						// Must be grass with nothing on it
						if( this->getEntityAt( x, y ) == nullptr && t->baseType == BASE_TYPES::Grass ){ 
							Entity ne( x, y, ENTITY_TYPES::TreeBuilder, t );
							this->addEntity( ne );
							treeCount++;
						}
					}
				}
			}
		}
	}
	
	printf( "- %s total trees have been planted.\n", getPrettyNumber( treeCount ).c_str() );
	
	// Commit tree entities to map tiles, and delete entities
	for( i = this->entities.size() - 1; i > 0; --i ){ // start at the end because the list is being shortened as we go
		Entity* z = &this->entities[i];
		if( z->getType() == ENTITY_TYPES::TreeBuilder ){ // TreeBuilders make permanent tree base tiles
			MapTile* mt = this->getTileAt( z->x, z->y );
			mt->setType( BASE_TYPES::Tree ); 
			mt->health = z->health;	// copy tree health from TreeBuilder entity
			
			this->deleteEntity( i );
		}
	}
}


// fillGrass
void MapSet::fillGrass(){
	for( int i = 0; i < this->tileCount; ++i ){
		this->tileset[i].setToGrass();
		
		// we also want to eliminate playerspace here
		this->tileset[i].isPlayerspace = false;
	}
	
	// since whole map is just grass, reset building count
	buildingIterator = 0;
	
	return;
}

// addEntity
bool MapSet::addEntity( Entity e ){
	// The entity 'e' must have it's MapTile referenced.
	// This function commits the entity to the entity list
	// and re-configures the specific map tile to point to
	// the new memory location in the vector list,
	// as this "renegade" entity will be destroyed
	// once it's commited to the vector list.
	MapTile* t = e.getCurrentTile();
	t->removeEntity();
	this->entities.push_back( e );
	t->addEntity( &this->entities.back() );
	this->entities.back().index = this->entities.size() - 1;
	
	return true;
}

// deleteEntity
bool MapSet::deleteEntity( int index ){
	Entity* e = &this->entities.at( index );
	MapTile* t = e->getCurrentTile();
	int i;
	
	// remove reference to entity to be deleted
	t->removeEntity();
	
	// erase the item from the list
	this->entities.erase( this->entities.begin() + index );
	
	// go thru list and re-designate pointers
	// and index values
	for( i = index; i < this->entities.size(); ++i ){
		t = this->entities[i].getCurrentTile();
		t->addEntity( &this->entities[i] );
		this->entities[i].index = i;
	}
	
	return true;
}


void MapSet::showPlayerspace(){
	MapTile* t;
	for( int y = 0; y < szHeight; ++y ){
		for( int x = 0; x < szWidth; ++x ){
			t = getTileAt( x, y );
			if( t->isPlayerspace ){
				t->tileset_x = 0;
				t->tileset_y = 16;
			}
		}
	}
}

void MapSet::hidePlayerspace(){
MapTile* t;
	for( int y = 0; y < szHeight; ++y ){
		for( int x = 0; x < szWidth; ++x ){
			t = getTileAt( x, y );
			if( t->isPlayerspace ){
				t->setToGrass();	// this only overwrites baseType and tileset coords, everything else is retained
			}
		}
	}	
}

void MapSet::updateEntities(){
	
	for( int i = 0; i < this->entities.size(); ++i ){
		this->entities[i].update();
	}
	
	return;
}


// addDoor
void MapSet::addDoor( int x, int y ){
	MapTile* t = this->getTileAt( x, y );
	t->setType( BASE_TYPES::Door );
}

// putBuilding
MapTile* MapSet::putBuilding( unsigned int left, unsigned int top, unsigned int right, unsigned int bottom ){
	unsigned int x, y;
	int x1, y1, x2, y2;
	
	bool boundary = false, hasDoorway = false, inCorner = false;
	MapTile *t, *nextToDoor;
	
	printf( "* Constructing a building...\n" );
	
	// increment the counter
	buildingIterator++;
	
	// Randomly leave one boundary tile open, for a doorway
	unsigned int DoorwayNumber = (bottom - top) + (right - left)*2;
	DoorwayNumber = rand() % DoorwayNumber;
	unsigned int DoorwayIterator = 0;
	unsigned int floodFillResult = 0;
	
	for( y = top; y < bottom; ++y ){
		for( x = left; x < right; ++x ){
			t = getTileAt( x, y );
			
			// check if boundary, build wall block if true
			if( y == top || y == bottom - 1 ) boundary = true;
			if( x == left || x == right - 1 ) boundary = true;
			
			// don't put door on a corner tile
			if( x == left && y == top ) inCorner = true;
			if( x == right - 1 && y == top ) inCorner = true;
			if( x == left && y == bottom - 1 ) inCorner = true;
			if( x == right - 1 && y == bottom - 1 ) inCorner == true;
			
			if( boundary ){
				DoorwayIterator++;
				if( DoorwayIterator > DoorwayNumber && hasDoorway == false && !inCorner ){
					// drop a door in
					t->setType( BASE_TYPES::WoodPlanks );
					this->addDoor( x, y );
					hasDoorway = true;
					
					// add a LockableBarrier entity
					Entity doorLock( x, y, ENTITY_TYPES::LockableBarrier, t );
					doorLock.flagsParam1 = buildingIterator;	// unique ID for lock/key pair
					this->addEntity( doorLock );

					// floodfill check
					// north
					if( y == top )
						nextToDoor = getTileAt( x, y - 1 );
					// south
					if( y == bottom-1 )
						nextToDoor = getTileAt( x, y + 1 );
					// east
					if( x == right-1 )
						nextToDoor = getTileAt( x + 1, y );
					// west
					if( x == left )
						nextToDoor = getTileAt( x - 1, y );
				}
				else
				{
					t->setType( BASE_TYPES::Wall );
				}
			}
			else
			{
				t->setType( BASE_TYPES::WoodPlanks );
			}
			
			boundary = false;
			inCorner = false;
		}
	}
	
	// clear trees around building for easy access
	x1 = left - 3;
	if( x1 < 0 ) x1 = 0;
	x2 = right + 3;
	if( x2 > this->szWidth )x2 = this->szWidth;
	y1 = top - 3;
	if( y1 < 0 ) y1 = 0;
	y2 = bottom + 3;
	if( y2 > this->szHeight) y2 = this->szHeight;
	
	int treeCount = 0;
	for( y = y1; y < y2; ++y ){
		for( x = x1; x < x2; ++x ){
			t = getTileAt( x, y );
			if( t->baseType == BASE_TYPES::Tree ){
				t->setToGrass();
				treeCount++;
			}
		}
	}
	
	printf( "- Building complete. Cleared %d trees.\n", treeCount );
	
	// Return the tile outside the front door
	return nextToDoor;
}

// Puts a key somewhere in the level for the most-recently created building
bool MapSet::putKey( unsigned int x, unsigned int y ){
	if( x > this->szWidth - 1 ) return false;
	if( y > this->szHeight - 1 ) return false;
	
	MapTile* t = this->getTileAt( x, y );
	
	Entity key( x, y, ENTITY_TYPES::Key, t );
	key.flagsParam1 = buildingIterator;
	
	this->addEntity( key ); 
	
	return true;
}

// getTileAt
MapTile* MapSet::getTileAt( int x, int y ){
	if( x < 0 || x >= this->szWidth ) return nullptr;
	if( y < 0 || y >= this->szHeight ) return nullptr;
	
	return &this->tileset[ (y * this->szWidth) + x ];	
}

// setViewRect
void MapSet::setViewRect(){
	unsigned int viewWidth = WINDOW_WIDTH / (TILEWIDTH * this->zoomLevel);
	unsigned int viewHeight = WINDOW_HEIGHT / (TILEHEIGHT * this->zoomLevel);
	int left, top;
	
	Entity p = this->entities[0]; // player entity

	left = p.x - ( viewWidth / 2 );
	top = p.y - ( viewHeight / 2 );
	
	// check boundary
	if( left < 0 ) left = 0;
	if( top < 0 ) top = 0;
	
	if( (left + viewWidth) >= this->szWidth ) left = this->szWidth - viewWidth;
	if( (top + viewHeight) >= this->szHeight ) top = this->szHeight - viewHeight;
	
	this->viewRect.x = left;
	this->viewRect.y = top;
	this->viewRect.w = viewWidth;
	this->viewRect.h = viewHeight;
	
	return;
}

void MapSet::increaseZoom(){
	this->zoomLevel++;
	if( this->zoomLevel > 2 ) this->zoomLevel = 2;
	
	return;
}

void MapSet::decreaseZoom(){
	this->zoomLevel--;
	if( this->zoomLevel < 1 ) this->zoomLevel = 1;
	
	return;
}

// draw
int MapSet::render(){
	int x = 0; int y = 0, i = 0;
	int offsetX, offsetY;
	
	setViewRect();
	offsetX = this->viewRect.x;
	offsetY = this->viewRect.y;
	
	// Draw tiles. Entities are rendered on top, if any.
	for( y = this->viewRect.y; y < this->viewRect.y + this->viewRect.h; ++y ){
		for( x = this->viewRect.x; x < this->viewRect.x + this->viewRect.w; ++x ){
			MapTile* t = getTileAt( x, y );
			
			drawTile( this->ren, *t, t->x - offsetX, t->y - offsetY, this->tile_image, this->zoomLevel );
		}
	}
	
	return 0;
}

std::string MapSet::toString(){
	
}

////////////////////////////////////////////////////
// Other Functions
////////////////////////////////////////////////////

Entity createNewPlayer( int x, int y, MapTile* tile ){
	printf( "Creating player...\n" );
	Entity p( x, y, ENTITY_TYPES::PlayerGuy, tile );
	return p;
}

bool rectOutOfScreen( SDL_Rect r ){
	bool result = false;
	if( r.x < 0 || r.x >= WINDOW_WIDTH ) result = true;
	if( r.y < 0 || r.y >= WINDOW_HEIGHT ) result = true;

	return result;
}

// Note: This is the only place when dealing with "screen-space" AKA pixels on-screen.
// Every other occurence of handling positions and locations are in world-space (i.e. increments of 1, not TILEWIDTH, etc...)
int drawTile( SDL_Renderer *ren, MapTile tile, int dst_x, int dst_y, SDL_Texture *img, unsigned char scale ){
	SDL_Rect src, dst;
	dst.w = TILEWIDTH * scale;
	dst.h = TILEHEIGHT * scale;
	dst.x = dst_x * TILEWIDTH * scale;
	dst.y = dst_y * TILEHEIGHT * scale;
	
	src.x = tile.tileset_x;
	src.y = tile.tileset_y;
	src.w = TILEWIDTH;
	src.h = TILEHEIGHT;
	
	if( rectOutOfScreen( dst ) ) return 1;
	
	SDL_RenderCopy( ren, img, &src, &dst );
	
	// draw entity on top of tile, if available
	if( tile.hasEntity() ){
		Entity* e = (Entity*)tile.getEntity();
		if( e->isDrawable() ){
			src.x = e->tileset_x;
			src.y = e->tileset_y;
			src.w = TILEWIDTH;
			src.h = TILEHEIGHT;
			
			SDL_RenderCopy( ren, img, &src, &dst );
		}
	}
	
	return 0;
}

MapTile entityToMapTile( Entity e ){
	MapTile t( e.x, e.y, e.tileset_x, e.tileset_y, BASE_TYPES::Entity );
	return t;	
}


bool isBlockable( MapTile* t ){
	if( t == nullptr ) return false;
	
	bool result = false;
	
	switch( t->baseType ){
	
		case BASE_TYPES::Tree:
			result = true;
			break;
			
		case BASE_TYPES::Wall:
			result = true;
			break;
			
		default:
			break;
	}
	
	return result;
}


bool isBlockable( Entity* e ){
	if( e == nullptr ) return false;
	
	bool result = false;
	ENTITY_TYPES type = e->getType();
	
	switch( type ){
			
		default:
			break;
	}
	
	return result;
}


unsigned int doFloodFill( MapSet* map, int x, int y ){
	int flCount = 1; // 1, because we will definitely be flipping the tile at x, y
	unsigned int totalCount = 0;
	int at_x, at_y;
	
	// convert the tile at x, y
	MapTile* t = map->getTileAt( x, y );
	t->isPlayerspace = true;
	
	while( flCount > 0 ){
		
		// reset counter
		totalCount += flCount;
		flCount = 0;
		
		for( at_y = 0; at_y < map->getHeight(); ++at_y ){
			for( at_x = 0; at_x < map->getWidth(); ++at_x ){

				t = map->getTileAt( at_x, at_y );
				
				if( t->isPlayerspace ){
					// north
					t = map->getTileAt( at_x, at_y - 1 );
					if( t != nullptr && t->baseType == BASE_TYPES::Grass && !t->isPlayerspace ){
						t->isPlayerspace = true;
						flCount++;
					}
					// south
					t = map->getTileAt( at_x, at_y + 1 );
					if( t != nullptr && t->baseType == BASE_TYPES::Grass && !t->isPlayerspace ){
						t->isPlayerspace = true;
						flCount++;
					}
					// east
					t = map->getTileAt( at_x + 1, at_y );
					if( t != nullptr && t->baseType == BASE_TYPES::Grass && !t->isPlayerspace ){
						t->isPlayerspace = true;
						flCount++;
					}
					// west
					t = map->getTileAt( at_x - 1, at_y );
					if( t != nullptr && t->baseType == BASE_TYPES::Grass && !t->isPlayerspace ){
						t->isPlayerspace = true;
						flCount++;
					}
				}
			}
		}
	}

	return totalCount;
}

bool inRect( SDL_Rect r, int x, int y ){
	bool result = false;
	if( (x >= r.x) && (x <= (r.x + r.w)) && (y >= r.y) && (y <= (r.y + r.h)) ) result = true;
	return result; 
}

std::string getPrettyNumber( unsigned int val ){
	std::string rawVal = std::to_string( val );

	std::string reverseString = "";
	std::string finalString = "";

	int i, commaCounter = 0;
	
	// copy number backwards, adding a comma every 3 characters
	for( i = rawVal.length() - 1; i > -1; --i ){
		commaCounter++;
		if( commaCounter > 3 ){
			reverseString += ",";
			commaCounter = 0;
		}

		reverseString += rawVal[i];
	}
	
	// Copy reverse string backwards so it's back to normal direction
	for( i = 0; i < reverseString.length(); i++ ){
		finalString += reverseString.at( reverseString.length() - (i + 1) );
	}
	
	return finalString;
}


bool tryEntityAction( Entity* actingEntity, Entity* targetEntity, MapSet* map ){
	
	// print messages if the player is performing actions
	bool isPlayer = ( actingEntity == map->getPlayerGuy() );
	bool result = true; // returns true if player movement is valid
	std::string msg;
	ENTITY_ACTIONS action = actingEntity->getAction();
	
	// if target is obtainable, try picking it up
	if( (targetEntity->flags & ENTITY_FLAGS_OBTAINABLE) == ENTITY_FLAGS_OBTAINABLE ){
		// Check if action is 'Pickup', if so, put target into inventory of acting entity
		// and disassociate MapTile underneath (entity is now 'detached' from map, or hidden
		msg = "This " + targetEntity->toString() + " can be picked up.";
		result = false;
	}
	
	// if target is locked, see if acting entity has the right key
	if( (targetEntity->flags & ENTITY_FLAGS_LOCKED) == ENTITY_FLAGS_LOCKED ){
		// check to see if acting entity has the right key
		// If so, unlock the thing and allow player to move through
		// If not, simply inform entity that it's locked
		msg = targetEntity->toString();
		result = false;
	} 
	
	// print message
	if( isPlayer )
		printf( "%s\n", msg.c_str() );
		
		return result;
}


// NOTE: moveEntity() should have all logic for ANY entity that wants to
// have valid movement within a MapSet. This includes PlayerGuy and Undead, and
// anything else similar.
bool moveEntity( MapSet* map, Entity* e, int x, int y ){
	
	bool validMovement = false;	// Should we actually move the entity?
	bool isPlayerGuy = ( e == map->getPlayerGuy() );
	int i;
	
	// don't bother if coords are out-of-bounds
	if( x < 0 || x > map->getWidth() - 1 ) return false;
	if( y < 0 || y > map->getHeight() - 1 ) return false;
	
	// let's see what's in the tile we're moving to
	MapTile* tileNew = map->getTileAt( x, y );
	
	// let's check first if the new tile already has an entity on it
	if( tileNew->hasEntity() ){
		Entity* newTileEntity = (Entity*)tileNew->getEntity();
		
		// perform actions that only a player can do
		if( isPlayerGuy ){
			
			// can we pick it up?
			if( (newTileEntity->flags & ENTITY_FLAGS_OBTAINABLE) == ENTITY_FLAGS_OBTAINABLE ){
				Entity entityCopy = *newTileEntity; // copy the data in the entity
				map->deleteEntity( newTileEntity->index ); // remove entity from the map's list of entities
				e->addToInventory( entityCopy ); // put the entity in the player's inventory
				validMovement = true; // Now that the tile is empty, we can move the player onto it
			}
			
			// if it's locked, do we have a key to unlock it?
			if( (newTileEntity->flags & ENTITY_FLAGS_LOCKED) == ENTITY_FLAGS_LOCKED ){
				if( e->hasMatchingKey( newTileEntity, true )){	// do we have a key that works? (get rid of key if we do)
					map->deleteEntity( newTileEntity->index );	// remove the locked barrier
					validMovement = true; 
				}
			}
		}
		else {
			// Entity isn't PlayerGuy:
			// Actions of moving entities reacting to existing entities on tiles go here.
			// (Should Undead pick up keys? should squirrels pick up objects? etc...)
			// Otherwise, validMovement is FALSE here
			
			// check to see if newTileEntity == PlayerGuy, and if entity being moved
			// is obtainable, in which case, add moving Entity to PlayerGuy inventory,
			// otherwise, validMovement is FALSE (which lets other moving entities (undead, etc)
			// perform their own handling on what to do
		}
	}
	// else, tile has no entity, so just check tile properties
	else {
		// do movement if valid grass/playerspace
		if((tileNew->baseType == BASE_TYPES::Grass) && (tileNew->isPlayerspace ))
			validMovement = true;
			
		// do movement if wooden floor
		if( tileNew->baseType == BASE_TYPES::WoodPlanks )
			validMovement = true;
			
		// do movement thru rubble
		if( tileNew->baseType == BASE_TYPES::Rubble )
			validMovement = true;
			
		// PlayerGuy can move thru unlocked doors (Will be blocked by LockableBarrier above)
		if( (tileNew->baseType == BASE_TYPES::Door) && isPlayerGuy )
			validMovement = true;
			
		if( isBlockable( tileNew )){
			// Do something here if entity needs feedback for Blockable tiles
			// (Should they attempt to knock it down/destroy it?)
		}
	}
	
	// Perform movement if allowed
	if( validMovement ){
		// remove entity from old tile
		MapTile* tileOld = e->getCurrentTile();
		tileOld->removeEntity();
		
		// relocate
		e->x = tileNew->x;
		e->y = tileNew->y;
		
		// give the entity a tile
		e->setCurrentTile( tileNew );
		
		// Let the tile know what entity is on it -- NOTE: addEntity() overwrites but we already checked that it's blank
		tileNew->addEntity( e );
		
		// miscellaneous actions for different tile types (play sound, etc...)
		switch( tileNew->baseType ){
			case BASE_TYPES::Door:
				playSound( SOUNDS::Door );
			default:
				break;
		}
	}
	
	// return true if movement occured
	return validMovement;
}

// An entity is attacking a tile such as a tree or a wall
// NOTE: "attacker" entity could be an equipped item, in which case,
// the "strength" of that item is used.
bool attackTile( MapSet* map, Entity* attacker, MapTile* tile ){
	bool isDeath = false;
	bool doHarm = false;
	bool attackerIsPlayerGuy = ( attacker == map->getPlayerGuy() );
	
	// what type of tile is being attacked?
	switch( tile->baseType ){
		
		// Perform action on a tree
		case BASE_TYPES::Tree:
			
			// if 'attacker' is a chainsaw,
			// first check if PlayerGuy has fuel, and if so,
			// decrement fuel 'count' and perform attack using chainsaw,
			// if not, do nothing (chainsaw fuel is empty.)
			
			tile->health -= attacker->strength;
			
			// is it killed?
			if( tile->health <= 0 ){
				playSound( SOUNDS::Destroy );			// sound for destroying tree
				tile->setToGrass();						// tree gone, make grass
				tile->isPlayerspace = false;       		// ensure it isn't playerspace (yet!) for proper floodfill
				doFloodFill( map, tile->x, tile->y );	// recalculate playerspace
				isDeath = true;
			}
			else {
				playSound( SOUNDS::Attack );
			}
			break;
			
		// Wall destruction
		case BASE_TYPES::Wall:
			
			if( attackerIsPlayerGuy ){
				// If Player has selected "wood" from inventory,
				// add health to wall and subtract Wood from inventory
			} else {
				tile->health -= attacker->strength;
				playSound( SOUNDS::Attack );
			}
			
			// Change wall appearance based on health points
			if( tile->health < 100 ){	// a few cracks
				tile->tileset_x = 128;
				tile->tileset_y = 0;
			}
 
			if( tile->health < 50 ){	// severely cracked
				tile->tileset_x = 144;
				tile->tileset_y = 0;
			}
			
			if( tile->health <= 0 ){	// completely destroyed (and no longer blockable)
				tile->tileset_x = 160;
				tile->tileset_y = 0;
				tile->setType( BASE_TYPES::Rubble );
				playSound( SOUNDS::Destroy );
				isDeath = true;
			}
			break;
		
		// attack anything on non-blockable tiles (do not BREAK, just go to DEFAULT:)
		case BASE_TYPES::Grass:
			// Check entity type of "attacker", perform action as necessary.
			// (i.e., if attacker==wood with a "count" of at least 5, build a wall
			// and decrement "count" by 5.
		
		case BASE_TYPES::GarlicFlower:
			// If "attacker" == trowel, harvest garlic
			// Create new Garlic entity, transform Tile into Grass,
			// and give it the Garlic entity for player to acquire
		case BASE_TYPES::Rubble:
			if( attackerIsPlayerGuy ){
				// if "wood" is selected in inventory
				// then subtract an amount and use
				// tile->setType() to change back to Wall
			}
		case BASE_TYPES::Door:
		case BASE_TYPES::WoodPlanks:
		
		// Default: just do this for most tiles (above)
		default:
			if( tile->hasEntity() ){
				Entity* e = (Entity*)tile->getEntity();
				
				// Decide what to do for each type of entity
				ENTITY_TYPES eType = e->getType();
				switch( eType ){
					case ENTITY_TYPES::PlayerGuy:	// player
						doHarm = true;
						
						break;
					case ENTITY_TYPES::Undead:	// undead
						doHarm = true;
						
						break;
						
					// for other types, perform whatever specific action is needed
					// i.e., create Wooden Stakes with a Lathe
					case ENTITY_TYPES::Lathe:
						// if 'attacker' is wood,
						// as long as count > 0,
						// subtract one wood, create entity Wooden Stake,
						// and "moveEntity()" same tile as "attacker" to perform
						// automatic inventory acquire
						break;
				}
				
				if( doHarm ){
					e->health -= attacker->strength;
				
					if( e->health <= 0 ){
						map->deleteEntity( e->index );
						playSound( SOUNDS::Destroy );
						isDeath = true;
					}
					else {
						playSound( SOUNDS::Attack );
					}
				}
			}		
	}
	
	
	return isDeath;
}









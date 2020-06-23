// Objects Header

#pragma once

#include "main.h"

#define TILESET_WIDTH 512
#define TILESET_HEIGHT 1024
#define TILEWIDTH 16
#define TILEHEIGHT 16
#define DEFAULT_DRAWSCALE 2
#define TILE_FILENAME "tiles.png"
#define NUM_GRASS_TILES 6
#define NUM_GRAVEL_TILES 3

#define MAX_ENTITIES 600000

enum DIRECTION { North, South, East, West };

enum class BASE_TYPES {
	Void = 0,
	Entity,
	MapSet,
	Rubble,
	Grass,
	GrassHighlight,
	WoodPlanks,
	Wall,
	Door,
	GarlicFlower,
	Water,
	Tree
};

enum class ENTITY_TYPES {
	PlayerGuy = 0,
	TreeBuilder,
	LockableBarrier,
	Key,
	Undead,
	Wood,
	Garlic,
	Lathe,
	WoodenStake,
	Axe,
	Trowel,
	Chainsaw,
	Fuel,
	Undefined
};

enum class ENTITY_ACTIONS {
	None = 0,
	Move,
	Inspect,
	Pickup,
	Loot,
	Push,
	Attack,
	Open,
	Lock,
	Unlock
};

// Generic Entity flags
const unsigned int ENTITY_FLAGS_LOCKED = 1;
const unsigned int ENTITY_FLAGS_UNLOCKED = 2;
const unsigned int ENTITY_FLAGS_OBTAINABLE = 4;
const unsigned int ENTITY_FLAGS_WEAPON = 8;
const unsigned int ENTITY_FLAGS_POWERUP = 16;
const unsigned int ENTITY_FLAGS_DEAD = 32;
const unsigned int ENTITY_FLAGS_BLOCKABLE = 64;

// Specific Undead flags
const unsigned int UNDEAD_FLAGS_WANDER = 1;
const unsigned int UNDEAD_FLAGS_PERSUE = 2;
const unsigned int UNDEAD_FLAGS_DORMANT = 3;
const unsigned int UNDEAD_FLAGS_FLEE = 4;
// Undead WANDER extra flags
const unsigned int UNDEAD_WALK_LEFT = 0;
const unsigned int UNDEAD_WALK_RIGHT = 1;
const unsigned int UNDEAD_WALK_UP = 2;
const unsigned int UNDEAD_WALK_DOWN = 3;
const unsigned int UNDEAD_WALK_DWELL = 4;

// BaseObject
class BaseObject {
	public:
	int x;
	int y;
	int tileset_x;
	int tileset_y;
	unsigned int health;
	BASE_TYPES baseType;
	
	virtual std::string toString() =0;
	std::string locationToString();
};

// MapTile
class MapTile : public BaseObject {
	BaseObject* entitySittingOnTile;
	bool holdingEntity;
	
	public:
	// public member properties
	bool isPlayerspace;
	
	// public member methods
	MapTile();
	MapTile( int x, int y, int tileset_x, int tileset_y, BASE_TYPES type );
	void setToGrass();
	void setType( BASE_TYPES type );
	bool hasEntity();
	bool addEntity( BaseObject* e );
	bool removeEntity();
	BaseObject* getEntity();
	std::string toString();
};

// Entity
class Entity : public BaseObject {
	ENTITY_ACTIONS action;
	ENTITY_TYPES entityType;
	std::vector<Entity> inventory;
	MapTile* currentTile;
	bool drawable;
	Entity* owner;
	
	public:
	// constructor/destructor
	Entity( int x, int y, ENTITY_TYPES type, MapTile* currentTile );
	Entity(); // blank entity
	~Entity();
	
	// public properties;
	unsigned int strength;	// Strength determines how much health is lost from receiving BaseObject being attacked by this entity
	unsigned int flags;
	int flagsParam1;
	int flagsParam2;
	int count;	// this is for inventory purposes, as opposed to making the list bigger with similar entities, just increment this
	DIRECTION facing;
	int index;
	
	// public methods
	void 			update();
	ENTITY_TYPES 	getType();
	void			setType( ENTITY_TYPES type );
	MapTile* 		getCurrentTile();
	bool			setCurrentTile( MapTile* t );
	std::string 	toString();
	int 			getIndex();
	void 			getLookingAt( int &x, int &y );
	int 			addToInventory( Entity e );
	void			deleteFromInventory( unsigned int index );
	std::string 	getActionString();
	ENTITY_ACTIONS	getAction();
	void 			setAction( ENTITY_ACTIONS action );
	bool 			isDrawable();
	bool			hasMatchingKey( Entity* lock, bool deleteKey );
};

// MapSet
class MapSet : public BaseObject {
	int szWidth;
	int szHeight;
	int tileCount;
	unsigned char zoomLevel;
	unsigned int keyIterator;
	SDL_Rect viewRect;
	std::vector<MapTile> tileset;
	std::vector<Entity> entities;	// entity 0 shall be reserved for the player entity
	SDL_Renderer *ren;
	SDL_Texture *tile_image;
	
	// private methods
	void setViewRect();
	
	public:
	// constructor/destructor
	MapSet();
	MapSet( int szWidth, int szHeight, SDL_Renderer *ren, std::string tileset_path );
	~MapSet();
	
	// public methods
	void init( int szWidth, int szHeight, SDL_Renderer *ren, std::string tileset_path );
	MapTile* getTileAt( int x, int y );
	Entity* getEntityAt( int x, int y );
	void fillGrass();
	MapTile* putBuilding( unsigned int left, unsigned int top, unsigned int right, unsigned int bottom );
	bool putKey( unsigned int x, unsigned int y );
	int render();
	Entity* getPlayerGuy();
	int getHeight();
	int getWidth();
	void increaseZoom();
	void decreaseZoom();
	void addDoor( int x, int y );
	void fillTrees( unsigned int density );
	std::string toString();
	bool addEntity( Entity e );
	bool deleteEntity( int index );
	void showPlayerspace();
	void hidePlayerspace();
	void updateEntities();
	
};

int drawTile( SDL_Renderer *ren, MapTile tile, int dst_x, int dst_y, SDL_Texture *img, unsigned char scale );
Entity createNewPlayer( int x, int y, MapTile* tile );
MapTile entityToMapTile( Entity e );
bool isBlockable( MapTile* t );
bool isBlockable( Entity* e );
unsigned int doFloodFill( MapSet* map, int x, int y );
std::string getPrettyNumber( unsigned int val );
bool tryEntityAction( Entity* actingEntity, Entity* targetEntity, MapSet* map );
bool moveEntity( MapSet* map, Entity* e, int x, int y );
bool attackTile( MapSet* map, Entity* attacker, MapTile* tile );
bool inRect( SDL_Rect r, int x, int y );

// undead.cpp
int spawnNewUndead( MapSet* map, int num );
void updateUndead( Entity* undead );
void moveUndead( Entity* undead, int directionFlags );
void persuePlayer(Entity* undead, int directionFlags );
void setUndeadTime( unsigned int currentTime, unsigned int maxTime );











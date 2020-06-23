#include "main.h"

#define PLAYER_CLOCK_SPEED 125
#define DAYLIGHT_CYCLE_DURATION 360000
#define ENTITY_CLOCK_SPEED 200

SDL_Window *win;
SDL_Renderer *ren;

MapSet menu;
MapSet map;
PLAYERACTION playerAction;
GAMESTATE gameState;

unsigned int playerClockTick = 0;
unsigned int daylightClockTick = 0;
unsigned int entityClockTick = 0;

bool showNightcolor = true;
bool showPlayerspace = false;


int main(int argc, char** argv) {
	SDL_Event e;
	bool quit = false;
	vividConsole log;
	TextBox txtBox;
	unsigned int currentTick, lastTick;
	float clockAngle;
	
	if( init() > 0 ) return 1;
	
	// TODO: Eventually, game startup should show Main Menu
	// with options to Continue previous progress (load)
	// start a new map, etc...
	// menu.init( 64, 40, ren, "menu.png" );
	// log.printSuccess( "Created menu." );
	
	
	// Create a new world
	// (NOTE: This will be what happens when player selects "Start New Game")
	generateNewWorld( MAP_DEFAULT_WIDTH, MAP_DEFAULT_HEIGHT );
	
	// Enter game as: (GameMode, Menu, Inventory, etc...)
	gameState = GAMESTATE::GameMode;
	
	// Set the time for early morning, after the dead have retreated
	daylightClockTick = 3 * (DAYLIGHT_CYCLE_DURATION / 4);
	
	
	
	// main loop
	// main loop
	// main loop
	log.print( "Let's begin!" );
	while( !quit ){
		
		// Get a snapshot of time
		lastTick = currentTick;
		if( lastTick == 0 ) lastTick = 1;
		currentTick = SDL_GetTicks();
		
		// handle SDL events
		while( SDL_PollEvent( &e )){
			if( e.type == SDL_QUIT )
				quit = true;
		}
		
		// get keyboard state and receive Player input
		playerAction = getPlayerAction();
		
		// Update PlayerGuy based on player input (above)
  		if( currentTick - playerClockTick > PLAYER_CLOCK_SPEED ){
			if( playerAction != PLAYERACTION::None ){
				playerClockTick = currentTick;
				doPlayerAction();
			}
		}
		
		// update entities
		if( currentTick - entityClockTick > ENTITY_CLOCK_SPEED ){
			map.updateEntities();
			entityClockTick = currentTick;
		}
		
		// Update day/night cycle
		daylightClockTick += currentTick - lastTick;
		doDaylightCycle( ren, daylightClockTick, DAYLIGHT_CYCLE_DURATION );
		clockAngle = ((float)daylightClockTick / DAYLIGHT_CYCLE_DURATION) * 360.0f;
		setUndeadTime( daylightClockTick, DAYLIGHT_CYCLE_DURATION );
		
		// render
		SDL_RenderClear( ren );
		
			// determine what needs to be on-screen (game field, pause menu, inventory menu, main menu, etc...)
			doRender();
			
			// tint game screen according to day/night cycle
			if( showNightcolor )
				renderColorize( ren );
			
			// display a clock to show current day/night time
			drawClock( ren, clockAngle + 90.f );
			
			// display any messages as text
			txtBox.setMessageFromQueue();
			txtBox.drawMessage( ren );
			
		// update SDL renderer
		SDL_RenderPresent( ren );
	
	} // mainloop quit...
	
	
	log.print( "Quitting..." );
	// clean up SDL2	
	cleanup();
    return 0;
} // main()




// generateNewWorld
void generateNewWorld(unsigned int mapWidth, unsigned int mapHeight){
	
	unsigned int treeDensity = (mapWidth * mapHeight) / 1000;
	bool goodPlayerspace = false;
	unsigned int validSpace = 0;
	MapTile* atDoorstep;
	
	printf( "Map area is %dx%d\n", mapWidth, mapHeight );
	printf( "Forest density is %d\n", treeDensity );
	
	// Initialize the main map
	map.init( mapWidth, mapHeight, ren, "tiles.png" );
	
	while( goodPlayerspace == false ){

		// make sure all tiles are grass - this resets everything - no entities (besides player) exist yet
		map.fillGrass();
		
		// next, plant all trees
		map.fillTrees( treeDensity );
		
		// Add a building, and see if playerspace is greater than a threshold - too low means trees are blocking much of the gameworld
		atDoorstep = map.putBuilding( 95, 95, 105, 105 );
		printf( "Scanning for player access (player-space)...\n" );
		validSpace = doFloodFill( &map, atDoorstep->x, atDoorstep->y );
		printf( "Found %s tiles of valid player-space.\n", getPrettyNumber( validSpace ).c_str() );
		if( validSpace > 10000 ){
			goodPlayerspace = true;
		}
		
		if( !goodPlayerspace )
			printf( "World discarded... trying again...\n.\n.\n.\n" );
	}
	
	 //Here, the world is deemed acceptable to proceed.
	
	 //Generate Undead
	spawnNewUndead( &map, 50 );
	
	// put a key to the right of the player
	map.putKey( map.getPlayerGuy()->x +1, map.getPlayerGuy()->y );
	
	return;
}


void doPlayerAction() {
	if( gameState == GAMESTATE::GameMode ){	
		
		// Get reference to PlayerGuy
		Entity* player = map.getPlayerGuy();
		MapTile* t;
		int x, y;
		
		// firstly, if user does nothing, then likewise, PlayerGuy should do nothing
		if( isAction( playerAction, PLAYERACTION::None )){
			player->setAction( ENTITY_ACTIONS::None );
			
		}
			
		// Player movement
		if( isAction( playerAction, PLAYERACTION::MoveUp )){
			t = map.getTileAt( player->x, player->y - 1 );
			if( t != nullptr )
				moveEntity( &map, player, t->x, t->y );
				
			player->facing = DIRECTION::North;
		}
		
		if( isAction( playerAction, PLAYERACTION::MoveDown )){
			t = map.getTileAt( player->x, player->y + 1 );
			if( t != nullptr )
				moveEntity( &map, player, t->x, t->y );
			
			player->facing = DIRECTION::South;
		}
		
		if( isAction( playerAction, PLAYERACTION::MoveLeft )){
			t = map.getTileAt( player->x - 1, player->y );
			if( t != nullptr )
				moveEntity( &map, player, t->x, t->y );
			
			player->facing = DIRECTION::West;
		}
		
		if( isAction( playerAction, PLAYERACTION::MoveRight )){
			t = map.getTileAt( player->x + 1, player->y );
			if( t != nullptr )
				moveEntity( &map, player, t->x, t->y );	
			
			player->facing = DIRECTION::East;
		}
			
		// Inspect
		if( isAction( playerAction, PLAYERACTION::Inspect )){
			player->getLookingAt( x, y );
			t = map.getTileAt( x, y );
			if( t != nullptr ){
				
				std::string inspect_msg;
				printf("It's %s. ", t->toString().c_str() );
				inspect_msg = "It's " + t->toString() + ". ";
				if( t->hasEntity() ){
					printf( "There's %s on it.\n", t->getEntity()->toString().c_str() );
					inspect_msg += "There's " + t->getEntity()->toString() + " on it.";
				}
				else {
					printf( "There's nothing else here.\n" );
					inspect_msg += "There is nothing else here.";
				}
				
				addQueueMessage( inspect_msg );
			}
		}
			
		// Attack
		if( isAction( playerAction, PLAYERACTION::Attack )){
			player->getLookingAt( x, y );
			t = map.getTileAt( x, y );
			if( attackTile( &map, player, t ) ){
				// do stuff here if player destroyed something
			}
		}
		
		
		// Drawing Stuff
		///////////////////////////
		// Map zoom
		if( isAction( playerAction, PLAYERACTION::ZoomIn )){
			map.increaseZoom();
		}
			
		if( isAction( playerAction, PLAYERACTION::ZoomOut )){
			map.decreaseZoom();
		}
			
		// Show playerspace
		if( isAction( playerAction, PLAYERACTION::ShowPlayerspace )){
			map.showPlayerspace();
			showPlayerspace = true;
		}
			
		if( isAction( playerAction, PLAYERACTION::HidePlayerspace )){
			map.hidePlayerspace();
			showPlayerspace = false;
		}
		
		// Toggle night darkening	
		if( isAction( playerAction, PLAYERACTION::ToggleNightcolor )){
			showNightcolor = !showNightcolor;
		}
	}
	else
	{
		// Do menu stuff here
	}

	return;	
}



void doRender(){
	switch( gameState ){
		case GAMESTATE::GameMode:
			map.render();
			break;
		case GAMESTATE::MenuMode:

			break;
	}
	
	return;
}

int init(){
	vividConsole log;
	
	// Random seed
	srand( time(NULL) );
	
	// start SDL2
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        log.printError( "Couldn't initialize SDL2!" );
        return 1;
    }
    log.printSuccess( "SDL2 loaded..." );
    
    // initialize SDL image helper library
    if( (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG ){
    	log.printError( "Couldn't initialize SDL Image!" );
		SDL_Quit();
		return 1;	
    }
    log.printSuccess( "SDL Image loaded..." );
    
    // initialize sound
    if( !initSound() )
		return 1;
    
    // create a window
    win = SDL_CreateWindow( WINDOW_TITLE,
							SDL_WINDOWPOS_CENTERED,
							SDL_WINDOWPOS_CENTERED,
							WINDOW_WIDTH,
							WINDOW_HEIGHT,
							SDL_WINDOW_SHOWN );
	if (win == nullptr){
		log.printError( "SDL could not create valid window!" );
		std::cout << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	log.printSuccess( "Window created..." ); 
	
	// create a renderer
	// second param sets preferred device, here we use -1 to let SDL choose the best one for us
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		log.printError( "SDL could not create valid renderer!" );
		std::cout << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	log.printSuccess( "Renderer created..." );
	
	initText( ren );
	
	return 0;	
}


void cleanup(){
	shutdownText();
	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );
	destroySound();
	IMG_Quit();
	SDL_Quit();
}

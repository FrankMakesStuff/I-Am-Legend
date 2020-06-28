#include "main.h"

#define PLAYER_CLOCK_SPEED 125
#define DAYLIGHT_CYCLE_DURATION 240000	// 360000 = 3 minutes per cycle
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
bool quit = false;


int main(int argc, char** argv) {
	SDL_Event e;
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
	generateNewWorld( 800, 800 );
	
	// Enter game as: (GameMode, Menu, Inventory, etc...)
	gameState = GAMESTATE::GameMode;
	
	// Set the time for early morning, after the dead have retreated
	daylightClockTick = 3 * (DAYLIGHT_CYCLE_DURATION / 4);
	daylightClockTick -= 1000;
	
	
	
	// main loop
	// main loop
	// main loop
	log.print( "Let's begin!" );
	while( !quit ){
		
		if( getQuit() ){
			quit == true;
			break;
		}
		
		// Get a snapshot of time
		lastTick = currentTick;
		if( lastTick == 0 ) lastTick = 1;
		currentTick = SDL_GetTicks();
		
		// handle SDL events
		while( SDL_PollEvent( &e )){
			if( e.type == SDL_QUIT )
				quit = true;
		}
		
		// update entities
		if( currentTick - entityClockTick > ENTITY_CLOCK_SPEED ){
			map.updateEntities();
			entityClockTick = currentTick;
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
		
		// Update day/night cycle
		daylightClockTick += currentTick - lastTick;
		doDaylightCycle( ren, daylightClockTick, DAYLIGHT_CYCLE_DURATION );
		clockAngle = ((float)daylightClockTick / DAYLIGHT_CYCLE_DURATION) * 360.0f;
		setUndeadTime( daylightClockTick, DAYLIGHT_CYCLE_DURATION );
		
		// render
		SDL_SetRenderDrawColor( ren, 0, 0, 0, 255 );
		SDL_RenderClear( ren );
		
			// determine what needs to be on-screen (game field, pause menu, inventory menu, main menu, etc...)
			doRender();
			
			// tint game screen according to day/night cycle
			if( showNightcolor )
				renderColorize( ren );
			
			// display a clock to show current day/night time
			drawClock( ren, clockAngle + 90.f );
			
			// display any messages as text
			std::string currentMsg = getQueueMessage();
			txtBox.setMessageFromQueue();
			txtBox.y = WINDOW_HEIGHT - 100;
			txtBox.x = (WINDOW_WIDTH / 2) - (currentMsg.length() / 2)*CHARACTER_WIDTH;
			txtBox.drawMessage( ren );
			
			// show inventory
			drawInventory();
			
			// show health bar
			drawHealth();
			
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
	unsigned int mapCenterX = mapWidth / 2;
	unsigned int mapCenterY = mapHeight / 2;
	unsigned int houseWidth = 10;
	unsigned int houseHeight = 8;
	
	unsigned int houseLeft = mapCenterX - 30;
	unsigned int houseTop = mapCenterY - 11;
	
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
		
		// put streets
		buildRoads( &map, 600, 600, 50 );
		
		// Add a building, and see if playerspace is greater than a threshold - too low means trees are blocking much of the gameworld
		atDoorstep = map.putBuilding( houseLeft, houseTop, houseLeft + houseWidth, houseTop + houseHeight );
		printf( "Discovering path nodes...\n" );
		validSpace = doFloodFill( &map, atDoorstep->x, atDoorstep->y );
		printf( "Found %s tiles of valid player-space.\n", getPrettyNumber( validSpace ).c_str() );
		if( validSpace > 10000 ){
			goodPlayerspace = true;
		}
		
		if( !goodPlayerspace )
			printf( "World discarded... trying again...\n.\n.\n.\n" );
	}
	
	//Here, the world is deemed acceptable to proceed.
	//////////////////////////////////////////////////
	
	// put an axe
	Entity newAxe( ENTITY_TYPES::Axe, map.getTileAt( atDoorstep->x - 1, atDoorstep->y ) );
	map.addEntity( newAxe );
	
	// put a trowel
	Entity newTrowel( ENTITY_TYPES::Trowel, map.getTileAt( atDoorstep->x + 1, atDoorstep->y ) );
	map.addEntity( newTrowel ); 
	 
	// Place a Lathe in the top-right corner
	Entity newLathe( ENTITY_TYPES::Lathe, map.getTileAt( houseLeft + houseWidth - 2, houseTop + 1 ));
	map.addEntity( newLathe );
	
	// plant some garlic
	plantGarlic( &map, 25 );
	
	 //Generate Undead
	spawnNewUndead( &map, 50 );
	
	// Move player into house
	moveEntity( &map, map.getPlayerGuy(), houseLeft + 2, houseTop + 2 );
	
	// put a key to the right of the player
	map.putKey( map.getPlayerGuy()->x +1, map.getPlayerGuy()->y );
	
	printf( "Number of entities: %d\n", map.entities.size() );
	
	return;
}


void doPlayerAction() {
	if( gameState == GAMESTATE::GameMode ){	
		
		// Get reference to PlayerGuy
		Entity* player = map.getPlayerGuy();
		
		// Get a reference to the selected inventory item
		Entity* inv = player->getSelectedInventory();
		
		MapTile* t;
		int x, y;
		
		// firstly, if user does nothing, then likewise, PlayerGuy should do nothing
		if( isAction( playerAction, PLAYERACTION::None )){
			player->setAction( ENTITY_ACTIONS::None );
			
		}
		
		// Should we quit?
		if( isAction( playerAction, PLAYERACTION::RequestExit )){
			quit = true;
		}
			
		// Player movement
		if( player->health > 0 ){
			
			if( isAction( playerAction, PLAYERACTION::MoveUp )){
				t = map.getTileAt( player->x, player->y - 1 );
				if( t != nullptr )
					moveEntity( &map, player, t->x, t->y );	
				
				player->faceDirection( DIRECTION::North );	
			}
			
			if( isAction( playerAction, PLAYERACTION::MoveDown )){
				t = map.getTileAt( player->x, player->y + 1 );
				if( t != nullptr )
					moveEntity( &map, player, t->x, t->y );	
					
				player->faceDirection( DIRECTION::South );	
			}
			
			if( isAction( playerAction, PLAYERACTION::MoveLeft )){
				t = map.getTileAt( player->x - 1, player->y );
				if( t != nullptr )
					moveEntity( &map, player, t->x, t->y );	
			
				player->faceDirection( DIRECTION::West );	
			}
			
			if( isAction( playerAction, PLAYERACTION::MoveRight )){
				t = map.getTileAt( player->x + 1, player->y );
				if( t != nullptr )
					moveEntity( &map, player, t->x, t->y );	
			
				player->faceDirection( DIRECTION::East );	
			}
		}
			
		// Inspect
		if( isAction( playerAction, PLAYERACTION::Inspect )){
			player->getLookingAt( x, y );
			t = map.getTileAt( x, y );
			if( t != nullptr ){
				
				// Describe the tile if it's empty. If it has an entity, describe the entity.
				std::string inspect_msg;
				if( t->hasEntity() ){
					
					printf( "There's %s.\n", t->getEntity()->toString().c_str() );
					inspect_msg += "There's " + t->getEntity()->toString() + ".";
				}
				else {
					printf("It's %s.\n", t->toString().c_str() );
					inspect_msg = "It's " + t->toString() + ".";
				}
				
				addQueueMessage( inspect_msg );
			}
		}
			
		// Attack
		if( isAction( playerAction, PLAYERACTION::Attack )){
			 
			player->getLookingAt( x, y );
			t = map.getTileAt( x, y );
			
			// Create a pointer to a valid 'attacker' entity
			Entity* attacker = nullptr;
			
			// designate what 'attacker' should be based on selected inventory item		
			if( inv == nullptr ){
				attacker = player;	// no inventory selected, USE YOUR BARE HANDS!!
			} else {
				attacker = inv; // Use your currently selected item as an 'attacker'
			}
			
			// Perform ATTACK!!
			if( attackTile( &map, attacker, t ) ){
				// perform action here for when player kills/destroys something
				if( showPlayerspace ){
					// This is good when a tree is destroyed and we need to expand the shown play area
					map.showPlayerspace();
				}
			}
		}
		
		// Cycle Inventory
		if( isAction( playerAction, PLAYERACTION::CycleInventory )){
			
			// tell player entity to cycle to next inventory item
			player->nextInventory();
			
			// refresh our pointer to the currently-selected item
			inv = player->getSelectedInventory();
			
			// print to the log what has been selected
			if( inv != nullptr ){
				printf( "Currently selected: %s\n", inv->toString().c_str() );
			} else {
				printf( "No inventory selected.\n" );
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



void drawInventory(){
	
	int invX = 20;
	int invY = 40;
	int invPadding = 4;
	int invWidth = (TILEWIDTH * 14) + (invPadding * 2);
	int invHeight = TILEHEIGHT + (invPadding * 2);
	
	TextBox inventoryText;
	TextBox label;
	
	inventoryText.x = invX + invPadding;
	inventoryText.y = invY + invPadding;
	inventoryText.showForever = true;
	
	label.x = inventoryText.x;
	label.y = inventoryText.y - CHARACTER_HEIGHT - invPadding;
	label.submitMessage( "Inventory:" );
	label.drawMessage( ren );

	// Get a reference to the currently-selected inventory item
	Entity* inv = map.getPlayerGuy()->getSelectedInventory();	// returns nullptr if none selected
	
	SDL_Rect blackBox = {invX, invY, invWidth, invHeight };
	SDL_Rect src;
	SDL_Rect dst = {invX + invWidth - invPadding - TILEWIDTH, invY + invPadding, TILEWIDTH, TILEHEIGHT };
	
	// First, create a black BG rectangle
	SDL_SetRenderDrawColor( ren, 0, 0, 0, 255 );
	SDL_RenderFillRect( ren, &blackBox );
	SDL_SetRenderDrawColor( ren, 255, 255, 255, 255 );
	SDL_RenderDrawRect( ren, &blackBox );
	
	if( inv == nullptr ){	// no inventory selected
		inventoryText.submitMessage( "" );
		
	} else {
		src = { inv->tileset_x, inv->tileset_y, TILEWIDTH, TILEHEIGHT };
		SDL_RenderCopy( ren, map.getTilesetImage(), &src, &dst );
		
		inventoryText.submitMessage( inv->toString() );
	}
	
	inventoryText.drawMessage( ren );
	
	return;
}

void drawHealth(){
	int boxX = 20;
	int boxY = 90;
	int boxPadding = 4;
	int boxWidth = (TILEWIDTH * 14) + (boxPadding * 2);
	int boxHeight = TILEHEIGHT + (boxPadding * 2);
	
	float healthPercentage = (float)map.getPlayerGuy()->health / 100.f;
	healthPercentage *= (boxWidth - (boxPadding * 2));
	
	TextBox label;
	
	label.x = boxX + boxPadding;
	label.y = boxY - CHARACTER_HEIGHT - boxPadding;
	label.submitMessage( "Health:" );
	label.drawMessage( ren );

	SDL_Rect blackBox = { boxX, boxY, boxWidth, boxHeight };
	SDL_Rect healthBox = { boxX + boxPadding, boxY + boxPadding, (int)healthPercentage, boxHeight - (boxPadding * 2) };

	
	// First, draw a black box with a white border
	SDL_SetRenderDrawColor( ren, 0, 0, 0, 255 );
	SDL_RenderFillRect( ren, &blackBox );
	SDL_SetRenderDrawColor( ren, 255, 255, 255, 255 );
	SDL_RenderDrawRect( ren, &blackBox );
	
	// Next, draw a health rect
	SDL_SetRenderDrawColor( ren, 255, 0, 0, 255 );
	SDL_RenderFillRect( ren, &healthBox );
	
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
							SDL_WINDOW_FULLSCREEN_DESKTOP );
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
	
	// hide cursor
	SDL_ShowCursor( SDL_DISABLE );
	
	initText( ren );
	
	return 0;	
}


void cleanup(){
	SDL_ShowCursor( SDL_ENABLE );
	
	shutdownText();
	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );
	destroySound();
	IMG_Quit();
	SDL_Quit();
}

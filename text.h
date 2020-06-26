// TEXT header file

#pragma once

#define MESSAGE_LIFETIME 600
#define CHARACTER_WIDTH 8
#define CHARACTER_HEIGHT 15

#include "main.h"

class TextBox{
	int countdown;
	bool isVisible;
	std::string message;
	
	public:
	TextBox();
	void submitMessage( std::string message );
	void drawMessage( SDL_Renderer* ren );
	void showLastMessage();
	void setMessageFromQueue();
	unsigned int x;
	unsigned int y;
	bool showForever;
};

void initText( SDL_Renderer* ren );
void shutdownText();
void drawCharacter( SDL_Renderer* ren, unsigned int x, unsigned int y, char c );
void addQueueMessage( std::string s );
void clearMessageQueue();
std::string getQueueMessage();


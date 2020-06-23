#include "text.h"

const std::string upperCase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string lowerCase = "abcdefghijklmnopqrstuvwxyz";
const std::string charNumbers = "0123456789";
const std::string charSymbols   = "!?.,$%&()-_+=:;\'\"";

// global variables
SDL_Texture* text_image;
std::string msgQueue;
bool newMessage = false;

///////////////


// TextBox class
//////////////////////////////////
TextBox::TextBox(){
	this->isVisible = false;
	this->message = "";
}

void TextBox::submitMessage( std::string message ){
	this->message = message;
	this->countdown = MESSAGE_LIFETIME;
	this->isVisible = true;
}

void TextBox::drawMessage( SDL_Renderer* ren ){
	unsigned int txtX, txtY;
	
	txtY = WINDOW_HEIGHT - 20;
	txtX = (WINDOW_WIDTH / 2) - (this->message.length() / 2)*CHARACTER_WIDTH;
	
	if( this->isVisible ){
		
		// draw whole message
		for( int i = 0; i < this->message.length(); ++i ){
			drawCharacter( ren, txtX, txtY, this->message.at( i ));
			txtX += CHARACTER_WIDTH;
		}
		
		this->countdown--;
		
		if( this->countdown <= 0 )
			this->isVisible = false;
	}
}

void TextBox::setMessageFromQueue(){
	if( newMessage ){
		this->message = msgQueue;
		this->countdown = MESSAGE_LIFETIME;
		this->isVisible = true;
		newMessage = false;
	}
}


// miscellaneous functions
///////////////////////////////////
void drawCharacter( SDL_Renderer* ren, unsigned int x, unsigned int y, char c ){
	unsigned int i;
	SDL_Rect src;
	SDL_Rect dst;
	bool isSymbol = true;
	
	src.h = CHARACTER_HEIGHT;
	src.w = CHARACTER_WIDTH;
	
	dst.x = x;
	dst.y = y;
	dst.w = CHARACTER_WIDTH;
	dst.h = CHARACTER_HEIGHT;
	
	if( c == ' ' ) return;
	
	// is uppercase letter?
	if( (c > 64) && (c < 91) ){
		src.y = 0;
		
		for( i = 0; i < upperCase.length(); ++i ){
			if( c == upperCase.at( i )){
				src.x = i * CHARACTER_WIDTH;
			}
		}
		
		isSymbol = false;
	}
	
	// is lowercase letter?
	if( (c > 96) && (c < 123) ){
		src.y = CHARACTER_HEIGHT;
		
		for( i = 0; i < lowerCase.length(); ++i ){
			if( c == lowerCase.at( i )){
				src.x = i * CHARACTER_WIDTH;
			}
		}
		
		isSymbol = false;
	}
	
	// is numeral?
	if( (c > 47) && (c < 58) ){
		src.y = CHARACTER_HEIGHT * 2;
		
		for( i = 0; i < charNumbers.length(); ++i ){
			if( c == charNumbers.at( i )){
				src.x = i * CHARACTER_WIDTH;
			}
		}
		
		isSymbol = false;
	}
	
	// is symbol?
	if( isSymbol ){
		src.y = CHARACTER_HEIGHT * 3;
		
		for( i = 0; i < charSymbols.length(); ++i ){
			if( c == charSymbols.at( i )){
				src.x = i * CHARACTER_WIDTH;
			}
		}
	}

	SDL_RenderCopy( ren, text_image, &src, &dst );
	
	return;
}

void initText( SDL_Renderer* ren ){
	text_image = loadTexture( "text.png", ren );
}

void shutdownText(){
	SDL_DestroyTexture( text_image );
}

void addQueueMessage( std::string s ){
	msgQueue = s;
	newMessage = true;
}

void clearMessageQueue(){
	msgQueue = "";
}
















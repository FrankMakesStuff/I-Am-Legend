#pragma once

#if __cplusplus < 201103L
#error "Please compile using the C++11 Standard (-std=c++11)"
#endif

#include <cstdint>
#include <iostream>
#include <windows.h>

const unsigned int CONSOLE_BLACK = 0;
const unsigned int CONSOLE_BLUE = 1;
const unsigned int CONSOLE_GREEN = 2;
const unsigned int CONSOLE_CYAN = 3;
const unsigned int CONSOLE_RED = 4;
const unsigned int CONSOLE_PURPLE = 5;
const unsigned int CONSOLE_YELLOW = 6;
const unsigned int CONSOLE_WHITE = 7;
const unsigned int CONSOLE_BRIGHT = 8;

class vividConsole{
	HANDLE handle;
	unsigned int bg_color;
	unsigned int fg_color;
	unsigned int color;
	
	void calcColor();
	
	public:
	vividConsole();
	//~vividConsole();
	
	void setDefault();
	void print( const char* string );
	void printSuccess ( const char* string );
	void printError( const char* string );
	void printWarning( const char* string );
	void printDestroy( const char* string );
	
	void setBgColor( unsigned int value ){
		if( value < 16 )
			bg_color = value;
			
		calcColor();
	}
	
	void setFgColor( unsigned int value ){
		if( value < 16 )
			fg_color = value;
			
		calcColor();
	}
	
	void setColor( unsigned int value ){
		if( value < 256 )
			color = value;
	}
};

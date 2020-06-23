#include "vividConsole.h"

void vividConsole::print( const char* string ){
	std::cout << string << std::endl;
}


void vividConsole::printSuccess ( const char* string ){
	setBgColor( CONSOLE_BLACK );
	setFgColor( CONSOLE_GREEN | CONSOLE_BRIGHT );
	std::cout << "SUCCESS: ";
	setDefault();
	std::cout << string << std::endl;
}


void vividConsole::printError( const char* string ){
	setBgColor( CONSOLE_BLACK );
	setFgColor( CONSOLE_RED | CONSOLE_BRIGHT );
	std::cout << "ERROR: ";
	setDefault();
	std::cout << string << std::endl;		
}


void vividConsole::printWarning( const char* string ){
	setBgColor( CONSOLE_BLACK );
	setFgColor( CONSOLE_YELLOW );
	std::cout << "WARNING: ";
	setDefault();
	std::cout << string << std::endl;	
}


void vividConsole::printDestroy( const char* string ){
	setBgColor( CONSOLE_BLACK );
	setFgColor( CONSOLE_PURPLE );
	std::cout << "DESTROY: ";
	setDefault();
	std::cout << string << std::endl;	
}


void vividConsole::calcColor() {
	this->color = (this->bg_color * 16) + this->fg_color;
	SetConsoleTextAttribute( this->handle, this->color );
}


void vividConsole::setDefault(){
	setBgColor( CONSOLE_BLACK );
	setFgColor( CONSOLE_WHITE | CONSOLE_BRIGHT );	
}


vividConsole::vividConsole(){
  this->handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

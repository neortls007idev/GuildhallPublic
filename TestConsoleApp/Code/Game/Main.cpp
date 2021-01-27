// #include "Game/UnitTests_Custom.hpp"	// Uncomment this line after adding the custom tests file

//#include "Game/GameCommon.hpp"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string>

#include "MostAwesomeGameplayJob.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem*			g_theInput;
extern JobSystem*			g_theJobSystem;
Window*						g_theWindow						= nullptr;

struct  testStruct1
{
	char a;
	char c;
	float* b;
}abc; 

struct  testStruct2
{
	char a;
	float* b;
	char c;
}abc1;



void SunRise()
{
	printf( "Sunrise has been called.\n" );
}

void SunSet()
{
	printf( "Sunset has been called.\n" );
}

typedef void( *EventCallbackFunctionPtrType )( );
typedef void( *EventCallbackFunctionPtrTypeThatTakesXY )( int x, int y );

// template<class T>
// class selfRef : public <T>
// {
// 	selfRef()
// 	{
// 		printf( "Inheritence successful" );
// 	}
// };

int main( int, char** )
{
	g_theJobSystem = new JobSystem();
	g_theInput = new InputSystem();
		
	printf( "\n\n%zd %zd \n\n" , sizeof( abc ), sizeof( abc1 ) );
	double a;
	int b;
	char c;
	char* d;
	printf( "\n%zd %zd %zd %zd %zd\n\n" , sizeof( a ) , sizeof( b ) , sizeof( c ) , sizeof( d ) , sizeof( Vertex_PCU* ) );

	const char* name = "Amy"; // array of '4' bytes: 'A' 'm' 'y' 0 or '\0';
	std::string a1( "Amy" ); // array of '4' bytes: 'A' 'm' 'y' 0 or '\0'; , copy constructor that takes in a const char*
	std::string a2( "" ); // array of 1 byte : 0
	std::string a3 = "" ; // array of 1 byte : 0
	std::string a4; // array of 1 byte : 0
	std::string a5( name ); // array of '4' bytes: 'A' 'm' 'y' 0 or '\0';
	std::string a6( a4 );
	std::string a7;
	a7 = a6;
	std::string a8 = a6;
	std::string a9("Hello! This is Interesting");
	size_t indexToStarCopyingFrom = 7;
	size_t numberOfcharsToCopy = 4;
	std::string a10( a9 , indexToStarCopyingFrom, numberOfcharsToCopy );
	size_t indexOfExclamationMark = a9.find( '!' );
	size_t indexOfTildeMark = a9.find( '~' ); // returns 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFf demonstrating end of string;
	std::string partBeforeExclmation;

	partBeforeExclmation = std::string( a9 , 0 , indexOfExclamationMark );

	//OR

	if ( indexOfExclamationMark != std::string::npos )
	{
		partBeforeExclmation = std::string( a9 , 0 , indexOfExclamationMark );
	}
	else
	{
		partBeforeExclmation = a9;
	}
	
	if ( indexOfTildeMark == std::string::npos )
	{
		
	}
	//getch();

	EventCallbackFunctionPtrType mySunrise = SunRise;


	mySunrise();
	mySunrise();

	mySunrise = SunSet;

	mySunrise();
	mySunrise();

	mySunrise = nullptr;
	
	if ( mySunrise!= nullptr )
	{
		mySunrise();
	}

// 	class jkdn{};
// 	selfRef:<jkdn> obj;

	bool isRunning = true;
	
	while( isRunning )
	{
		
		if( _getch() == 'p' )
		{
			for ( int index = 0 ; index < 1000 ; index++ )
			{
				MostAwesomeGameplayJob* job = new MostAwesomeGameplayJob( index );
				g_theJobSystem->PostJob( *job );
			}
		}
		if( _getch() == 'q' )
		{
			g_theJobSystem->HandleQuitRequested();
			isRunning = false;
		}
	}
	return 0;
}



#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/TheApp.hpp"
#include "GameCommon.hpp"
#include <cassert>
#include <crtdbg.h>
#include <math.h>

//-----------------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//-----------------------------------------------------------------------------------------------

extern TheApp*		g_theApp;
extern InputSystem* g_theInput;

	   Window*		g_theWindow = nullptr;


//-----------------------------------------------------------------------------------------------

const	char* APP_NAME = "SD1 : A4 PROTOGAME 2D ( C29 : Rahul Gupta )";

//-----------------------------------------------------------------------------------------------

int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( applicationInstanceHandle );
	UNUSED( commandLineString );

	g_theApp = new TheApp();

	g_theWindow = new Window();
	g_theWindow->Open( APP_NAME , CLIENT_ASPECT , 0.9f );

	g_theApp->Startup();

	while ( !g_theWindow->IsQuitting() )
	{
		g_theApp->RunFrame();
	}

	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	g_theWindow->Close();
	delete g_theWindow;
	g_theWindow = nullptr;

	return 0;
}



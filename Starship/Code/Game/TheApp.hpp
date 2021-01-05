#pragma once
#include "Game/Game.hpp"
#include "Game/AttractMode.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class AttractMode;

//--------------------------------------------------------------------------------------------------------------------------------------------

class TheApp
{

public:
	TheApp();
	~TheApp();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const													{ return m_isQuitting; }
	bool HandleKeyReleased();
	bool HandleKeyPressed();
	bool HandleQuitRequested();

	void BeginFrame();
	void Update( float deltaSeconds );
	void Render() const;

	void EndFrame();
	bool IsKeyPressed(unsigned char keyCode);
		
private :
	bool			m_isQuitting				= false ;
	AttractMode*	m_theAttractMode			= nullptr;
	
public:
	bool			m_attractModeStatus			= false;
	bool			m_isPaused					= false;
	bool			m_isSloMo					= false;
	Camera*			g_theCamera					= new Camera();
	
	Game*			m_theGame					= nullptr;
	bool			m_keyPressed[256]			= { false };
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------
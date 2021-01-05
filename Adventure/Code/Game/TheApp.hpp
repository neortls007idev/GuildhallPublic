#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class TheApp
{

public:

	TheApp();
	~TheApp();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const														{ return m_isQuitting; }
	bool HandleQuitRequested();

	void BeginFrame();
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void Render() const;

	void EndFrame();
	

//--------------------------------------------------------------------------------------------------------------------------------------------

private :

	bool m_isQuitting  = false ;
	bool m_isSloMo = false;
	bool m_isSpeedMo = false;

public:

	bool m_isPaused = false;
	bool m_debugCamera = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

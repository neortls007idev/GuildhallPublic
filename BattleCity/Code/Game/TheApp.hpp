#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

class PlayerClient;
class RemoteClient;

//--------------------------------------------------------------------------------------------------------------------------------------------

class TheApp
{

public:

	TheApp();
	~TheApp();
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const																							{ return m_isQuitting; }
	bool HandleQuitRequested();

	void BeginFrame();
	void Update( float deltaSeconds );
	void UpdateFromKeyboard();
	void Render() const;

	void EndFrame();

	void AddMultiplayerGameCommandsToConsole();
	
	static bool StartMultiplayerServer( EventArgs& args );
	static bool ConnectToMultiplayerServer( EventArgs& args );
	bool StartMultiplayerServerCheck();
	bool ConnectToMultiplayerServerCheck();

	
//--------------------------------------------------------------------------------------------------------------------------------------------

private :

	bool					m_isQuitting			= false ;
	bool					m_hasServerJustStarted;
	EventArgs				m_serverArgs;
	bool					m_hasClientJustConnected;
	EventArgs				m_clientArgs;
public:
	PlayerClient*			m_localclient			= nullptr;
	RemoteClient*			m_distantClient			= nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

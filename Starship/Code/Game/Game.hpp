#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Entites/Enemies/bettle.hpp"
#include "Entites/Enemies/Wasp.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Entites/Debris.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

private:
	float			m_screenShakeIntensity					= 0.f;
	bool			m_bulletButtonPress						= false;
	int				m_controllerID							= -1;

public:
	PlayerShip*		m_Ship;
	Bullet*			m_Bullets[ MAX_BULLETS ]				= { nullptr };
	Asteroid*		m_Asteroids[ MAX_ASTEROIDS ]			= { nullptr };
	Bettle*			m_Bettles[ MAX_BETTLES ]				= { nullptr };
	Wasp*			m_Wasps[ MAX_WASPS ]					= { nullptr };
	Debris*			m_Debris[ MAX_DEBRIS ]					= { nullptr };

	int				m_currentlyAliveBullets					= 0;

	int				m_waveNumber							= 0;
	int				m_totalEnemies							= 0;
	int				m_AsteroidSpawnCount					= 0;
	int				m_BettleSpawnCount						= 0;
	int				m_WaspSpawnCount						= 0;

	bool			m_devMode								= false;
	Camera*			m_uiCamera								= new Camera();

public:
	Game();
	~Game();

	void		Levels();

	void		Update( float deltaSeconds );

	void		KeyboardKeyPressEvents();

	void		Render() const;

	void		ShipDebugDrawing() const;
	void		BulletDebugDrawing( int BulletPointerArrayIndex ) const;
	void		AsteroidDebugDrawing( int AsteroidPointerArrayIndex ) const;

	void		RenderUI() const;

	void		UpdateCamera();
	void		AddScreenShakeIntensity( float deltaShakeIntensity );

	void		SpawnPlayerShip();
	Vec2		GetPlayerPosition();
	float		GetPlayerOrientation();
	void		ReSpawn();

	int			SpawnBullet( Vec2 pos , float orientation );
	void		MaxBulletWarning();
	void		UpdateBullet( float deltaSeconds );

	void		SpawnInitialEnemies();
	int			SpawnNewAsteroidAtRandomPosition( int newAsteroidNumber );
	void		UpdateAsteroid( float deltaSeconds );
	void		MaxAsteroidWarning();

	int			SpawnNewBettleAtRandomPosition( int newBettleNumber );
	void		UpdateBettles( float deltaSeconds );

	int			SpawnNewWaspAtRandomPosition( int newWaspNumber );
	void		UpdateWasps( float deltaSeconds );

	void		SpawnDebrisField( const Vec2& center , int howmany , const Rgba8& color );
	void		UpdateDebris( float deltaSeconds );

	void		DetectCollissionBetweenAstroidAndShip();
	void		DetectCollissionBetweenAstroidsAndBullets();
	void		DetectCollissionBetweenBettleAndShip();
	void		DetectCollisionBetweenBettleAndBullet();
	void		DetectCollissionBetweenWaspAndShip();
	void		DetectCollisionBetweenWaspAndBullet();

	void		WaitAndRestart( float deltaSecond );

	bool		OutOfScreenBounds( const Entity* entity );

	void		GarbageCollection();
	void		GarbageDeletion();

	void		Die();
};

//--------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Shader;
class Texture;
class GPUMesh;
class ParticleEmitter3D;
class GameParticleEmitterSpawnJob;

typedef std::vector< Transform* > OBJInstances;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{
public:
			Game();
				void LoadShaders();
				void LoadTextures();
				void LoadPlanetaryTextures();
				void LoadModels();
				void LoadScene();
				void InitializeCameras();
				void IntializeGameObjects();
				void InitializeLightData();
				void InitializeShadowMapTextures();
				void InitializeShaderMaterialData();
				void InitializeParticleEmitters();
				void InitializeShadowTestTrasforms();
				void TestCaseDirectionalShadows();
				void TestCaseSpotShadows();
				void TestCaseSpotShadows2();
				void TestCaseDirectionalAndSpotShadows();
			~Game();

			void Update( float deltaSeconds );
				void UpdateViewFrustumCulling();
				void UpdateLightViewFrustumCulling();
				void ThreadedUpdateViewFrustumCulling();
				void ThreadedUpdateLightViewFrustumCulling();
				void UpdateLightPosition( float deltaSeconds );

				void UpdateAllStarEmitters();
				void ThreadedUpdateAllStarEmitters();
				void UpdateEmitterOfType( GameStarEmitters emitterType );
			
			void Render() const;
				void BindShaderSpecificMaterialData() const;
				void RenderFresnelShader2ndPass() const;
				void RenderShadowMapPass() const;
					void ClearShadowMapsForNonShadowCastingLights() const;
					void RenderOrthoShadowMapPass() const;
					void RenderSpotShadowMapPass() const;

private:
			void DebugUI();
				void UIControlsGUI();
					void ShadersGUI();
					void LightSettingsGUI();
						void SetLightPositionToOrigin( int lightIndex );
						void SetLightPositionToCameraPos( int lightIndex );
						void SetLightDirectionToCameraForwardVector( int lightIndex );
						void UpdateLightData( int lightIndex );
						void UpdateSpotLightConeData( int lightIndex );
					void EffectsSettingsUI();
						void UpdateToneMapData();
					void SceneSetupGUI();
					void ProfilingSettingsGUI();
					void ParticleEmitterSettingsGUI();
				void PermanentDebugDataGUI();
				void PlotFrameRate();
				void SpecialControlsDisplayGUI();

			//----------------------------------------------------------------------------------------------------------

			bool AddGameOBJInstance( eGameObjModels modelType );
			bool AddGameOBJInstances( eGameObjModels modelType );
			bool DestroyGameOBJInstance( eGameObjModels modelType , int instanceCount );
			void RenderAllInstancesOfType( eGameObjModels modelType ) const;
			void RenderAllModelInstances() const;
//			void RenderAllModelInstancesUsingInstanceArray( OBJInstances& instanceArray ) const;
			void RenderAllModelInstancesOfTypeUsingInstanceArray( const OBJInstances& instanceArray , eGameObjModels modelType ) const;
			void SaveScene();

//--------------------------------------------------------------------------------------------------------------------------------------------
//				METHODS TO HANDLE USER INPUT
//--------------------------------------------------------------------------------------------------------------------------------------------
	
			void UpdateFromKeyBoard( float deltaSeconds );
			void UpdateLightsFromKeyBoard( float deltaSeconds );
			void UpdateLightPositionOnUserInput();
			void CameraPositionUpdateOnInput( float deltaSeconds );
	
public:

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				GAME CAMERA
	//--------------------------------------------------------------------------------------------------------------------------------------------
	mutable Camera						m_gameCamera;
	
	Vec3								m_cameraPosition									= Vec3::ZERO;
	float								m_pitch												= 0.f;
	float								m_yaw												= 0.f;

	float								m_nearZ												= GAME_CAM_NEAR_Z;
	float								m_farZ												= GAME_CAM_FAR_Z;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
			
	Shader*								m_lightShaders[ LitShaderTypes::TOTAL_LITSHADERS ];
	Shader* 							m_currentShader;
	int									m_currentShaderIndex;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				LIGHTS DATA
	//--------------------------------------------------------------------------------------------------------------------------------------------	
	shaderLightDataT					m_lights;
	Vec2								m_spotlightConeAngles[ TOTAL_LIGHTS ];
	Rgba8								m_ambientLightColor;
	mutable Camera						m_lightsOrtho3DCamera;
	mutable Camera						m_lightsProjectionCamera;
//	int									m_lightType[ TOTAL_LIGHTS ];
	bool								m_isLightFollowingTheCamera							= false;
	bool								m_isLightAnimated									= false;
	int									m_currentLightIndex									= 0;
	mutable Texture*					m_shadowMap[ TOTAL_LIGHTS ];
	Vec3								m_lightsPitchYawRoll[ TOTAL_LIGHTS ];

	//--------------------------------------------------------------------------------------------------------------------------------------------
			
	Sampler*							m_bilinear											= nullptr;
	Sampler*							m_linear											= nullptr;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				SCENE OBJECTS
	//--------------------------------------------------------------------------------------------------------------------------------------------
	uint								m_hCuts = 32;		// slices
	uint								m_vCuts = 16;		// stacks

	// this is shared by all maps.
	GPUMesh*							m_gameModels[ NUM_GAME_MODELS ];
	Texture*							m_gameModelsDiffuse[ NUM_GAME_MODELS ];
	Texture*							m_gameModelsNormals[ NUM_GAME_MODELS ];
	Texture*							m_gamePlanetaryDiffuse[ NUM_GAME_MODELS ];
	Texture*							m_gamePlanetaryNormals[ NUM_GAME_MODELS ];
	// this is map unique 

	OBJInstances						m_ModelInstances[ NUM_GAME_MODELS ];
	std::vector<bool>					m_isInstanceNotGarbage[ NUM_GAME_MODELS ];
	OBJInstances						m_ModelDrawableInstances[ NUM_GAME_MODELS ];
	OBJInstances						m_ModelLightDrawableInstances[ TOTAL_LIGHTS ][ NUM_GAME_MODELS ];

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				SKY-BOX
	//--------------------------------------------------------------------------------------------------------------------------------------------
	GPUMesh*							m_unitCubeMesh										= nullptr;
	// cubemap tex in the map class.
	Texture*							m_cubeMapex											= nullptr;
	Shader*								m_cubeMapTest										= nullptr;
	Sampler*							m_cubeSampler										= nullptr;
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
		
	bool								m_debugSwitchs[ NUM_GAME_DEBUG_SWITCHS ];
	//--------------------------------------------------------------------------------------------------------------------------------------------
		
private:
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				MATERIAL SHADER DATA
	//--------------------------------------------------------------------------------------------------------------------------------------------
	Shader* 							m_depthShader;
	Shader* 							m_blurShader;
	Shader* 							m_toneMapShader;
	Shader* 							m_toneMapComputeShader;
	
	Mat44								m_toneMapTransform;
	float								m_tonePower											= 1.f;
	fresnelData_t						m_fresnelShaderData;
	dissolveData_t						m_dissolveShaderData;
	fogDataT							m_fogData;
	bool								m_isFresnelShaderActive								= false;
	bool								m_isblurShaderActive								= false;
	bool								m_isToneMapShaderActive								= false;
	bool								m_isToneMapComputeShaderActive						= false;
	//bool								m_isFogShaderActive									= false;

	Texture*							m_dissolveShaderPatternTexture						= nullptr;
	Texture*							m_triplanarShaderTextures[ 6 ];
	int									m_currentToneMap									= ToneMap::SEPHIA;
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				FRAME RATE DEBUG DATA
	//--------------------------------------------------------------------------------------------------------------------------------------------
	float								m_frameRate											= 0.f;
	float								m_rollingAvgFPS										= 0.f;
	float								m_worstFrame										= INFINITY;
	float								m_bestFrame											= 0.f;
	float								m_currentFrame										= 0;
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				UI DEBUG DATA
	//--------------------------------------------------------------------------------------------------------------------------------------------		
	bool								m_isMouseUnlocked									= false;
	bool								m_dirtyUBOs[ NUM_DIRTY_UBOS ];
	uint								m_totalDrawableMeshes;
	uint								m_currentlyDrawingMeshes;
	uint								m_currentlyDrawingShadowMeshes[ TOTAL_LIGHTS ];
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				PARTICLE EMITTERS
	//--------------------------------------------------------------------------------------------------------------------------------------------
	GameParticleEmitter					m_starEmitters[ NUM_STARS_EMITTERS ];
	SpriteSheet* m_particleEmitterSheets[ NUM_GAME_SS ];
	
	//----------------------------------------------------------------------------------------------------------
	//				SHADOW
	//----------------------------------------------------------------------------------------------------------
	float								m_shadowCamHeight						= 2.5f;
	int									m_shadowMapDimension					= 1024;
	int									m_shadowMapDimensionCopy				= 1024;

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				FRAME DATA
	//--------------------------------------------------------------------------------------------------------------------------------------------
	float								m_frameTimes[ FRAME_RATE_BUFFER_SIZE ]	= { 0.f };
	float								m_frameRates[ FRAME_RATE_BUFFER_SIZE ]	= { 0.f };
	size_t								m_currentFrameInBuffer					= 0;

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				TEST / DEBUG CODE
	//--------------------------------------------------------------------------------------------------------------------------------------------

	Transform							m_shadowTestCubes[ 8 ];

	GPUMesh*							m_cubeMesh;
	GPUMesh*							m_meshSphere;
	GPUMesh*							m_quadMesh;
	GPUMesh*							m_objSciFiShipMesh;
	Transform							m_cubeMeshTransform;
	Transform							m_sphereMeshTransform;
	Transform							m_quadTransform;
	Transform							m_objSciFiShipTransform;
	Texture*							m_meshTex_D											= nullptr;
	Texture*							m_meshTex_N											= nullptr;
	Texture*							m_tileDiffuse										= nullptr;
	Texture*							m_tileNormal										= nullptr;
	Texture*							m_objSciFiShipMeshTex_D								= nullptr;
	Texture*							m_objSciFiShipMeshTex_N								= nullptr;

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//		JOBS
	//--------------------------------------------------------------------------------------------------------------------------------------------
	GameParticleEmitterSpawnJob*					m_particleSpawnJob									= nullptr;

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//				GUI TEST CODE
	//--------------------------------------------------------------------------------------------------------------------------------------------
	int									m_numNewInstancesCount	= 0;
	float								m_numNewInstancesRadius = 0.f;
	float								m_minPitch				= 0.f;
	float								m_minYaw				= 0.f;
	float								m_minRoll				= 0.f;
	float								m_maxPitch				= 0.f;
	float								m_maxYaw				= 0.f;
	float								m_maxRoll				= 0.f;
		
};
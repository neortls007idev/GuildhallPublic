#include "Engine/Core/DebugRender.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ParticleSystem/ParticleEmitter3D.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Game/Game.hpp"
#include "ThirdParty/ImGUI/imgui.h"
#include "ThirdParty/ImGUI/ImGuiFileDialog.h"
#include "ThirdParty/ImGUI/implot.h"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

static ImGuiComboFlags flags = 0;
static int currentModelToAddIndex = 0;
static int currentModelToUpdateIndex = 0;

//--------------------------------------------------------------------------------------------------------------------------------------------

const char* modelNames[ NUM_GAME_MODELS ] =
{
	"SPACESHIP" ,
	"LUMINARIS SHIP" ,
//	"W-CRUISER SHIP" ,
//	"MINI SHIP",
//	"STAR SHIP" ,
//	"STRIDER"
};

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugUI()
{
	ImGui::NewFrame();
	UIControlsGUI();
	PermanentDebugDataGUI();
	SpecialControlsDisplayGUI();
	PlotFrameRate();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UIControlsGUI()
{
	if( ImGui::Begin( "UI Controls" ) )
	{
		ShadersGUI();
		LightSettingsGUI();
		EffectsSettingsUI();
		SceneSetupGUI();
		ProfilingSettingsGUI();
		ParticleEmitterSettingsGUI();
	}
	ImGui::End();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ShadersGUI()
{
	if( ImGui::CollapsingHeader( "Shaders" ) )
	{
		if( ImGui::TreeNode( "Currently Active Shader" ) )
		{
			ImGui::RadioButton( "Lighting and Shadow"		, &m_currentShaderIndex , LIT_SHADOW );
			ImGui::RadioButton( "Lighting"					, &m_currentShaderIndex , LIT );
			ImGui::RadioButton( "UnLit"					, &m_currentShaderIndex , UNLIT );
			ImGui::RadioButton( "UV Debugger"				, &m_currentShaderIndex , UV );
			ImGui::RadioButton( "Normals Debugger"			, &m_currentShaderIndex , NORMAL );
			ImGui::RadioButton( "Tangents Debugger"		, &m_currentShaderIndex , TANGENT );
			ImGui::RadioButton( "BiTangents Debugger"		, &m_currentShaderIndex , BITANGENT );
			ImGui::RadioButton( "Surface Normal Debugger"	, &m_currentShaderIndex , SURFACE_NORMAL );
			ImGui::RadioButton( "Dissolve Shader"			, &m_currentShaderIndex , DISSOLVE );
			ImGui::RadioButton( "Triplanar UnLit Shader"	, &m_currentShaderIndex , TRIPLANAR_UNLIT );
			ImGui::RadioButton( "Triplanar Lit Shader"		, &m_currentShaderIndex , TRIPLANAR_LIT );

			m_currentShader = m_lightShaders[ m_currentShaderIndex ];
			ImGui::TreePop();
			ImGui::Separator();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LightSettingsGUI()
{
	if( ImGui::CollapsingHeader( "Lights" ) )
	{
		if( ImGui::CollapsingHeader( "GlobalSettings" ) )
		{
			ImGui::ColorEdit3( "Ambient Light Color"		, ( float* ) &m_lights.ambientLight );
			ImGui::DragFloat( "Ambient Light Intensity"	, &m_lights.ambientLight.w , 0.01f , 0.f , 1.f );
			ImGui::DragFloat( "Global Specular Power"	, &m_lights.SPECULAR_POWER , 2.f , 1.f , 10000.f );
			ImGui::DragFloat( "Global Specular Factor"	, &m_lights.SPECULAR_FACTOR , 0.01f , 0.f , 1.f );
			ImGui::Separator();
		}

		if( ImGui::TreeNode( "Currently Actively selected light( For additional movement options )" ) )
		{
			for( int index = 0; index < TOTAL_LIGHTS - 1 ; index++ )
			{
				ImGui::RadioButton( ToString( index + 1 ).c_str() , &m_currentLightIndex , index ); ImGui::SameLine();
			}
			ImGui::RadioButton( ToString( ( int ) TOTAL_LIGHTS ).c_str() , &m_currentLightIndex , TOTAL_LIGHTS - 1 );

			ImGui::TreePop();
			ImGui::Separator();
		}
		
		for( uint lightIndex = 0; lightIndex < TOTAL_LIGHTS ; lightIndex++ )
		{
			std::string lightName = "Light ";
			lightName.append( ToString( ( int ) lightIndex + 1 ) );
			if( ImGui::TreeNode( lightName.c_str() ) )
			{
				bool lightStatus = true;
				if( m_lights.lights[ lightIndex ].intensity >= 0.001f && m_lights.lights[ lightIndex ].intensity <= 0.001f )
				{
					lightStatus = false;
				}

				ImGui::Checkbox( "isEnabled" , &lightStatus );

				//ImGui::ColorEdit3( "Light position" , ( float* ) &uiTestColor );
				if( ImGui::TreeNode( "Movement" ) )
				{
					ImGui::InputFloat3( "Light World Position" , ( float* ) &m_lights.lights[ lightIndex ].worldPosition );
					if( ImGui::Button( "Set Position to Origin" ) )
					{
						SetLightPositionToOrigin( lightIndex );
					}

					if ( ImGui::Button("Set Position to Camera Position") )
					{
						SetLightPositionToCameraPos( lightIndex );
					}
					
					//ImGui::RadioButton( "Follows the Camera" , 1 );
					//ImGui::RadioButton( "Moves in a fixed path" , 0 );

					ImGui::TreePop();
					ImGui::Separator();
				}

				if( ImGui::TreeNode( "Light Type" ) )
				{
					ImGui::RadioButton( "Point Light"		 , ( int* ) &m_lights.lights[ lightIndex ].lightType , POINT_LIGHT );
					ImGui::RadioButton( "Spot Light"		 , ( int* ) &m_lights.lights[ lightIndex ].lightType , SPOT_LIGHT );
					ImGui::RadioButton( "Directional Light" , ( int* ) &m_lights.lights[ lightIndex ].lightType , DIRECTIONAL_LIGHT );
					
					ImGui::Text( "Direction Factor = %.3f"	 , m_lights.lights[ lightIndex ].directionFactor );
					ImGui::InputFloat3( "Light Direction"	 , ( float* ) &m_lights.lights[ lightIndex ].direction );
					//ImGui::SliderAngle( "Dot Inner Angle" , &m_lights.lights[ lightIndex ].dotInnerAngle , -360.f , 360.f );
					//ImGui::SliderAngle( "Dot Outer Angle" , &m_lights.lights[ lightIndex ].dotOuterAngle , -360.f , 360.f );
					if( ImGui::Button( "Set Direction to Camera Forward Vector" ) )
					{
						SetLightDirectionToCameraForwardVector( lightIndex );
					}
					
					UpdateLightData( lightIndex );
					ImGui::TreePop();
					ImGui::Separator();
				}

				ImGui::ColorEdit3( "Light Color" , ( float* ) &m_lights.lights[ lightIndex ].color );
				ImGui::DragFloat( "Light Intensity" , &m_lights.lights[ lightIndex ].intensity , 0.01f , 0.f , 1.f );

				if( ImGui::TreeNode( "Attenuation" ) )
				{
					ImGui::InputFloat3( "Diffuse Attenuation Factor"  , ( float* ) &m_lights.lights[ lightIndex ].attenuation );
					ImGui::InputFloat3( "Specular Attenuation Factor" , ( float* ) &m_lights.lights[ lightIndex ].specularAttenuation );

					ImGui::TreePop();
					ImGui::Separator();
				}

				if( m_lights.lights[ lightIndex ].lightType == 2 )
				{
					if ( ImGui::TreeNode( "Spot Light Cone Angles" ) )
					{
						ImGui::SliderFloat( "Outer Cone Angle" , ( float* ) &m_spotlightConeAngles[ lightIndex ].y , 0.f , 120.f );
						ImGui::SliderFloat( "Inner Cone Angle" , ( float* ) &m_spotlightConeAngles[ lightIndex ].x , 0.f , m_spotlightConeAngles[ lightIndex ].y );

						m_spotlightConeAngles[ lightIndex ].x = Clamp( m_spotlightConeAngles[ lightIndex ].x , 0.f , m_spotlightConeAngles[ lightIndex ].y );

						UpdateSpotLightConeData( lightIndex );

						ImGui::TreePop();
						ImGui::Separator();
					}
				}

				if ( m_lights.lights[ lightIndex ].lightType != 0 )
				{
					if ( ImGui::TreeNode( "Shadow Parameters" ) )
					{
						ImGui::DragFloat( "Enable Shadow" , &m_lights.lights[ lightIndex ].shadowFlag , 1.f , 0.f , 1.f );
						ImGui::DragFloat( "Shadow Bias" , &m_lights.lights[ lightIndex ].shadowBias , 0.0001f , 0.f , 1.f );
						
						ImGui::TreePop();
						ImGui::Separator();
					}
				}

				ImGui::TreePop();
				ImGui::Separator();
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::EffectsSettingsUI()
{
	if( ImGui::CollapsingHeader( "Effects" ) )
	{
		ImGui::Checkbox( "Tone Map" , &m_isToneMapShaderActive );
		ImGui::Checkbox( "Use Compute Shader" , &m_isToneMapComputeShaderActive );
		ImGui::Text( "Current Tone Map Shader" );
		ImGui::RadioButton( "No Tone" , &m_currentToneMap , NO_TONE ); ImGui::SameLine();
		ImGui::RadioButton( "GrayScale" , &m_currentToneMap , GRAYSCALE ); ImGui::SameLine();
		ImGui::RadioButton( "Sephia" , &m_currentToneMap , SEPHIA );
		ImGui::SliderFloat( "Tone Map Strength" , &m_toneMapTransform.Tw , 0.f , 1.f );
		ImGui::SliderFloat( "Tone Map Power" , &m_tonePower , 0.f , 200.f );

		if( m_isToneMapShaderActive )
		{
			UpdateToneMapData();
		}

		ImGui::Separator();

		ImGui::Checkbox( "Bloom Blur" , &m_isblurShaderActive );

		ImGui::Separator();

		if( ImGui::TreeNode( "Fog Data" ) )
		{
			ImGui::Checkbox( "Is Fog Active" , &m_isblurShaderActive );
			ImGui::SliderFloat( "Near Distance" , &m_fogData.nearFog , -100.f , 100.f );
			ImGui::SliderFloat( "Far Distance" , &m_fogData.farFog , -100.f , 100.f );
			ImGui::ColorEdit3( "Near Color" , ( float* ) &m_fogData.nearFogColor );
			ImGui::ColorEdit3( "Far Color" , ( float* ) &m_fogData.farFogColor );

			ImGui::TreePop();
			ImGui::Separator();
		}

		if( ImGui::TreeNode( "Fresnel Shader Data" ) )
		{
			ImGui::Checkbox( "Is Active" , &m_isFresnelShaderActive );
			ImGui::SliderFloat( "Fresnel Power" , &m_fresnelShaderData.fresnelPower , 0.f , 1.f );
			ImGui::SliderFloat( "Fresnel Factor" , &m_fresnelShaderData.fresnelfactor , 0.f , 1.f );
			ImGui::ColorEdit3( "Fresnel Color" , ( float* ) &m_fresnelShaderData.fresnelColor );

			ImGui::TreePop();
			ImGui::Separator();
		}

		if( ImGui::TreeNode( "Dissolve Shader Data" ) )
		{
			ImGui::SliderFloat( "Burn Value" , &m_dissolveShaderData.burnValue , 0.f , 1.f );
			ImGui::SliderFloat( "Burn Edge Width" , &m_dissolveShaderData.burnEdgeWidth , 0.f , 1 + 2 * m_dissolveShaderData.burnValue );
			ImGui::ColorEdit3( "Start Color" , ( float* ) &m_dissolveShaderData.startColor );
			ImGui::ColorEdit3( "End Color" , ( float* ) &m_dissolveShaderData.endColor );

			ImGui::TreePop();

			if( ImGui::Button( "Open File Dialog For Setting Dissolve Pattern" ) )
			{
				igfd::ImGuiFileDialog::Instance()->OpenDialog( "Select Dissolve Pattern" , "Choose File" , ".png,.jpg,.jpeg" , "." );
			}

			// display
			if( igfd::ImGuiFileDialog::Instance()->FileDialog( "Select Dissolve Pattern" ) )
			{
			  // action if OK
				if( igfd::ImGuiFileDialog::Instance()->IsOk == true )
				{
					std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
					std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
					m_dissolveShaderPatternTexture = g_theRenderer->GetOrCreateTextureFromFile( filePathName.c_str() );
					// action
					ImGui::Separator();
				}
				// close
				igfd::ImGuiFileDialog::Instance()->CloseDialog( "Select Dissolve Pattern" );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SceneSetupGUI()
{
	if( ImGui::CollapsingHeader( "Scene Setup" ) )
	{
		if( ImGui::BeginCombo( "OBJs" , modelNames[ currentModelToAddIndex ] , flags ) )
		{
			for( int modelIndex = 0; modelIndex < NUM_GAME_MODELS; modelIndex++ )
			{
				const bool is_selected = ( currentModelToAddIndex == modelIndex );
				if( ImGui::Selectable( modelNames[ modelIndex ] , is_selected ) )
				{
					currentModelToAddIndex = modelIndex;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if( is_selected )
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		std::string instanceButton = "Add instance of" + std::string( modelNames[ currentModelToAddIndex ] );

		if( ImGui::Button( instanceButton.c_str() ) )
		{
			AddGameOBJInstance( ( eGameObjModels ) currentModelToAddIndex );
		}
		if ( ImGui::CollapsingHeader( "Instances Spawn settings" ) )
		{
			ImGui::InputInt( "New Instances Count" , &m_numNewInstancesCount );
			ImGui::InputFloat( "new Instances Radius" , &m_numNewInstancesRadius );
			ImGui::InputFloat( "min Yaw"	, &m_minYaw  );		//ImGui::SameLine();
			ImGui::InputFloat( "min Pitch" , &m_minPitch );	//ImGui::SameLine();
			ImGui::InputFloat( "min Roll"	, &m_minRoll );
			ImGui::InputFloat( "max Yaw"	, &m_maxYaw );		//ImGui::SameLine();
			ImGui::InputFloat( "max Pitch" , &m_maxPitch );	//ImGui::SameLine();
			ImGui::InputFloat( "max Roll"	, &m_maxRoll );
		}

		std::string instancesButton = "Add " + ToString( m_numNewInstancesCount ) + " instances of" + std::string( modelNames[ currentModelToAddIndex ] );
		if ( ImGui::Button( instancesButton.c_str() ) )
		{
			AddGameOBJInstances( ( eGameObjModels ) currentModelToAddIndex );
 		}
		
		if( ImGui::BeginCombo( "OBJs to Update" , modelNames[ currentModelToUpdateIndex ] , flags ) )
		{
			for( int modelIndex = 0; modelIndex < NUM_GAME_MODELS; modelIndex++ )
			{
				const bool is_selected = ( currentModelToUpdateIndex == modelIndex );
				if( ImGui::Selectable( modelNames[ modelIndex ] , is_selected ) )
				{
					currentModelToUpdateIndex = modelIndex;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if( is_selected )
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		std::string instancesHeader = "All instance of" + std::string( modelNames[ currentModelToUpdateIndex ] );
		if( ImGui::CollapsingHeader( instancesHeader.c_str() ) )
		{
			for( int instanceIndex = 0 ; instanceIndex < ( int ) m_ModelInstances[ currentModelToUpdateIndex ].size() ; instanceIndex++ )
			{
				if( nullptr != m_ModelInstances[ currentModelToUpdateIndex ][ instanceIndex ] )
				{
					std::string instanceName = modelNames[ currentModelToUpdateIndex ] + std::string( " " ) + ToString( instanceIndex + 1 );

					if( ImGui::TreeNode( instanceName.c_str() ) )
					{
						ImGui::Text( "Transform" );
						ImGui::DragFloat3( "World Position" , ( float* ) &m_ModelInstances[ currentModelToUpdateIndex ][ instanceIndex ]->m_position );
						ImGui::DragFloat( "Yaw" , ( float* ) &m_ModelInstances[ currentModelToUpdateIndex ][ instanceIndex ]->m_yaw );		//ImGui::SameLine();
						ImGui::DragFloat( "Pitch" , ( float* ) &m_ModelInstances[ currentModelToUpdateIndex ][ instanceIndex ]->m_pitch );	//ImGui::SameLine();
						ImGui::DragFloat( "Roll" , ( float* ) &m_ModelInstances[ currentModelToUpdateIndex ][ instanceIndex ]->m_roll );
						ImGui::DragFloat3( "Scale" , ( float* ) &m_ModelInstances[ currentModelToUpdateIndex ][ instanceIndex ]->m_scale );
						DebugAddWorldWireSphere( m_ModelInstances[ SPACESHIP ][ instanceIndex ]->m_position ,
							m_gameModels[ SPACESHIP ]->m_boundingSphereRadius , GREEN , 0.01667f );
						if( ImGui::Button( "Destroy Instance" ) )
						{
							DestroyGameOBJInstance( ( eGameObjModels ) currentModelToUpdateIndex , instanceIndex );
						}

						ImGui::TreePop();
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ProfilingSettingsGUI()
{
	if( ImGui::CollapsingHeader( "Profiling Debugging" ) )
	{
		ImGui::Checkbox( "Camera View Frustum Culling" , &m_debugSwitchs[ GAME_CAMERA_VIEW_FRUSTUM_CULLING ] );

		for ( uint lightIndex = 0; lightIndex < TOTAL_LIGHTS; lightIndex++ )
		{
			std::string lightName = "Light ";
			lightName.append( ToString( ( int ) lightIndex + 1 ) );

			if( m_debugSwitchs[ THREADED_VIEW_FRUSTUM_CULLING ] && m_debugSwitchs[ GAME_CAMERA_VIEW_FRUSTUM_CULLING ] )
			{
				size_t numCulledModelsCount = 0;
				for( int modelIndex = 0 ; modelIndex < NUM_GAME_MODELS ; modelIndex++ )
				{
					numCulledModelsCount += m_ModelLightDrawableInstances[ lightIndex ][ modelIndex ].size();
				}
				
				lightName += " View Frustum Culling Count = " + ToString( ( int ) numCulledModelsCount );
				ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , lightName.c_str() );
			}
			else if ( !m_debugSwitchs[ THREADED_VIEW_FRUSTUM_CULLING ] && m_debugSwitchs[ GAME_CAMERA_VIEW_FRUSTUM_CULLING ] )
			{
				lightName += " View Frustum Culling Count = " + ToString( ( int ) m_currentlyDrawingShadowMeshes[ lightIndex ] );
				ImGui::TextColored( ImVec4( 1.f , 1.f , 0.f , 1.f ) , lightName.c_str() );
			}
			else if ( !m_debugSwitchs[ THREADED_VIEW_FRUSTUM_CULLING ] && !m_debugSwitchs[ GAME_CAMERA_VIEW_FRUSTUM_CULLING ] )
			{
				lightName += " View Frustum Culling Count = " + ToString( ( int ) m_currentlyDrawingShadowMeshes[ lightIndex ] );
				ImGui::TextColored( ImVec4( 1.f , 0.f , 0.f , 1.f ) , lightName.c_str() );
			}
		}
		ImGui::Checkbox( "Threaded ViewFrustumCulling"  , &m_debugSwitchs[ THREADED_VIEW_FRUSTUM_CULLING ] );
		ImGui::Checkbox( "Debug Draw Bounding Spheres" , &m_debugSwitchs[ VIEW_FRUSTUM_DEBUG_DRAW ] );
		ImGui::Checkbox( "Threaded Particles Spawning"  , &m_debugSwitchs[ THREADED_PARTICLE_SPAWN ] );
		ImGui::Checkbox( "Threaded Particle System Update"  , &m_debugSwitchs[ THREADED_PARTICLE_UPDATE ] );

		ImGui::InputInt( "Shadow Map Edge Size(input power a of 2)" , &m_shadowMapDimensionCopy );
		if( ImGui::Button( "Apply New Shadow Map Edge Dimension" ) )
		{
			m_shadowMapDimension = m_shadowMapDimensionCopy;

			for ( uint index = 0; index < TOTAL_LIGHTS; index++ )
			{
				delete m_shadowMap[ index ];
				m_shadowMap[ index ] = nullptr;
			}

			InitializeShadowMapTextures();
		}
		ImGui::InputFloat( "Shadow Cam Ortho Height" , &m_shadowCamHeight );
		if ( ImGui::Button( "Apply New Shadow Cam Ortho Height" ) )
		{
			m_lightsOrtho3DCamera.SetOrthoView3D( m_shadowCamHeight , CLIENT_ASPECT , -GAME_CAM_NEAR_Z , -GAME_CAM_FAR_Z );	
		}
		ImGui::SliderFloat( "Game Cam Near Z" , &m_nearZ , 0.001f , 0.999f );
		ImGui::SliderFloat( "Game Cam Far Z" , &m_farZ , 10.f , 1000.f );
		if( ImGui::Button("Apply new Near and FarZ" ) )
		{
			m_gameCamera.SetProjectionPerspective( GAME_CAM_FOV , CLIENT_ASPECT , -m_nearZ , -m_farZ );
		}
		//Vec3 cameraPos = m_gameCamera.GetCameraTransform().GetPostion();
		//float cameraPitch = m_gameCamera.GetCameraTransform().Get;
		//ImGui::InputFloat3( "Light World Position" , ( float* ) &m_gameCamera. );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PermanentDebugDataGUI()
{
	ImGui::Begin( "Debug Data" , 0 , ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/ | ImGuiWindowFlags_NoBackground );
	ImGui::Text( "RTVs Created = %d" , g_theRenderer->GetTotalRenderTargetPoolSize() );
	ImGui::Text( "RTVs In Free Pool = %d" , g_theRenderer->GetTexturePoolFreeCount() );

	if( m_isMouseUnlocked )
	{
		ImGui::TextColored( ImVec4( 1.f , 1.f , 1.f , 1.f ) , "GAME MODE = " ); ImGui::SameLine();
		ImGui::TextColored( ImVec4( 1.f , 0.f , 0.f , 1.f ) , "UI-MODE" );
	}
	else
	{
		ImGui::TextColored( ImVec4( 1.f , 1.f , 1.f , 1.f ) , "GAME MODE = " ); ImGui::SameLine();
		ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "GAME-MODE" );
	}

	if( m_frameRate >= 50.f )
	{
		ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "Frame Rate = %.3f" , m_frameRate );
	}
	else
	{
		ImGui::TextColored( ImVec4( 1.f , 0.f , 0.f , 1.f ) , "Frame Rate = %.3f" , m_frameRate );
	}
	ImGui::TextColored( ImVec4( 1.f , 0.f , 0.f , 1.f ) , "Worst Frame Time = %.3f" , m_worstFrame );
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "Best Frame Time = %.3f" , m_bestFrame );
	ImGui::TextColored( ImVec4( 1.f , 1.f , 0.f , 1.f ) , "Average Frame Time = %.3f" , m_rollingAvgFPS );
	
	
	Vec3 cameraPos = m_gameCamera.GetPosition();

	ImGui::TextColored( ImVec4( 1.f , 1.f , 1.f , 1.f ) , "Camera Pos " ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 1.f , 0.f , 0.f , 1.f ) , "X = %.3f " , cameraPos.x ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "Y = %.3f " , cameraPos.y ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 0.f , 0.f , 1.f , 1.f ) , "Z = %.3f " , cameraPos.z );
	ImGui::TextColored( ImVec4( 1.f , 1.f , 1.f , 1.f ) , "Camera Rotation " ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 1.f , 1.f , 0.f , 1.f ) , "Yaw = %.3f " , m_yaw ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 0.f , 1.f , 1.f , 1.f ) , "Pitch = %.3f " , m_pitch ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 1.f , 0.f , 1.f , 1.f ) , "Roll = 0.00" );
	ImGui::TextColored( ImVec4( 1.f , 1.f , 1.f , 1.f ) , "Total Drawable Objects = " , m_totalDrawableMeshes ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 1.f , 0.f , 0.f , 1.f ) , "%i" , m_totalDrawableMeshes );
	ImGui::TextColored( ImVec4( 1.f , 1.f , 1.f , 1.f ) , "Currently Drawing Objects = " , m_currentlyDrawingMeshes ); ImGui::SameLine();
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "%i" , m_currentlyDrawingMeshes );
	ImGui::End();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PlotFrameRate()
{
	ImGui::Begin( "Frame Rate" );
	if ( ImPlot::BeginPlot( "Frame Data Graph" ) )
	{
		//ImPlot::PlotLine( "FrameTime" , m_frameRates.data() , m_frameRates.size() );
		ImPlot::PlotLine( "FrameTime" , &m_frameTimes[0] , FRAME_RATE_BUFFER_SIZE );
		ImPlot::PlotLine( "FrameRate" , &m_frameRates[ 0 ] , FRAME_RATE_BUFFER_SIZE );
		ImPlot::EndPlot();
	}
	ImGui::End();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SpecialControlsDisplayGUI()
{
	ImGui::Begin( "Controls" , 0 , ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/ | ImGuiWindowFlags_NoBackground );
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "SWITCH BETWEEN UI AND GAME MODE - [ SHIFT + T ]" );
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "CHANGE CURRENT LIGHT FROM LIGHTS MENU USING THE UI MODE" );
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "[O] - Reset Camera Position and Orientation to Origin" );
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "[F6] - Move Current Light to the Camera Position" );
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "[F7] - Current Light Follows the Camera Position" );
	ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "[F9] - Light moves in a pre-defined Circular Path" );

	ImGui::End();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::ParticleEmitterSettingsGUI()
{
	if( ImGui::CollapsingHeader( "Particle Emitters" ) )
	{
		for( uint index = 0; index < NUM_STARS_EMITTERS ; index++ )
		{
			std::string emitterName = "Star ";
			emitterName.append( ToString( ( int ) index + 1 ) );
			size_t frustumCulledParticles = m_starEmitters[ index ].m_emitter->m_particlesInViewFrustum;
			size_t numAliveParticles = m_starEmitters[ index ].m_emitter->m_numAliveParticles;
			
			ImGui::TextColored( ImVec4( 0.f , 1.f , 0.f , 1.f ) , "Num Particles in View Frustum %s Emitter Particles = %u" , emitterName.c_str() , frustumCulledParticles );
			ImGui::TextColored( ImVec4( 1.f , 1.f , 0.f , 1.f ) , "Num Alive %s Emitter Particles = %u" , emitterName.c_str() , numAliveParticles );
			ImGui::TextColored( ImVec4( 1.f , 1.f , 1.f , 1.f ) , "Num Max Allowed %s Emitter Particles = %u" , emitterName.c_str() , PARTICLE_EMITTER_LIMIT );
				
			if( ImGui::TreeNode( emitterName.c_str() ) )
			{
				if( ImGui::TreeNode( "Movement" ) )
				{
					ImGui::InputFloat3( "Emitter Center Position" , ( float* ) &m_starEmitters[ index ].m_center );
					ImGui::SliderFloat( "Movement Multiplier" , &m_starEmitters[ index ].m_movementRadius , -30.f , 30.f );
					ImGui::TreePop();
					ImGui::Separator();
				}

				if( ImGui::TreeNode( "Particle Settings" ) )
				{
					ImGui::SliderInt( "New Particles Per Frame" , ( int* ) &m_starEmitters[ index ].m_numParticlesToSpawnPerFrame , 0 , 10000 );
					ImGui::SliderInt( "Min Life Time"			, ( int* ) &m_starEmitters[ index ].m_particleMinLifeTime , 0 , 120 );
					ImGui::SliderInt( "Max Life Time"			, ( int* ) &m_starEmitters[ index ].m_particleMaxLifeTime , 0 , 120 );
					ImGui::SliderFloat( "Velocity Multiplier"	, ( float* ) &m_starEmitters[ index ].m_particleVelocity , 0.f ,100.f );
					ImGui::SliderFloat2( "Particle Size"	, ( float* ) &m_starEmitters[ index ].m_particleSize , 0.f ,10.f );

					Vec4 startColor = m_starEmitters[ index ].m_particleStartColor.GetAsNormalizedFloat4();
					Vec4 endColor	= m_starEmitters[ index ].m_particleEndColor.GetAsNormalizedFloat4();
					ImGui::ColorEdit4( "Particle Start Color"  , ( float* ) &startColor );
					ImGui::ColorEdit4( "Particle End Color"	, ( float* ) &endColor );
					m_starEmitters[ index ].m_particleStartColor.SetColorFromNormalizedFloat( startColor );
					m_starEmitters[ index ].m_particleEndColor.SetColorFromNormalizedFloat( endColor );
					
					ImGui::TreePop();
					ImGui::Separator();
				}

				ImGui::TreePop();
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

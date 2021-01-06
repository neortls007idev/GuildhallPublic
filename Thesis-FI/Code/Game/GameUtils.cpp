#include "Engine/Core/StdExtensions.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SetLightPositionToOrigin( int lightIndex )
{
	m_lights.lights[ lightIndex ].worldPosition = Vec3::ZERO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SetLightPositionToCameraPos( int lightIndex )
{
	m_lights.lights[ lightIndex ].worldPosition = m_gameCamera.GetPosition();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SetLightDirectionToCameraForwardVector( int lightIndex )
{
	Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
	m_lights.lights[ lightIndex ].direction = -direction;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateToneMapData()
{
	switch( m_currentToneMap )
	{
		case ToneMap::NO_TONE:
			{
				m_toneMapTransform = Mat44::IDENTITY;
			}break;

		case ToneMap::GRAYSCALE:
			{
				Vec3 grayScale = Vec3( 0.2126f , 0.7152f , 0.0722f );
				m_toneMapTransform.SetBasisVectors3D( grayScale , grayScale , grayScale );
			}break;

		case ToneMap::SEPHIA:
			{
				Vec3 newRed = Vec3( 0.393f , 0.769f , 0.189f );
				Vec3 newGreen = Vec3( 0.394f , 0.686f , 0.168f );
				Vec3 newBlue = Vec3( 0.272f , 0.534f , 0.131f );
				m_toneMapTransform.SetBasisVectors3D( newRed , newGreen , newBlue );
			}break;
	}

	m_toneMapTransform.ScaleUniform3D( m_tonePower );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::AddGameOBJInstance( eGameObjModels modelType )
{
	if( nullptr == m_gameModels[ modelType ] )
	{
		return false;
	}

	Transform* newTransform = new Transform();
	EmplaceBackAtEmptySpace( m_ModelInstances[ modelType ] , newTransform );
//	EmplaceBackAtEmptySpace( m_isInstanceGarbage[ modelType ] , false );

	m_totalDrawableMeshes++;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::AddGameOBJInstances( eGameObjModels modelType )
{
	float deltaYaw		= ( m_maxYaw - m_minYaw ) / ( float ) m_numNewInstancesCount;
	float deltaPitch	= ( m_maxPitch - m_minPitch ) / ( float ) m_numNewInstancesCount;
	float deltaRoll		= ( m_maxRoll - m_minRoll ) / ( float ) m_numNewInstancesCount;

	float Yaw		= m_minYaw;
	float Pitch		= m_minPitch;
	float Roll		= m_minRoll;
	
	for( int index = 0 ; index < m_numNewInstancesCount ; index++ )
	{		
		if ( nullptr == m_gameModels[ modelType ] )
		{
			return false;
		}

		Transform* newTransform =  new Transform();

		EmplaceBackAtEmptySpace( m_ModelInstances[ modelType ] , newTransform );
//		EmplaceBackAtEmptySpace( m_isInstanceGarbage[ modelType ] , false );
		m_totalDrawableMeshes++;
		newTransform->SetPosition( Vec3::MakeFromSpericalCoordinates( Pitch , Yaw , m_numNewInstancesRadius ) );
		newTransform->SetRotation( Pitch , Yaw , Roll );
		Yaw		+= deltaYaw;
		Pitch	+= deltaPitch;
		Roll	+= deltaRoll;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::DestroyGameOBJInstance( eGameObjModels modelType , int instanceCount )
{
	if ( nullptr == m_ModelInstances[ modelType ][ instanceCount ] )
	{
		return false;
	}
	
	delete m_ModelInstances[ modelType ][ instanceCount ];
	m_ModelInstances[ modelType ][ instanceCount ] = nullptr;
	m_totalDrawableMeshes--;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadScene()
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( "Data/Models/ModelInstances.xml" );

	if( xmlDocument.ErrorID() == tinyxml2::XML_SUCCESS )
	{
		tinyxml2::XMLElement* modelsRoot = xmlDocument.RootElement();
		
		tinyxml2::XMLElement* modelInstance = modelsRoot->FirstChildElement( "ModelInstances" );
		tinyxml2::XMLElement* modelTransform = nullptr;

		for( int modelIndex = 0 ; modelIndex < NUM_GAME_MODELS ; modelIndex++ )
		{
			if( modelInstance == nullptr )
			{
				continue;
			}

			int modelinstanceCount		= ParseXmlAttribute( *modelInstance , "count" , 0 );
			int  enumValIndex			= ParseXmlAttribute( *modelInstance , "enumValue" , modelIndex );

			modelTransform = modelInstance->FirstChildElement( "Transform" );

			while( modelTransform != nullptr )
			{
				bool isDataBad			= false;

				Vec3 instancePos		= ParseXmlAttribute( *modelTransform , "pos" , Vec3::ZERO );
				float instanceYaw		= ParseXmlAttribute( *modelTransform , "yaw" , 0.0f );
				float instancePitch		= ParseXmlAttribute( *modelTransform , "pitch" , 0.0f );
				float instanceRoll		= ParseXmlAttribute( *modelTransform , "roll" , 0.0f );
				Vec3 instanceScale		= ParseXmlAttribute( *modelTransform , "scale" , Vec3::ZERO );

//--------------------------------------------------------------------------------------------------------------------------------------------
//				HACK TO CREATE SYMMETRY ON THE OPPOSITE SIDE WITH THE OTHER MESH
//--------------------------------------------------------------------------------------------------------------------------------------------
				
// 			if( modelIndex == 1 )
// 			{
// 				instancePos.x = abs( instancePos.x );
// 			}
			
				if( !isDataBad )
				{
					Transform* newInstance = new Transform();
					newInstance->SetPosition( instancePos );
					newInstance->SetRotation( instancePitch , instanceYaw , instanceRoll );
					newInstance->SetScale( instanceScale.x , instanceScale.y , instanceScale.z );
					m_ModelInstances[ enumValIndex ].emplace_back( newInstance );
					m_totalDrawableMeshes++;
				}
				modelTransform = modelTransform->NextSiblingElement( "Transform" );
			}
			modelInstance = modelInstance->NextSiblingElement( "ModelInstances" );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::SaveScene()
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( "Data/Models/ModelInstances.xml" );
	xmlDocument.Clear();

	XMLElement* modelInstancesRootNode = xmlDocument.NewElement( "ModelIntancesData" );
	xmlDocument.LinkEndChild( modelInstancesRootNode );

	for ( int modelType = 0 ; modelType < NUM_GAME_MODELS ; modelType++ )
	{
		XMLElement* modelInstances = xmlDocument.NewElement( "ModelInstances" );
		tinyxml2::XMLText* modeInstanceText = xmlDocument.NewText( "" );
		
		modelInstances->SetAttribute( "count" , ToString( ( int ) m_ModelInstances[ modelType ].size() ).c_str() );
		modelInstances->SetAttribute( "enumValue" , modelType );
		
		for ( size_t modelInstanceCount = 0 ; modelInstanceCount < m_ModelInstances[modelType].size() ; modelInstanceCount++ )
		{
			Transform* currTransform = m_ModelInstances[ modelType ][ modelInstanceCount ];

			if ( nullptr == currTransform )
			{
				continue;
			}
			
			XMLElement* instanceTransform = xmlDocument.NewElement( "Transform" );
			tinyxml2::XMLText* instanceTransformText = xmlDocument.NewText( "" );

			instanceTransform->SetAttribute( "pos"		, ToString( currTransform->GetPostion() ).c_str() );
			instanceTransform->SetAttribute( "yaw"		, ToString( currTransform->GetYaw() ).c_str() );
			instanceTransform->SetAttribute( "pitch"	, ToString( currTransform->GetPitch() ).c_str() );
			instanceTransform->SetAttribute( "roll"		, ToString( currTransform->GetRoll() ).c_str() );
			instanceTransform->SetAttribute( "scale"	, ToString( currTransform->GetScale() ).c_str() );
			
			modelInstances->InsertEndChild( instanceTransform );
			modelInstances->LinkEndChild( instanceTransform );
			instanceTransform->LinkEndChild( instanceTransformText );

		}
		modelInstances->LinkEndChild( modeInstanceText );
		modelInstancesRootNode->LinkEndChild( modelInstances );
	}

	xmlDocument.SaveFile( "Data/Models/ModelInstances.xml" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
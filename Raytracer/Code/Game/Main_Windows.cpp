#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Engine/Experimental/Raytracer/Ray.hpp"
#include "Engine/Experimental/Raytracer/RayHittableList.hpp"
#include "Engine/Experimental/Raytracer/RayHittableSphere.hpp"
#include "Engine/Experimental/Raytracer/RaytracerUtils.hpp"

#include <memory>			// for std::make_shared

#include "Engine/Experimental/Raytracer/LambertianDiffuseMaterial.hpp"
#include "Engine/Experimental/Raytracer/LDM_Metal.hpp"
#include "Engine/Experimental/Raytracer/RaytracerCamera.hpp"
#include "Engine/Experimental/Raytracer/RaytracerMaterialDielectrics.hpp"

extern InputSystem* g_theInput;

Window* g_theWindow = nullptr;


bool g_isQuitting = false;							// ...becomes App::m_isQuitting
const char* APP_NAME = "Protogame2D";				// ...becomes ??? (Change this per project!)

#include <iostream>

#pragma warning( push )
#pragma warning ( disable: 4244 )

RayHittableList RandomizeScene()
{
	RayHittableList world;

	world.Add( std::make_shared< RayHittableSphere >( Vec3( 0.f , -1000.f , 0.f ) , 1000.f ,
	                                                  std::make_shared< LambertianDiffuse >(
		                                                  Vec3( 0.5f , 0.5f , 0.5f ) ) ) );

	RandomNumberGenerator rng;
	
	for ( int a = -11; a < 11; a++ )
	{
		for ( int b = -11; b < 11; b++ )
		{
			float choose_mat = rng.RollRandomFloatBetweenZerotoOne();
			rng.manuallyIncrementPosition();
			
			Vec3 center( ( float )a + 0.9f * rng.RollRandomFloatBetweenZerotoOne() , 0.2f ,
			             ( float )b + 0.9f * rng.RollRandomFloatBetweenZerotoOne() );

			rng.manuallyIncrementPosition();
			
			if ( ( center - Vec3( 4.f , 0.2f , 0.f ) ).GetLength() > 0.9f )
			{
				if ( choose_mat < 0.8f )
				{
					// diffuse
					Vec3 random1 = rng.RollRandomVec3BetweenZerotoOne();
					rng.manuallyIncrementPosition();
					Vec3 random2 = rng.RollRandomVec3BetweenZerotoOne();
					rng.manuallyIncrementPosition();
					
					Vec3 albedo = random1 * random2;
					
					world.Add( std::make_shared<RayHittableSphere>( center ,
						  0.2f , std::make_shared<LambertianDiffuse>( albedo ) ) );
				}
				
				else if ( choose_mat < 0.95f )
				{
					// LDM_Metal
					Vec3 albedo = rng.RollRandomVec3InRange( 0.5f , 1.0f );
					rng.manuallyIncrementPosition();
					float fuzz = rng.RollRandomFloatInRange( 0.f , 0.5f );
					rng.manuallyIncrementPosition();
					world.Add(std::make_shared<RayHittableSphere>( center ,
						  0.2f , std::make_shared<LDM_Metal>( albedo , fuzz ) ) );
				}
				else
				{
					// glass
					world.Add( std::make_shared<RayHittableSphere>( center ,
						  0.2 , std::make_shared<RaytracerMaterialDielectric>( 1.5f ) ) );
				}
			}
		}
	}

	world.Add( std::make_shared<RayHittableSphere>( Vec3( 0.f , 1.f , 0.f ) ,
						1.0f , std::make_shared<RaytracerMaterialDielectric>( 1.5f ) ) );

	world.Add( std::make_shared<RayHittableSphere>( Vec3( -4.f , 1.f , 0.f ) ,
				1.0f , std::make_shared<LambertianDiffuse>( Vec3( 0.4f , 0.2f , 0.1f ) ) ) );

	world.Add( std::make_shared<RayHittableSphere>( Vec3( 4.f , 1.f , 0.f ) ,
				1.0f , std::make_shared<LDM_Metal>( Vec3( 0.7f , 0.6f , 0.5f ) , 0.0f ) ) );

	return world;
}


int main()
{
	const int imageWidth		= 2160;
	const int imageHeight		= 1080;
	//const int imageWidth		= 600;
	//const int imageHeight		= 300;
	const int samplesPerPixel	= 100;
	const int maxDepth			= 50;
	
	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	RayHittableList world;
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( 0 , 0 , -1 ) , 0.5 ) );
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( 0 , -100.5 , -1 ) , 100 ) );

	//world.Add( std::make_shared<RayHittableSphere>(	Vec3(  0.f , 0.f , -1.f ) , 0.5f		 , std::make_shared<LambertianDiffuse>( Vec3( 0.7 , 0.3 , 0.3 ) ) ) );
	//world.Add( std::make_shared<RayHittableSphere>(	Vec3(  0.f , -100.5f , -1.f ) , 100.f , std::make_shared<LambertianDiffuse>( Vec3( 0.8 , 0.8 , 0.0 ) ) ) );
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( 1.f , 0.f , -1.f ) , 0.5f , std::make_shared<LDM_Metal>( Vec3( 0.8 , 0.6 , 0.2 ) ) ) );
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( -1.f , 0.f , -1.f ) , 0.5f , std::make_shared<LDM_Metal>( Vec3( 0.8 , 0.8 , 0.8 ) ) ) );

	//world.Add( std::make_shared<RayHittableSphere>( Vec3( 0.f , 0.f , -1.f ) , 0.5f , std::make_shared<LambertianDiffuse>( Vec3( 0.1f , 0.2f , 0.5f ) ) ) );
	//world.Add( std::make_shared<RayHittableSphere>(	Vec3( 0.f , -100.5f , -1.f ) , 100.f , std::make_shared<LambertianDiffuse>( Vec3( 0.8f , 0.8f , 0.0f ) ) ) );
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( 1.f , 0.f , -1.f ) , 0.5f , std::make_shared<LDM_Metal>( Vec3( 0.8f , 0.6f , 0.2f ) , 0.3f ) ) );
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( -1.f , 0.f , -1.f ) , 0.5f   , std::make_shared<RaytracerMaterialDielectric>( 1.5f ) ) );
	//world.Add( std::make_shared<RayHittableSphere>( Vec3( -1.f , 0.f , -1.f ) , -0.45f , std::make_shared<RaytracerMaterialDielectric>( 1.5f ) ) );

	world = RandomizeScene();
	
	//RaytracerCamera camera;

	Vec3 lookfrom( 13.f , 2.f , 3.f );
	Vec3 lookat( 0.f , 0.f , 0.f );
	Vec3 vup( 0.f , 1.f , 0.f );
	float distanceToFocus = 10.0f;
	float aperture = 0.1f;

	const float aspectRatio = float( imageWidth ) / imageHeight;
	
	RaytracerCamera camera( lookfrom , lookat , vup , 20.f , aspectRatio , aperture , distanceToFocus );
	
	RandomNumberGenerator rng;
	
	for ( int j = imageHeight - 1; j >= 0; --j )
	{
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for ( int i = 0; i < imageWidth; ++i )
		{
			Vec3 color = Vec3::ZERO;

			for ( int s = 0; s < samplesPerPixel; ++s )
			{
				float u = ( ( float ) i + rng.RollRandomFloatBetweenZerotoOne() ) / imageWidth;
				rng.manuallyIncrementPosition();
				float v = ( ( float ) j + rng.RollRandomFloatBetweenZerotoOne() ) / imageHeight;
				rng.manuallyIncrementPosition();
				
				Ray ray = camera.GetRay( u , v );
				color += RayColor( ray , world , maxDepth );
			}

			color.WriteColor( std::cout , samplesPerPixel );
		}
	}
	
	std::cerr << "\nDone.\n";
}
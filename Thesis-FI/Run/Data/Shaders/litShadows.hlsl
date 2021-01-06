#include "ShaderMathUtils.hlsl"
#include "LightMathUtils.hlsl"

//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.  
// If you say "Draw(3,0)", you are telling to the GPU to expect '3' sets, or 
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element. 
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//                      PROGRAMMABLE SHADER STAGES STRUCTS
//--------------------------------------------------------------------------------------
//
// INPUT FROM VERTEX BUFFERS
//
//--------------------------------------------------------------------------------------
struct vs_input_t
{
   // we are not defining our own input data; 
    float3 position : POSITION;
    float4 color    : COLOR;
    float2 uv       : TEXCOORD;

    float3 normal   : NORMAL;
    float4 tangent  : TANGENT;
};


//--------------------------------------------------------------------------------------
//
// INPUT FOR PASSING DATA FROM VERTEX STAGE TO FRAGMENT STAGE (V-2-F)
// 
//--------------------------------------------------------------------------------------

struct v2f_t
{
    float4 position             : SV_POSITION;
    float4 color                : COLOR;
    float2 uv                   : UV;

    float3 world_position       : WORLD_POSITION;
    float3 world_normal         : WORLD_NORMAL;
    float4 world_tangent        : WORLD_TANGENT;

    float4 lightViewPosition[ TOTAL_LIGHTS ] : LIGHTVIEWPOS;
    float3 lightPos[ TOTAL_LIGHTS ] : LIGHTPOS;
};

//--------------------------------------------------------------------------------------
//                      PROGRAMMABLE SHADER STAGES FUNCTIONS
//--------------------------------------------------------------------------------------
//
// VERTEX SHADER
//
//--------------------------------------------------------------------------------------

v2f_t VertexFunction(vs_input_t input)
{
    v2f_t v2f = (v2f_t) 0;

   // move the vertex through the spaces
    float4 local_pos        = float4( input.position , 1.0f );                                      // passed in position is usually inferred to be "local position", ie, local to the object
    float4 world_pos        = mul( MODEL , local_pos );                                             // world pos is the object moved to its place int he world by the model, not used yet

   // float4 world_pos_copy   = world_pos;

    float4 camera_pos       = mul( VIEW , world_pos );
    float4 clip_pos         = mul( PROJECTION , camera_pos );                                   
                                                                                                
   // normal is currently in model/local space                                                  
    float4 local_normal     = float4( input.normal , 0.0f );                                    
    float4 world_normal     = mul( MODEL , local_normal );                                      
                                                                                                
   // tangent & bitangent                                                                       
                                                                                                
    float4 local_tangent    = float4( input.tangent.xyz , 0.0f );                               
    float4 world_tangent    = mul( MODEL , local_tangent );                                     


    for( uint lightIndex = 0 ; lightIndex < TOTAL_LIGHTS ; lightIndex++ )
    {
        v2f.lightViewPosition[ lightIndex ] = float4( input.position , 1.0f );

        float4 temp = mul( MODEL , v2f.lightViewPosition[ lightIndex ] );
        v2f.lightViewPosition[ lightIndex ] = temp;
        temp = mul( LIGHT_VIEW[ lightIndex ].LIGHT_VIEW , v2f.lightViewPosition[ lightIndex ] );
        v2f.lightViewPosition[ lightIndex ] = temp;
        temp = mul( LIGHT_VIEW[ lightIndex ].LIGHT_PROJECTION , v2f.lightViewPosition[ lightIndex ] );
        v2f.lightViewPosition[ lightIndex ] = temp;

        v2f.lightPos[ lightIndex ] = normalize( LIGHTS[ lightIndex ].worldPosition.xyz - world_pos.xyz );
    }
		
    v2f.position            = clip_pos;                                                             // we want to output the clip position to raster (a perspective point)
    v2f.color               = input.color * TINT;
    v2f.uv                  = input.uv;
    v2f.world_position      = world_pos.xyz;
    v2f.world_normal        = world_normal.xyz;
    v2f.world_tangent       = float4( world_tangent.xyz , input.tangent.w );
    return v2f;
}

//--------------------------------------------------------------------------------------
//                  FRAGMENT SHADER
//--------------------------------------------------------------------------------------
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
//--------------------------------------------------------------------------------------

struct fragmentFunctionOutput
{
    float4 color        : SV_Target0;
    float4 bloom        : SV_Target1;
    //float4 lightView    : SV_Target2;
};

Texture2D<float4> depthMapTexture[ TOTAL_LIGHTS ] : register( t8 );

//Texture2D<float4> depthMapTexture0 : register( t8 );
//Texture2D<float4> depthMapTexture1 : register( t9 );

fragmentFunctionOutput FragmentFunction( v2f_t input )
{
	// Shadow mapping requires a bias adjustment when comparing the depth of the light
	// and the depth of the object due to the low floating point precision of the depth map.
	
   // float bias = 0.0002f;
    //float bias = 0.0002f;
    float bias = 0.0002f;
  
    float4 color = float4( 0.f.xxxx );
    float2 projectTexCoord[TOTAL_LIGHTS];
    float  depthValue;
    float  lightDepthValue;
    float  lightIntensity;
    float4 textureColor;
    float3 lightDirection[ TOTAL_LIGHTS ];

    for( uint lightDirIndex = 0 ; lightDirIndex < TOTAL_LIGHTS ; lightDirIndex++ )
    {
        lightDirection[ lightDirIndex ] = -LIGHTS[ lightDirIndex ].direction;
    }
  
   // Calculate the projected texture coordinates for sampling the shadow map (depth buffer texture) based on the light's viewing position.
   // Calculate the projected texture coordinates.
   //   uint index = 0;
   for (uint index = 0; index < TOTAL_LIGHTS; index++)
   {
        float2 mapDim;
        depthMapTexture[ index ].GetDimensions( mapDim.x , mapDim.y );
	// Range mapping NDC space to 0 to 1
      
        projectTexCoord[ index ].x = input.lightViewPosition[ index ].x / input.lightViewPosition[ index ].w / 2.0f + 0.5f;
        projectTexCoord[ index ].y = -input.lightViewPosition[ index ].y / input.lightViewPosition[ index ].w / 2.0f + 0.5f;
	
   // Check if the projected coordinates are in the view of the light, if not then the pixel gets just an ambient value.
   // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
       if( ( saturate( projectTexCoord[ index ].x ) == projectTexCoord[ index ].x ) &&
		( saturate( projectTexCoord[ index ].y ) == projectTexCoord[ index ].y ) )
       {
           depthValue = depthMapTexture[ index ].Sample( sSampler , projectTexCoord[ index ] ).r;
                 	
           // Calculate the depth of the light.
           lightDepthValue = input.lightViewPosition[ index ].z / input.lightViewPosition[ index ].w;
  
           // Subtract the bias from the lightDepthValue.
           lightDepthValue = lightDepthValue - bias;

           if( lightDepthValue < depthValue )
           {
		    // Calculate the amount of light on this pixel.
               lightIntensity = saturate( dot( lightDirection[ index ] , input.world_normal ) );
               
               //lightIntensity = saturate( dot( input.lightPos[ index ] , input.world_normal ) );
               //lightIntensity = saturate( dot( input.world_normal , input.lightPos[ index ] ) );
		           
               if( lightIntensity > 0.f )
               {
			    // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                    color += float4( LIGHTS[ index ].color , lightIntensity );
                }
           }
       }
   }
       
//--------------------------------------------------------------------------------------
//              SAMPLE THE TEXTURES
//--------------------------------------------------------------------------------------    
        float4 diffuseColor = tDiffuse.Sample( sSampler , input.uv );
        float4 normalColor = tNormal.Sample( sSampler , input.uv );
//--------------------------------------------------------------------------------------
//              COMPUTE SURFACE COLOR
//--------------------------------------------------------------------------------------
    
        float3 surfaceColor = diffuseColor.xyz * input.color.xyz;
        surfaceColor = pow( abs( surfaceColor ) , GAMMA.xxx );
        float alpha = diffuseColor.w * input.color.w;
      
        float3 tangent = normalize( input.world_tangent.xyz );
        float3 normal = normalize( input.world_normal );
        float3 bitangent = normalize( cross( normal , tangent ) ) * input.world_tangent.w;
        float3x3 TBN = float3x3( tangent , bitangent , normal );
        float3 directionToCamera = normalize( CAMERA_POSITION - input.world_position ); // As the Camera IS OUR EYE
    
        float3 surfaceNormal = NormalColorToVector3( normalColor.xyz );
        float3 worldNormal = mul( surfaceNormal , TBN );
    worldNormal = normal;
//--------------------------------------------------------------------------------------
//              COMPUTE LIGHT FACTOR
//--------------------------------------------------------------------------------------
        PostLightingData lightResult = ComputeLightingAt( input.world_position , worldNormal , surfaceColor , float3( 0.0f , 0.0f , 0.0f ) , SPECULAR_FACTOR );
        float3 finalColor = lightResult.diffuse + lightResult.specularEmmisive;
   
        float3 bloom = max( float3( 0.f , 0.f , 0.f ) , finalColor - float3( 1.f , 1.f , 1.f ) );
        bloom = pow( bloom.xyz , INVERSE_GAMMA.xxx );
   // gamma correct back, and output
        finalColor = pow( abs( finalColor.xyz )  , INVERSE_GAMMA.xxx );
        //finalColor = ApplyLinearFog( input.world_position , finalColor );
               
        fragmentFunctionOutput output;
        output.color = float4( finalColor.xyz , alpha );
        output.bloom = float4( bloom , 1 );
    	output.color *= color;
    // output.color = float4( lightDepthValue.xxx , 1.f );
    // output.color = float4( depthValue.xxx , 1.f );
    // output.color = float4( projectTexCoord[ index ] , 0.f , 1.f );
    //float4( lightIntensity.xxx , 1.f );
        return output;    
}
//--------------------------------------------------------------------------------------
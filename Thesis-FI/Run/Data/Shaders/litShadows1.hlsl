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
        float4x4 lightPV = mul( LIGHT_VIEW[ lightIndex ].LIGHT_PROJECTION , LIGHT_VIEW[ lightIndex ].LIGHT_VIEW );
        v2f.lightViewPosition[ lightIndex ] = mul( lightPV , world_pos );
        
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
};

Texture2D<float4> depthMapTexture[ TOTAL_LIGHTS ] : register( t8 );

fragmentFunctionOutput FragmentFunction( v2f_t input )
{
       
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
    
    float3 directionToEye = normalize( CAMERA_POSITION - input.world_position );
 
    //float3 diffuse                  = AMBIENT.xyz * AMBIENT.w;                                                          // assumes ambient is set from a user - so sRGB space
    float3 diffuse = pow( AMBIENT.xyz * AMBIENT.w , GAMMA.xxx ); // assumes ambient is set from a user - so sRGB space
    float3 specular = float3( 0.0f.xxx );
    float3 emmisiveColor = float3( 0.0f.xxx );


    // Shadow mapping requires a bias adjustment when comparing the depth of the light
	// and the depth of the object due to the low floating point precision of the depth map.
	
    // float bias = 0.0002f;
    float bias = 0.0002f;
  
    float4 color = float4( 0.f.xxxx );
    float2 projectTexCoord[ TOTAL_LIGHTS ];
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
    float3 lightDirection[ TOTAL_LIGHTS ];
    
    float2 lightFactors;

    for( uint lightDirIndex = 0 ; lightDirIndex < TOTAL_LIGHTS ; lightDirIndex++ )
    {
        if( LIGHTS[ lightDirIndex ].lightType == 1 )
        {
            lightDirection[ lightDirIndex ] = -LIGHTS[ lightDirIndex ].direction;
        }
    }
  
    for( uint index = 0 ; index < TOTAL_LIGHTS ; index++ )
    {
        lightFactors = ComputeLightFactor( LIGHTS[ index ] , input.world_position , worldNormal , directionToEye );
        
        float3 lightColor = LIGHTS[ index ].color.xyz;
        lightColor = pow( lightColor , GAMMA.xxx );                                                                 // assumes light color is set by a user - so sRGB space
        bias = LIGHTS[ index ].shadowBias;                                                                          // Shadow bias of the light source          
	// Range mapping NDC space to 0 to 1
      
   // Calculate the projected texture coordinates for sampling the shadow map (depth buffer texture) based on the light's viewing position.
   //  Here, u, v ranges [0, 1] provided x, y ranges [-1,1]. We scale the y-coordinate by a negative to invert the axis because the positive y-axis in NDC coordinates goes in the direction
   //  opposite to the positive v-axis in texture coordinates.
                
        projectTexCoord[ index ].x = input.lightViewPosition[ index ].x / input.lightViewPosition[ index ].w / 2.0f + 0.5f;
        projectTexCoord[ index ].y = -input.lightViewPosition[ index ].y / input.lightViewPosition[ index ].w / 2.0f + 0.5f;
	
        if( projectTexCoord[ index ].x < 0.0 || projectTexCoord[ index ].x > 1.0 || projectTexCoord[ index ].y < 0.0 || projectTexCoord[ index ].y > 1.0 )
        {
            diffuse += lightFactors.x * lightColor;
            specular += lightFactors.y * lightColor;
        }
        
   // Check if the projected coordinates are in the view of the light, if not then the pixel gets just an ambient value.
   // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
        if( ( saturate( projectTexCoord[ index ].x ) == projectTexCoord[ index ].x ) &&
		( saturate( projectTexCoord[ index ].y ) == projectTexCoord[ index ].y ) )
        //projectTexCoord[ index ].x = saturate( projectTexCoord[ index ].x );
        //projectTexCoord[ index ].y = saturate( projectTexCoord[ index ].y );
        {
            depthValue = depthMapTexture[ index ].Sample( sSampler , projectTexCoord[ index ] ).r;
                 	
           // Calculate the depth of the light.
            lightDepthValue = input.lightViewPosition[ index ].z / input.lightViewPosition[ index ].w;
  
           // Subtract the bias from the lightDepthValue.
            lightDepthValue = lightDepthValue - bias;
            
            if( lightDepthValue <= depthValue )
            {
		    // Calculate the amount of light on this pixel.
               //lightIntensity = saturate( dot( lightDirection[ index ] , input.world_normal ) );
                if( LIGHTS[ index ].lightType == 1 )
                {
                    lightIntensity = saturate( dot( lightDirection[ index ] , input.world_normal ) );
                }
                if( LIGHTS[ index ].lightType == 2 )
                {
                    //lightIntensity = saturate( dot( LIGHTS[ index ].worldPosition , input.world_normal ) );
                    lightIntensity = saturate( dot( input.lightPos[ index ] , input.world_normal ) );
                    //lightIntensity = saturate( dot( input.world_normal , input.lightPos[ index ] ) );
                    //lightIntensity = saturate( dot( input.world_normal , input.lightPos[ index ] ) );
                    /*saturate( dot( LIGHTS[ index ].worldPosition , input.world_normal ) );*/
                }
		           
                if( lightIntensity > 0.f )
                {
			        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                    //color += float4( LIGHTS[ index ].color , LIGHTS[ index ].intensity );
                    if( LIGHTS[ index ].shadowFlag > 0.f )
                    {
                        diffuse += lightFactors.x * lightColor;
                        specular += lightFactors.y * lightColor;
                    }
                }
            }
        }
        
        if( LIGHTS[ index ].shadowFlag < 1.f )
        {
            diffuse += lightFactors.x * lightColor;
            specular += lightFactors.y * lightColor;
        }
    }
    
    // limit it
    diffuse = min( DIFFUSE_FACTOR * diffuse , float3( 1.f.xxx ) );
    specular *= SPECULAR_FACTOR; // scale back specular based on spec factor

    // returns light color (in linear space)
    PostLightingData result;
    result.specularEmmisive = specular + emmisiveColor;
    result.diffuse = diffuse * surfaceColor;
        
        
    float3 finalColor = result.diffuse + result.specularEmmisive;
   
    float3 bloom = max( float3( 0.f , 0.f , 0.f ) , finalColor - float3( 1.f , 1.f , 1.f ) );
    bloom = pow( bloom.xyz , INVERSE_GAMMA.xxx );
    // gamma correct back, and output
    finalColor = pow( abs( finalColor.xyz ) , INVERSE_GAMMA.xxx );
    //finalColor = ApplyLinearFog( input.world_position , finalColor );
           
    fragmentFunctionOutput output;
    output.color = float4( finalColor.xyz , alpha );
    output.bloom = float4( bloom , 1 );
    
    //	output.color = color;
    // output.color = float4( lightDepthValue.xxx , 1.f );
    // output.color = float4( depthValue.xxx , 1.f );
    // output.color = float4( projectTexCoord[ 0 ] , 0.f , 1.f );
    // output.color = float4( lightIntensity.xxx , 1.f );
        return output;    
}
//--------------------------------------------------------------------------------------
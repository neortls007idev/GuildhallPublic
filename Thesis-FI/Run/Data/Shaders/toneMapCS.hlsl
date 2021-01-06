//--------------------------------------------------------------------------------------
// constants
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// vs_input_t is unused but added to avoid a hard crash due to setting invalid input layout 
//--------------------------------------------------------------------------------------
struct vs_input_t
{
    uint vidx : SV_VERTEXID; // SV_* stands for System Variable (ie, built-in by D3D11 and has special meaning)
                                                            // in this case, is the the index of the vertex coming in.
};

//--------------------------------------------------------------------------------------

cbuffer material_constants : register( b8 ) // constant buffer slot 9
{
    float4x4 toneMapMatrix;
};

//--------------------------------------------------------------------------------------

float4 ToneMap( float4 color )
{
    float4 toneMappedColor;
    toneMappedColor.r = toneMapMatrix[ 0 ][ 0 ] * color.r + toneMapMatrix[ 1 ][ 0 ] * color.g + toneMapMatrix[ 2 ][ 0 ] * color.b;
    toneMappedColor.g = toneMapMatrix[ 0 ][ 1 ] * color.r + toneMapMatrix[ 1 ][ 1 ] * color.g + toneMapMatrix[ 2 ][ 1 ] * color.b;
    toneMappedColor.b = toneMapMatrix[ 0 ][ 2 ] * color.r + toneMapMatrix[ 1 ][ 2 ] * color.g + toneMapMatrix[ 2 ][ 2 ] * color.b;
    toneMappedColor.a = toneMapMatrix[ 3 ][ 3 ];
    return toneMappedColor;
}

//--------------------------------------------------------------------------------------

Texture2D<float4> gInput : register( t0 );
RWTexture2D<float4> gOutput : register( u1 );

//--------------------------------------------------------------------------------------
// Compute Shader
//--------------------------------------------------------------------------------------

[numthreads( 32 , 32 , 1 )]
void ComputeFunction( uint3 DTid : SV_DispatchThreadID )
{
    float4 imageColor = gInput[ DTid.xy ];
    float4 finalColor = mul( toneMapMatrix , imageColor );
    finalColor = ToneMap( imageColor );
	
    float4 imageFactor = float4( imageColor.xyz * ( 1 - finalColor.a ) , ( 1 - finalColor.a ) );
    finalColor = float4( finalColor.xyz * finalColor.a , finalColor.a ) + imageFactor;

    gOutput[ DTid.xy ] = finalColor;
    //float4( 1.f , 1.f , 1.f , 1.f );

}

//--------------------------------------------------------------------------------------

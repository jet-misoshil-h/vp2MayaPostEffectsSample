//**************************************************************************/
// Copyright (c) 2024 jet-misoshil-h.
// All rights reserved.
//
// vp2AMGPostEffect
// Repository: https://github.com/jet-misoshil-h/vp2AMGPostEffect
// Author: jet_misoshil (jet-misoshil-h)
//**************************************************************************/

#ifndef _AMG_POSTEFFECT_SAMPLE_FXH_
#define _AMG_POSTEFFECT_SAMPLE_FXH_

// World-view-projection transformation.
float4x4 gWVPXf : WorldViewProjection < string UIWidget = "None"; >;

// Screen size, in pixels.
float2 gScreenSize : ViewportPixelSize < string UIWidget = "None"; >;
// Flip in V
bool gVerticalFlip = false;
static float2 gTexelSize = 1.0f / gScreenSize;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Screen Quad Vertex Shader
////////////////////////////////////////////////////////////////////////////////////////////////////

// Vertex shader input structure.
struct VS_INPUT_ScreenQuad
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD0;
};

// Vertex shader output structure.
struct VS_TO_PS_ScreenQuad
{
    float4 HPos : SV_Position;
    float2 UV : TEXCOORD0;
};

// Vertex shader.
VS_TO_PS_ScreenQuad VS_ScreenQuad(VS_INPUT_ScreenQuad In)
{
    VS_TO_PS_ScreenQuad Out;
    
    // Transform the position from object space to clip space for output.
    Out.HPos = mul(float4(In.Pos, 1.0f), gWVPXf);
    // Pass the texture coordinates.
    Out.UV = gVerticalFlip ? float2( In.UV[0], 1.0f - In.UV[1] ) : In.UV;
    
    return Out;
}

#define SCREEN_SAMPLE(tex, samp, uv) tex.Sample(samp, uv)

#endif // _AMG_POSTEFFECT_SAMPLE_FXH_
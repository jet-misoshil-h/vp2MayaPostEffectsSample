//**************************************************************************/
// Copyright (c) 2024 jet-misoshil-h.
// All rights reserved.
//
// vp2AMGPostEffect
// Repository: https://github.com/jet-misoshil-h/vp2AMGPostEffect
// Author: jet_misoshil (jet-misoshil-h)
//
// Some descriptions are provided by modifying the shader for Unity from the following site.
// https://light11.hatenadiary.com/entry/2018/03/15/000022
//**************************************************************************/

#include "PostEffectSample10.fxh"

// MQuadRenderは複数用意しますが、同じシェーダーファイルを扱うのでTexture宣言を全部記述します
// こちらは1つ目のテクスチャの入力プロパティ
Texture2D gInputTex1 < string UIWidget = "None";
> ;
// こちらは2つ目のテクスチャの入力プロパティ
Texture2D gInputTex2 < string UIWidget = "None";
> ;
// こちらは3つ目のテクスチャの入力プロパティ
Texture2D gInputTex3 < string UIWidget = "None";
> ;

// どれも同じサンプラー情報でよいのでひとつ宣言
// UVをずらすためwrap指定してます
SamplerState gInputSampler
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Blend State
BlendState AlphaBlend {
    AlphaToCoverageEnable = false;
    BlendEnable[0] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
};

// ------------------------------------------------------------------------------------------------
// ピクセルシェーダー
// ------------------------------------------------------------------------------------------------

// 赤色のみ抽出して描画するピクセルシェーダー
float4 PS_PostEffectSampleRedPass(VS_TO_PS_ScreenQuad i) : SV_TARGET
{
    // 10ピクセル分正方向にずらしたオフセットを用意
    float2 offset = i.UV + (gTexelSize * 10);

    float4 col = SCREEN_SAMPLE(gInputTex1, gInputSampler, i.UV);
    float4 offsetCol = SCREEN_SAMPLE(gInputTex1, gInputSampler, offset);
    

    // ずらした分のカラーから赤色のみ抽出して描画します
    return float4(
        offsetCol.r,
        col.g,
        col.b,
        0.5f
    );
}

// 青色のみ抽出して描画するピクセルシェーダー
float4 PS_PostEffectSampleBluePass(VS_TO_PS_ScreenQuad i) : SV_TARGET
{
    // 10ピクセル分負方向にずらしたオフセットを用意
    float2 offset = i.UV - (gTexelSize * 10);

    float4 col = SCREEN_SAMPLE(gInputTex1, gInputSampler, i.UV);
    float4 offsetCol = SCREEN_SAMPLE(gInputTex1, gInputSampler, offset);
    

    // ずらした分のカラーから青色のみ抽出して描画します
    return float4(
        col.r,
        col.g,
        offsetCol.b,
        0.5f
    );
}

// 最終合成を行うピクセルシェーダー
float4 PS_PostEffectSampleCombinePass(VS_TO_PS_ScreenQuad i) : SV_TARGET
{
    float4 color1 = SCREEN_SAMPLE(gInputTex1, gInputSampler, i.UV);
    float3 black = float3(0.0f, 0.0f, 0.0f);
    // カラーターゲットのカラーに加算合成します
    float4 color2 = SCREEN_SAMPLE(gInputTex2, gInputSampler, i.UV);
    float4 color3 = SCREEN_SAMPLE(gInputTex3, gInputSampler, i.UV);
    color1.rgb += color2.rgb;
    color1.rgb += color3.rgb;
    return color1;
}

// カラーターゲットを一度テンプバッファーに逃がすピクセルシェーダー
float4 PS_Blit(VS_TO_PS_ScreenQuad i) : SV_TARGET
{
    return SCREEN_SAMPLE(gInputTex1, gInputSampler, i.UV);
}

// カラーターゲットをクリアするピクセルシェーダー
float4 PS_Clear(VS_TO_PS_ScreenQuad i) : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

// ------------------------------------------------------------------------------------------------
// テクニックを用意します
// ------------------------------------------------------------------------------------------------

// 赤と青をずらすテクニックです
technique10 Shift
{
    
    pass P0
    {
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, VS_ScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_PostEffectSampleRedPass()));
    }
    pass P1
    {
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, VS_ScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_PostEffectSampleBluePass()));
    }
}


// 赤にシフトして描画を書き込むテクニックです
technique10 RedShift
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS_ScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_PostEffectSampleRedPass()));
    }
}

// 青にシフトして描画を書き込むテクニックです
technique10 BlueShift
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS_ScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_PostEffectSampleBluePass()));
    }
}

// 最終合成のテクニックです
technique10 Combine
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS_ScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_PostEffectSampleCombinePass()));
    }
}

// テンプバッファーに書き込むテクニックです
technique10 Blit
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS_ScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_Blit()));
    }
}

// クリア用のテクニックです
technique10 Clear
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS_ScreenQuad()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS_Clear()));
    }
}
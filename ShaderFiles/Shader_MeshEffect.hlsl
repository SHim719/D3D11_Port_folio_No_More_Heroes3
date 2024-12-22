#include "Shader_Defines.hlsli"
#include "Shader_Effect_Defines.hlsli"
//#pragma enable_d3d11_debug_symbols

// float4x4
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor;
float g_fClipRange;
vector g_vStartColor;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
    float4 vFlag : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    float fAlphaMask = 1.f;
    float fColoScaleUV = 1.f;
 
    float fNoise = 1.f;
    float fNoiseAlpha = 1.f;
    float fDistortion = 0.f;
    float fAlphaMaskNoiseUV = 0.f;
    float fBaseNoiseUV = 0.f;
    float fDissolve = 0.f;
    float fClip;
    
    float4 vColor, vNormal, vAlphaMask, vColorScaleUV, vColorScale;
    float4 vNoise, vDissolve, vBaseNoiseUV, vAlphaMaskNoiseUV, vDistortion;
    
    float2 vAlphaRadialUV, vBaseRadialUV, vNoiseRadialUV, vDistortionRadialUV;
    
    if (0 != g_iRadialMappingFlag)
    {
        float2 vCenterUV = mul(In.vTexcoord, 2.f) - float2(1.f, 1.f);
        
        float2 vRadius = length(vCenterUV);
        float fTheta = atan2(vCenterUV.y, vCenterUV.x);
        fTheta /= (2.f * 3.14159265359f) + 0.5f;
        
        
        float2 vRadialUV;
        if (g_iRadialMappingFlag & ALPHARADIAL)
        {
            
            if (g_TextureFlag & ALPHA_NOISE_UV)
            {
                vAlphaMaskNoiseUV = g_AlphaMaskNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vAlphaMaskNoiseUVOffset);
                fAlphaMaskNoiseUV = vAlphaMaskNoiseUV.r;
            }
        
            if (1.f == g_vAlphaRadiusFlag.x)
            {
        
                vRadialUV.x = vRadius + fAlphaMaskNoiseUV; // U : Radius
                vRadialUV.y = fTheta; // V : Theta;
            }
            else if (1.f == g_vAlphaRadiusFlag.y)
            {
                vRadialUV.y = vRadius + fAlphaMaskNoiseUV; // U : Radius
                vRadialUV.x = fTheta; // V : Theta;
            }
            vAlphaRadialUV = vRadialUV;
        }
        
        if (g_iRadialMappingFlag & BASERADIAL)
        {
            
            if (g_TextureFlag & BASE_NOISE_UV)
            {
                vBaseNoiseUV = g_BaseNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vBaseNoiseUVOffset);
                fBaseNoiseUV = vBaseNoiseUV.r;
            }
        
            if (1.f == g_vBaseRadiusFlag.x)
            {
        
                vRadialUV.y = vRadius + fBaseNoiseUV; // U : Radius
                vRadialUV.x = fTheta; // V : Theta;
            }
            else if (1.f == g_vBaseRadiusFlag.y)
            {
                vRadialUV.x = vRadius + fBaseNoiseUV; // U : Radius
                vRadialUV.y = fTheta; // V : Theta;
            }
        
            vBaseRadialUV = vRadialUV;
        }
        
        if (g_iRadialMappingFlag & NOISERADIAL)
        {
        
            if (1.f == g_vNoiseRadiusFlag.x)
            {
        
                vRadialUV.y = vRadius; // U : Radius
                vRadialUV.x = fTheta; // V : Theta;
            }
            else if (1.f == g_vNoiseRadiusFlag.y)
            {
                vRadialUV.x = vRadius; // U : Radius
                vRadialUV.y = fTheta; // V : Theta;
            }
        
            vNoiseRadialUV = vRadialUV;
        }
        
        if (g_iRadialMappingFlag & DISTORTIONRADIAL)
        {

        
            if (1.f == g_vDistortionRadiusFlag.x)
            {
        
                vRadialUV.y = vRadius; // U : Radius
                vRadialUV.x = fTheta; // V : Theta;
            }
            else if (1.f == g_vDistortionRadiusFlag.y)
            {
                vRadialUV.x = vRadius; // U : Radius
                vRadialUV.y = fTheta; // V : Theta;
            }
        
            vDistortionRadialUV = vRadialUV;
        }
    }
    
    if (g_TextureFlag & BASE)
    {
        
        if (true == g_bUseSpriteBase)
        {
            int row = g_iBaseSpriteIndex / g_iBaseCol;
            int col = g_iBaseSpriteIndex % g_iBaseCol;
            
            float2 vBaseUV;
            vBaseUV.x = (col * g_fColBaseUVInterval) + (In.vTexcoord.x * g_fColBaseUVInterval);
            vBaseUV.y = (row * g_fRowBaseUVInterval) + (In.vTexcoord.y * g_fRowBaseUVInterval);
            vBaseUV += g_vBaseUVOffset;
            vColor = 0 == g_iBaseSampler ? g_BaseTexture.Sample(LinearWrapSampler, vBaseUV)
            : g_BaseTexture.Sample(LinearClampSampler, vBaseUV);
        }
        else
        {
            float2 vBaseUV;
            if (g_iRadialMappingFlag & BASERADIAL)
                vBaseUV = vBaseRadialUV;
            else
            {
                if (g_TextureFlag & BASE_NOISE_UV)
                {
                    vBaseNoiseUV = g_BaseNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vBaseNoiseUVOffset);
                    fBaseNoiseUV = vBaseNoiseUV.r;
                
                    if (g_vBaseNoiseInfoFlag.NORMALIZED_NOIOSE)
                        fBaseNoiseUV = fBaseNoiseUV * 2.f - 1.f;
                
                    if (g_vBaseNoiseInfoFlag.OFFSET_NOIOSE)
                    {
                        if (g_vBaseNoiseInfoFlag.ADD_NOISE)
                            vBaseUV = In.vTexcoord + fBaseNoiseUV;
                        else
                            vBaseUV = In.vTexcoord - fBaseNoiseUV;
                    }
                    else
                        vBaseUV = fBaseNoiseUV;

                }
                else
                {
                    vBaseUV = In.vTexcoord;
                }
            }

            vColor = 0 == g_iBaseSampler ? g_BaseTexture.Sample(LinearWrapSampler, vBaseUV + g_vBaseUVOffset)
            : g_BaseTexture.Sample(LinearClampSampler, vBaseUV + g_vBaseUVOffset);
        }
    }
    if (g_TextureFlag & ALPHA) // Mask
    {
        
        if (true == g_bUseSpriteMask)
        {
            int row = g_iMaskSpriteIndex / g_iMaskCol;
            int col = g_iMaskSpriteIndex % g_iMaskCol;
            
            float2 vMaskUV;
            vMaskUV.x = (col * g_fColMaskUVInterval) + (In.vTexcoord.x * g_fColMaskUVInterval);
            vMaskUV.y = (row * g_fRowMaskUVInterval) + (In.vTexcoord.y * g_fRowMaskUVInterval);
            vMaskUV += g_vAlphaMaskUVOffset;
            vAlphaMask = 0 == g_iAlphaMaskSampler ? g_AlphaMask.Sample(LinearWrapSampler, vMaskUV)
            : g_AlphaMask.Sample(LinearClampSampler, vMaskUV);
        }
        else
        {
            float2 vMaskUV;
        
            if (g_iRadialMappingFlag & ALPHARADIAL)
                vMaskUV = vAlphaRadialUV;
            else
            {
                if (g_TextureFlag & ALPHA_NOISE_UV)
                {
                    vAlphaMaskNoiseUV = g_AlphaMaskNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vAlphaMaskNoiseUVOffset);
                    fAlphaMaskNoiseUV = vAlphaMaskNoiseUV.r;
                
                    if (g_vAlphaNoiseInfoFlag.NORMALIZED_NOIOSE)
                        fAlphaMaskNoiseUV = fAlphaMaskNoiseUV * 2.f - 1.f;
                
                    if (g_vAlphaNoiseInfoFlag.OFFSET_NOIOSE)
                    {
                        if (g_vAlphaNoiseInfoFlag.ADD_NOISE)
                            vMaskUV = In.vTexcoord + fAlphaMaskNoiseUV;
                        else
                            vMaskUV = In.vTexcoord - fAlphaMaskNoiseUV;
                    }
                    else
                        vMaskUV = fAlphaMaskNoiseUV;

                }
                else
                {
                    vMaskUV = In.vTexcoord;
                }
                
            }
        
            vAlphaMask = 0 == g_iAlphaMaskSampler ? g_AlphaMask.Sample(LinearWrapSampler, vMaskUV + g_vAlphaMaskUVOffset)
                : g_AlphaMask.Sample(LinearClampSampler, vMaskUV + g_vAlphaMaskUVOffset);
        
        }
        fAlphaMask = vAlphaMask.r;
    }
    if (g_TextureFlag & COLORSCALE_UV)
    {
        float2 vUV = In.vTexcoord + g_vColorScaleUVOffset;
        vColorScaleUV = 0 == g_iColorScaleSampler ? g_ColorScaleUV.Sample(LinearWrapSampler, vUV)
            : g_ColorScaleUV.Sample(LinearClampSampler, vUV);
        
        fColoScaleUV = vColorScaleUV.r;
    }
    if (g_TextureFlag & NOISE)    // Noise
    {
        float2 vNoiseUV;
        if (g_iRadialMappingFlag & NOISERADIAL)
            vNoiseUV = vNoiseRadialUV;
        else
            vNoiseUV = In.vTexcoord;
        
        vNoise = 0 == g_iNoiseSampler ? g_NoiseTexture.Sample(LinearWrapSampler, vNoiseUV + g_vNoiseUVOffset)
            : g_NoiseTexture.Sample(LinearClampSampler, vNoiseUV + g_vNoiseUVOffset);
        
        fNoise = vNoise.r;
        
        if (true == g_bNoiseMultiplyAlpha)
            fNoiseAlpha = fNoise;
    }
    if (g_TextureFlag & DISSOLVETEX)
    {
        float2 vDissolveUV = In.vTexcoord + g_vDissolveUVOffset;
        vDissolve = 0 == g_iDissolveSampler ? g_DissolveTexture.Sample(LinearWrapSampler, vDissolveUV)
            : g_DissolveTexture.Sample(LinearClampSampler, vDissolveUV);
        fDissolve = vDissolve.r;
        
        fClip = fDissolve - g_fDissolveAmount;
    
        clip(fClip - 0.001f);
    }
    if (g_TextureFlag & COLORSCALE)    // ColorScale
    {
        vColorScale = g_ColorScaleTexture.Sample(LinearWrapSampler, fColoScaleUV);
    }
    else
        vColorScale = float4(1.f, 1.f, 1.f, 1.f);
    
    if (g_TextureFlag & DISTORTIONTEX)
    {
        float2 vUV;
        if (g_iRadialMappingFlag & DISTORTIONRADIAL)
            vUV = vDistortionRadialUV;
        else
            vUV = In.vTexcoord;
        
        vDistortion = 0 == g_iDistortionSampler ? g_DistortionTexture.Sample(LinearWrapSampler, vUV + g_vDistortionUVOffset)
            : g_DistortionTexture.Sample(LinearClampSampler, vUV + g_vDistortionUVOffset);
        
        fDistortion = vDistortion.r;
    }
    
    if (g_TextureFlag & UV_SLICE_CLIP)
    {
       
        float2 vUV = In.vTexcoord + g_vUVSliceClipOffset;
        float4 vSlice = 0 == g_iUVSliceClipSampler ? g_UVSliceClipTexture.Sample(LinearWrapSampler, vUV)
            : g_UVSliceClipTexture.Sample(LinearClampSampler, vUV);
        
        if (true == g_bUseGreaterThan_U)
        {
                // g_fGreaterValue  <= Texcoord -> return 1
            int iResult = step(g_fGreaterCmpValue_U, In.vTexcoord.x);
            
            clip(vSlice.r - iResult * g_fClipFactor);
        }
            
        if (g_bUseGreaterThan_V)
        {
            int iResult = step(g_fGreaterCmpValue_V, In.vTexcoord.y);
            
            clip(vSlice.r - iResult * g_fClipFactor);
        }
            
        if (true == g_bUseLessthan_U)
        {
            // Texcoord <= g_fLessCmpValue  -> 1
            int iResult = step(In.vTexcoord.x, g_fLessCmpValue_U);
            
            clip(vSlice.r - iResult * g_fClipFactor);
                
        }
            
        if (true == g_bUseLessthan_V)
        {
            int iResult = step(In.vTexcoord.y, g_fLessCmpValue_V);
            
            clip(vSlice.r - iResult * g_fClipFactor);
                
        }
    }
    
   
    if (g_bColorLerp)
    {
        Out.vColor.rgb = mul(vColor.rgb * g_vLerpColor.rgb * g_vMulColor.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        Out.vColor.a = vColor.a * fAlphaMask * g_vLerpColor.a * g_vMulColor.a * vColorScale.a * fNoiseAlpha + g_vAddColor.a;
    }
    else
    {
        if (0.f != g_vStartColor.r || 0.f != g_vStartColor.g || 0.f != g_vStartColor.b)
            Out.vColor.rgb = mul(vColor.rgb * g_vStartColor.rgb * g_vMulColor.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        else
            Out.vColor.rgb = mul(vColor.rgb * g_vMulColor.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        
        if (0.f != g_vStartColor.a)
            Out.vColor.a = vColor.a *  fAlphaMask * g_vStartColor.a * g_vMulColor.a * vColorScale.a * fNoiseAlpha + g_vAddColor.a;
        else
            Out.vColor.a = vColor.a * fAlphaMask * g_vMulColor.a * vColorScale.a * fNoiseAlpha + g_vAddColor.a;
    }
    
    
    
    if (Out.vColor.r <= g_vColor_Clip.r && Out.vColor.g <= g_vColor_Clip.g && Out.vColor.b <= g_vColor_Clip.b)
    {
        discard;
    }
    
    if (Out.vColor.a <= g_vColor_Clip.a)
    {
        discard;
    }
    
    
    if (g_bDistortion)
    {
        if (g_TextureFlag & DISTORTIONTEX)
            Out.vDistortion = fAlphaMask * fDistortion * g_fDistortionIntensity;
        else
            Out.vDistortion = fAlphaMask * fNoise * g_fDistortionIntensity;
    }
    
    if (true == g_bDissolveEdge)
    {
        float fEdgeWidth = g_fDissolveEdgeWidth;
        float4 vDissolveColor = g_vDissolveDiffuse;
        
        if (fClip < fEdgeWidth)
        {
            float fEdgeFactor = smoothstep(0.f, fEdgeWidth, fClip);
            Out.vColor = lerp(vDissolveColor, Out.vColor, fEdgeFactor);
        }
    }
    
    Out.vFlag.w = 1.f;
    
    return Out;
}


technique11 DefaultTechinque
{
    pass Default // 0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass AlphaTest // 1
    {
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }


    pass One // 2
    {
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

}


#include "Shader_Defines.hlsli"
#include "Shader_PostProcess_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_OriginTexture;

// BLOOM ///////////////////
Texture2D g_BloomTexture;

// DISTORTION///////////////
Texture2D g_DistortionTexture;
///////////////////////////

/////FINAL_SLASH_FLAG/////////////////
Texture2D g_FinalSlashFlagTexture;
///////////////////////////////////

Texture2D g_FinalTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = float4(In.vPosition, 1.f);
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
};

PS_OUT PS_MAIN_COPY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vCopy = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor = vCopy;
    
    return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float  fDistortion = g_DistortionTexture.Sample(LinearClampSampler, In.vTexcoord).r;
    
    Out.vColor = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord + float2(fDistortion, fDistortion));
  
    return Out;
}

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vOrigin = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vBloom = g_BloomTexture.Sample(LinearClampSampler, In.vTexcoord);
  
    float3 vCombined = vOrigin.rgb + g_fBloomStrength * vBloom.rgb;
    
    Out.vColor = float4(ACESFilmTonemap(vCombined.rgb), 1.f);
    //float4(lumaBasedReinhardToneMapping(vCombined.rgb), 1.f);
    //float4(LinearToneMapping(vCombined.rgb), 1.f);
    //float4(ReinhardToneMapping(vCombined.rgb), 1.f);
   
    return Out;
}


PS_OUT PS_MAIN_RADIALBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    float2 vDist = In.vTexcoord - g_vBlurCenter;
    for (int i = 0; i < g_iSampleCount; ++i)
    {
        float fScale = 1.f - g_fBlurStrength * (i / (float)g_iSampleCount) * saturate(length(vDist) / g_fBlurRadius);
        vColor += g_OriginTexture.Sample(LinearClampSampler, vDist * fScale + g_vBlurCenter);
    }
    vColor /= (float) g_iSampleCount;
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_MAIN_GLITCH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fEnableShift = Get_GlitchRandom(floor(g_fTime * g_fGlitchSpeed)) < g_fGlitchRate ? 1.0f : 0.0f;
    
    float2 vFixedUV = In.vTexcoord;
    
    vFixedUV.x += (Get_GlitchRandom((floor(In.vTexcoord.y * g_fGlitchBlockSize) / g_fGlitchBlockSize) + g_fTime) - 0.5f) * g_fGlitchPower * fEnableShift;

    float4 vOriginColor = g_OriginTexture.Sample(LinearClampSampler, vFixedUV);

    float fColor_R = lerp(vOriginColor.r, g_OriginTexture.Sample(LinearClampSampler, vFixedUV + float2(g_fGlitchColorRate, 0.0f)).r, fEnableShift);
    float fColor_B = lerp(vOriginColor.b, g_OriginTexture.Sample(LinearClampSampler, vFixedUV + float2(-g_fGlitchColorRate, 0.0f)).b, fEnableShift);

    Out.vColor = float4(fColor_R, vOriginColor.g, fColor_B, 1.f);
    
    return Out;
}

float4 PS_MAIN_FINALSLASH(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    float4 vOrigin = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord);
    float fFlag = g_FinalSlashFlagTexture.Sample(LinearClampSampler, In.vTexcoord).a;
    
    if (0.f == fFlag)
        return vColor;
    
    float3 vBrightness = (vOrigin.r + vOrigin.g + vOrigin.b) / 3.f;
    
    vColor.r = vBrightness;
    vColor.a = 1.f;
    
    return vColor;
}

float4 PS_MAIN_MOZAIC(PS_IN In) : SV_TARGET0
{
    float2 vResolution = float2(1280.f, 720.f);
    
    // Get screen space coordinates
    float2 p = floor(In.vTexcoord * vResolution / g_fMozaicTileSize) * g_fMozaicTileSize;

    // Define offsets for sampling
    float2 quat_x = float2(g_fMozaicTileSize / 4.0, 0);
    float2 quat_y = float2(0, g_fMozaicTileSize / 4.0);

    // Accumulate color values from surrounding texels
    float3 color = (
        g_OriginTexture.Load(int3(p, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_x, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_x * 2.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_x * 3.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y + quat_x, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y + quat_x * 2.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y + quat_x * 3.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 2.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 2.0 + quat_x, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 2.0 + quat_x * 2.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 2.0 + quat_x * 3.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 3.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 3.0 + quat_x, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 3.0 + quat_x * 2.0, 0)).xyz
        + g_OriginTexture.Load(int3(p + quat_y * 3.0 + quat_x * 3.0, 0)).xyz
    ) / 16.0;

 
    return float4(color, 1.f);
}


float4 PS_MAIN_COLOR_INVERSION(PS_IN In) : SV_TARGET0
{
    float4 vColor = 0.f;
    
    float4 vOriginColor = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vInverseColor = 1.f - vOriginColor;
   
    float brightness = dot(vInverseColor.xyz, float3(0.299, 0.587, 0.114));
    
    float3 darkColor = float3(0.0, 0.0, 0.0); // 어두운 영역 - 검정색
    float3 redColor = float3(1.0, 0.0, 0.0); // 중간 밝기 - 빨간색
    float3 cyanColor = float3(0.0, 1.0, 1.0); // 밝은 영역 - 청록색
    
    float lowThreshold = 0.3; // 어두운 부분 기준 값
    float highThreshold = 0.6; // 밝은 부분 기준 값

    // 밝기에 따라 색상 결정
    float3 finalColor;
    float darkToRed = smoothstep(lowThreshold - 0.1, lowThreshold + 0.1, brightness); // 어두운 -> 빨강
    float redToCyan = smoothstep(highThreshold - 0.1, highThreshold + 0.1, brightness); // 빨강 -> 청록

    // 색상 혼합
    finalColor = lerp(darkColor, redColor, darkToRed); // 검정과 빨간색 보간
    finalColor = lerp(finalColor, cyanColor, redToCyan); // 빨간색과 청록색 보간

    return float4(finalColor, 1.f);
}


float4 PS_MAIN_FINAL(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
   vColor = g_FinalTexture.Sample(LinearClampSampler, In.vTexcoord);
 
    return vColor;
}

technique11 DefaultTechnique
{
    pass Copy // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPY();
    }

    pass Bloom // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }

    pass RadialBlur // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RADIALBLUR();
    }


    pass Final // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass Distortion // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }

    pass Hit_Glitch // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLITCH();
    }

    pass Final_Slash // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINALSLASH();
    }

    pass Mozaic // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_RenderOnlyStencilEqual, 1);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOZAIC();
    }

    pass AllBlack_Without_Travis // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_RenderOnlyStencilEqual, 1);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COPY();
    }



    pass Color_Inversion // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR_INVERSION();
    }

}


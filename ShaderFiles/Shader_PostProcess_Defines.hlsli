#ifndef SHADER_FUNCTION
#define SHADER_FUNCTION

float g_fBloomStrength = 0.7f;
float g_fGamma = 2.2f;
float g_fExposure = 1.f;

// RADIAL_BLUR/////////////////////
int     g_iSampleCount = 16;
float   g_fBlurStrength;
float2  g_vBlurCenter;
float   g_fBlurRadius;
///////////////////////////////////

// COLOR_INVERSION/////////////////////
float   g_fInversionRatio = 1.f;

/////////////////////////////////////

float3 LinearToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / g_fGamma;

    color = clamp(g_fExposure * color, 0.f, 1.f);
    color = pow(color, invGamma);
    return color;
}

float3 FilmicToneMapping(float3 color)
{
    color = max(float3(0, 0, 0), color);
    color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
    return pow(color, 1.f / g_fGamma);
}

float3 Uncharted2ToneMapping(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, float3(1.0, 1.0, 1.0) / g_fGamma);
    return color;
}

float3 ACESFilmTonemap(float3 color)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;

    color = saturate((color * (a * color + b)) / (color * (c * color + d) + e));
    return pow(color, 1.f / g_fGamma); //color; 
}

float3 lumaBasedReinhardToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / g_fGamma;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1. + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, invGamma);
    return color;
}


float3 ReinhardToneMapping(float3 vColor)
{
    float k = 1.0f;

    vColor.xyz = vColor.xyz / (vColor.xyz + k);

    vColor = pow(vColor, 1 / 2.2f);

    return vColor;
    
}


///HIT_GLITCH//////////////////
float g_fGlitchPower = 0.05f;
float g_fGlitchRate = 0.2f;
float g_fGlitchSpeed = 5.0f;
float g_fGlitchBlockSize = 150.5f;
float g_fGlitchColorRate = 0.01f;
float g_fTime;

float Get_GlitchRandom(float fSeed)
{
    return frac(543.2543 * sin(dot(float2(fSeed, fSeed), float2(3525.46, -54.3415))));
}

//////////////////////////////



////////////////Mozaic
float g_fMozaicTileSize = 40.f;
/////////////////////////////////


DepthStencilState DSS_RenderOnlyStencilEqual
{
    DepthEnable = false; 
    DepthWriteMask = zero;

    StencilEnable = true;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFunc = equal; 
    FrontFaceStencilFail = keep; 
    FrontFaceStencilDepthFail = keep;
    FrontFaceStencilPass = keep; 
};

#endif
#ifndef SHADER_EFFECT_DEFINE
#define SHADER_EFFECT_DEFINE
#include "Shader_Defines.hlsli"

#define BASE            (1<<1)
#define NORMALTEX       (1<<2)
#define ALPHA           (1<<3)
#define COLORSCALE_UV   (1<<4)
#define NOISE           (1<<5)
#define COLORSCALE      (1<<6)
#define DISTORTIONTEX   (1<<7)
#define DISSOLVETEX     (1<<8)
#define BASE_NOISE_UV   (1<<9)
#define ALPHA_NOISE_UV  (1<<10)
#define UV_SLICE_CLIP   (1<<11)

#define BASERADIAL          (2<<1)
#define ALPHARADIAL         (2<<2)
#define NOISERADIAL         (2<<3)
#define DISTORTIONRADIAL    (2<<4)

#define NORMALIZED_NOIOSE x
#define OFFSET_NOIOSE y
#define ADD_NOISE z

int g_TextureFlag;

Texture2D g_BaseTexture;
Texture2D g_NormalTexture;
Texture2D g_AlphaMask;
Texture2D g_ColorScaleUV;
Texture2D g_NoiseTexture;
Texture2D g_ColorScaleTexture;
Texture2D g_DistortionTexture;
Texture2D g_DissolveTexture;
Texture2D g_BaseNoiseUVTexture;
Texture2D g_AlphaMaskNoiseUVTexture;
Texture2D g_UVSliceClipTexture;

float3 g_vBaseNoiseInfoFlag;
float3 g_vAlphaNoiseInfoFlag;

float2 g_vBaseRadiusFlag;
float2 g_vAlphaRadiusFlag;
float2 g_vNoiseRadiusFlag;
float2 g_vDistortionRadiusFlag;

int g_iRadialMappingFlag;

bool g_bColorLerp;
bool g_bColorMask;

float4 g_vAddColor;
float4 g_vMulColor;
float4 g_vLerpColor;
float4 g_vColor_Clip;

// UV OFFSET
float2  g_vBaseUVOffset;
int     g_iBaseSampler;

float2  g_vAlphaMaskUVOffset;
int     g_iAlphaMaskSampler;

float2  g_vColorScaleUVOffset;
int     g_iColorScaleSampler;

float2  g_vNoiseUVOffset;
int     g_iNoiseSampler;

float2  g_vDistortionUVOffset;
int     g_iDistortionSampler;

float2  g_vDissolveUVOffset;
int     g_iDissolveSampler;

float2  g_vBaseNoiseUVOffset;
int     g_iBaseNoiseSampler;

float2  g_vAlphaMaskNoiseUVOffset;
int     g_iAlphaMaskNoiseSampler;

float2  g_vUVSliceClipOffset;
int     g_iUVSliceClipSampler;

float   g_fDissolveAmount;
bool    g_bDistortion;
float   g_fDistortionIntensity;

bool    g_bUseSpriteBase;
int     g_iBaseCol;
float   g_fRowBaseUVInterval;
float   g_fColBaseUVInterval;
int     g_iBaseSpriteIndex;

bool    g_bUseSpriteMask;
int     g_iMaskCol;
float   g_fRowMaskUVInterval;
float   g_fColMaskUVInterval;
int     g_iMaskSpriteIndex;

int     g_iTransformType;
bool    g_bBillboardOnlyLook;

bool g_bUseGreaterThan_U;
bool g_bUseGreaterThan_V;
bool g_bUseLessthan_U;
bool g_bUseLessthan_V;

float g_fGreaterCmpValue_U;
float g_fGreaterCmpValue_V;
float g_fLessCmpValue_U;
float g_fLessCmpValue_V;
float g_fClipFactor;

bool g_bNoiseMultiplyAlpha;

float4 g_vDissolveDiffuse;
bool g_bDissolveEdge;

float g_fDissolveEdgeWidth;

float Hash(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.223))) * 43758.5453);
}

float2 RansdomUV(float2 vUV)
{
    float fRandValue = Hash(vUV);
    return frac(vUV + fRandValue);
}

float4x4 CreateAxisRotMat(float4 vAxis, float fTheta)
{
    float fCosTheta = cos(fTheta);
    float fsinTheta = sin(fTheta);
    float oneMinusCosTheta = 1.0f - fsinTheta;

    vAxis = normalize(vAxis);
    
    float x = vAxis.x;
    float y = vAxis.y;
    float z = vAxis.z;
    
    return float4x4(
        fCosTheta + oneMinusCosTheta * x * x,
        oneMinusCosTheta * x * y - fsinTheta * z,
        oneMinusCosTheta * x * z + fsinTheta * y,
        0.0f,

        oneMinusCosTheta * y * x + fsinTheta * z,
        fCosTheta + oneMinusCosTheta * y * y,
        oneMinusCosTheta * y * z - fsinTheta * x,
        0.0f,

        oneMinusCosTheta * z * x - fsinTheta * y,
        oneMinusCosTheta * z * y + fsinTheta * x,
        fCosTheta + oneMinusCosTheta * z * z,
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f
    );
}

#endif

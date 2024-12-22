#ifndef SHADER_DEFINE_PBR
#define SHADER_DEFINE_PBR

#include "Shader_Defines.hlsli"

TextureCube g_SpecularIBLTexture;
TextureCube g_IrradianceIBLTexture;
Texture2D   g_BRDFTexture;


static const float3 Fdielectric = 0.04f; // 비금속(Dielectric) 재질의 F0

float3 SchlickFresnel(float3 F0, float NdotH)
{
    return F0 + (1.f - F0) * pow(2, (-5.55473 * NdotH - 6.98316) * NdotH);
}

float NdfGGX(float NdotH, float fRoughness)
{
    NdotH = clamp(NdotH, 0.f, 0.99f);
    float fAlpha = fRoughness * fRoughness;
    float fAlphaSq = fAlpha * fAlpha;
    float fDenom = (NdotH * NdotH) * (fAlphaSq - 1.f) + 1.f;
    fDenom = 3.141592f * fDenom * fDenom;
    
    return fAlphaSq / fDenom;
}

float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.f - k) + k);
}

float SchlickGGX(float NdotI, float NdotO, float fRoughness)
{
    float r = fRoughness + 1.f;
    float k = (r * r) / 8.f;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}


float3 DiffuseIBL(float3 vAlbedo, float3 vNormal, float3 vToEye, float fMetallic)
{
    float3 F0 = lerp(Fdielectric, vAlbedo, fMetallic);
    float3 F = SchlickFresnel(F0, max(0.f, dot(vNormal, vToEye)));
    float3 kd = lerp(1.f - F, 0.f, fMetallic);
    float3 vIrradiance = g_IrradianceIBLTexture.Sample(LinearClampSampler, vNormal).rgb;
    
    return kd * vAlbedo * vIrradiance;
}

float3 SpecularIBL(float3 vAlbedo, float3 vNormal, float3 vToEye, float fMetallic, float fRoughness)
{
    float2 specularBRDF = g_BRDFTexture.Sample(LinearClampSampler, float2(dot(vNormal, vToEye), 1.f - fRoughness)).rg;
    float3 specularIrradiance = g_SpecularIBLTexture.SampleLevel(LinearClampSampler, reflect(-vToEye, vNormal),
                                                            3.f + fRoughness * 5.f).rgb;
    const float3 Fdielectric = 0.04; 
    float3 F0 = lerp(Fdielectric, vAlbedo, fMetallic);

    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 vAlbedo, float3 vNormal, float3 vToEye,
                            float fMetallic, float fRoughness)
{
    float3 diffuseIBL = DiffuseIBL(vAlbedo, vNormal, vToEye, fMetallic);
    float3 specularIBL = SpecularIBL(vAlbedo, vNormal, vToEye, fMetallic, fRoughness);
    
    return (diffuseIBL + specularIBL);
}

#endif



#include "Shader_Defines.hlsli"


const float g_fSampleRadius = 0.5f;
const int g_iKernelSize = 64;
const float g_fBias = 0.025f;
float2 g_vNoiseScale = float2(1280.f / 4.f, 720.f / 4.f);
float3 g_vSampleKernel[64];
float g_fSSAOPower = 1.f;

Texture2D g_SSAONoiseTexture;

float3 g_vRandom[16] =
{
    float3(0.2024537f, 0.841204f, -0.9060141f),
    float3(-0.2200423f, 0.6282339f, -0.8275437f),
    float3(0.3677659f, 0.1086345f, -0.4466777f),
    float3(0.8775856f, 0.4617546f, -0.6427765f),
    float3(0.7867433f, -0.141479f, -0.1567597f),
    float3(0.4839356f, -0.8253108f, -0.1563844f),
    float3(0.4401554f, -0.4228428f, -0.3300118f),
    float3(0.0019193f, -0.8048455f, 0.0726584f),
    float3(-0.7578573f, -0.5583301f, 0.2347527f),
    float3(-0.4540417f, -0.252365f, 0.0694318f),
    float3(-0.0483353f, -0.2527294f, 0.5924745f),
    float3(-0.4192392f, 0.2084218f, -0.3672943f),
    float3(-0.8433938f, 0.1451271f, 0.2202872f),
    float3(-0.4037157f, -0.8263387f, 0.4698132f),
    float3(-0.6657394f, 0.6298575f, 0.6342437f),
    float3(-0.0001783f, 0.2834622f, 0.8343929f),
};

float3 RandomNormal(float2 tex)
{
    float noiseX = (frac(sin(dot(tex, float2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f));
    float noiseY = (frac(sin(dot(tex, float2(11.9899f, 62.223f) * 2.0f)) * 34748.34744f));
    float noiseZ = (frac(sin(dot(tex, float2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f));
    return normalize(float3(noiseX, noiseY, noiseZ));
}

struct SSAO_IN
{
    float2 vUV;
    float fDepth;
    float fViewZ;
    float3 vNormalWorld;
};



//float4 Calc_SSAO(SSAO_IN In)
//{
//    float4 vAO = (float4) 0;
//    
//    float3 vRay;
//    float3 vReflect;
//    float2 vRandomUV;
//    float fOccNormal;
// 
//    int iColor = 0;
// 
//    for (int i = 0; i < 16; ++i)
//    {
//        vRay = reflect(RandomNormal(In.vUV), g_vRandom[i]);
//        vReflect = normalize(reflect(vRay, In.vNormalWorld)) * g_fSampleRadius;
//        vReflect.x *= -1.f;
//        vRandomUV = In.vUV + vReflect.xy;
//        fOccNormal = g_DepthTexture.Sample(LinearClampSampler, vRandomUV).x * g_fFar * In.fViewZ;
// 
//        if (fOccNormal <= In.fDepth + 0.0003f)
//            ++iColor;
//    }
//    
//    vAO = abs((iColor / 16.f) - 1.f);
//    return vAO;
//}
//
//float4 PS_MAIN_SSAO(PS_IN In) : SV_TARGET0
//{
//    float4 vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
//    float4 vNormalDesc = g_NormalTexture.Sample(LinearClampSampler, In.vTexcoord);
//    
//    if (vNormalDesc.a != 0.f)
//        return float4(1.f, 1.f, 1.f, 1.f);
//    
//    float4 vNormal = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f));
//    
//    SSAO_IN SSAO_In = (SSAO_IN) 0;
//    SSAO_In.vUV = In.vTexcoord;
//    SSAO_In.vNormalWorld = vNormal.rgb;
//    SSAO_In.fViewZ = vDepthDesc.y * g_fFar;
//    SSAO_In.fDepth = vDepthDesc.x * g_fFar * SSAO_In.fViewZ;
//   
//    return 1.f - Calc_SSAO(SSAO_In);
//}



    
//float fStep = 0.5f;
//    
//matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
//    
//float fPixelDepth = 0.f;
//int iStepDistance = 0;
//float2 vRayUV = (float2) 0;
//    
//    [loop]
//    for (iStepDistance = 1; iStepDistance < 60; ++iStepDistance)
//    {
//vector vDirStep = vRayDir * fStep * iStepDistance;
//        vDirStep.w = 0.f;
//vector vRayWorldPos = vRayOrigin + vDirStep;
//
//vector vRayProjPos = mul(vRayWorldPos, matVP);
//        vRayProjPos /= vRayProjPos.
//w;
//      
//        vRayUV = float2(vRayProjPos.x * 0.5f + 0.5f, vRayProjPos.y * -0.5f + 0.5f);
//        
//        clip(vRayUV);
//        clip(1.f - vRayUV);
//        
//        fPixelDepth = g_DepthTexture.Sample(LinearClampSampler, vRayUV).
//r;
//      
//float fDiff = vRayProjPos.z - fPixelDepth;
//        
//        if (fDiff > 0.f)
//        {
//float fStart = (iStepDistance - 1) * fStep;
//float fEnd = iStepDistance * fStep;
//float fMid = 0.f;
//
//          // ÀÌºÐ Å½»ö
//            for (
//int i = 0;i < 5; ++i)
//            {
//                fMid = (fStart + fEnd) * 0.5f;
//                
//vector vMidDirStep = vRayDir * fMid;
//                vMidDirStep.w = 0.f;
//vector vMidRayWorldPos = vRayOrigin + vMidDirStep;
//
//                vRayProjPos = mul(vRayWorldPos, matVP);
//                
//                vRayUV = float2(vRayProjPos.x * 0.5f + 0.5f, vRayProjPos.y * -0.5f + 0.5f);
//                
//                fPixelDepth = g_DepthTexture.Sample(LinearClampSampler, vRayUV).
//r;
//
//                fDiff = vRayProjPos.z - fPixelDepth;
//
//                if (fDiff > 0.f)
//                {
//                    fEnd =
//fMid;
//                }
//                else
//                {
//                    fStart =
//fMid;
//                }
//            }
//            break;
//        }
//    }
//   
//    clip(59.5f - iStepDistance);
//
//    vColor = g_OriginTexture.Sample(LinearClampSampler, vRayUV);
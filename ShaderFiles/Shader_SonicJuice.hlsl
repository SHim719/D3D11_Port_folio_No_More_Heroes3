#include "Shader_Defines.hlsli"
#include "Shader_Defines_PBR.hlsli"
#include "Shader_PostProcess_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_BoneMatrices[256];

Texture2D   g_DiffuseTexture;
Texture2D   g_NormalTexture;
Texture2D   g_MRAOTexture;
Texture2D   g_FrenselNormalTexture;
Texture2D   g_GlassMaskTexture;
Texture2D   g_OriginTexture;
TextureCube g_EnvTexture;

float4      g_vCamPosition;
float4      g_vRimColor;
float       g_fRimPower;

int         g_iTextureFlag;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

    vector vBonePosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    vector vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);
    vector vPosition = mul(vBonePosition, matWVP);

    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix).xyz);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix).xyz);
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vBonePosition, g_WorldMatrix);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};


float4 PS_MAIN_SONICJUICE(PS_IN In) : SV_TARGET0
{
    float4 vDiffuse = pow(g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV), 2.2f);
    float3 vNormal;
    float fGlassMask = g_GlassMaskTexture.Sample(LinearWrapSampler, In.vTexUV);
    
    if (g_iTextureFlag & (1 << NORMALS))
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexUV);
        vNormal = Decode_NormalMap(vNormalDesc);
    
        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
        vNormal = mul(vNormal, WorldMatrix);
    }
    else
    {
        vNormal = In.vNormal;
    }
    
    vNormal = normalize(vNormal);
   
    float4 vFrenselNormalDesc = g_FrenselNormalTexture.Sample(LinearWrapSampler, In.vTexUV);
    float3 vFrenselNormal = Decode_NormalMap(vFrenselNormalDesc);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    vFrenselNormal = mul(vFrenselNormal, WorldMatrix);
    vFrenselNormal = normalize(vFrenselNormal);
    
    
    float3 vToEye = normalize(g_vCamPosition - In.vWorldPos);
    
    float fFresnelFactor = pow(1.0 - dot(vFrenselNormal, vToEye), 5.f);
    float3 vFrensel = float3(0.f, 0.739295f, 1.f) * fFresnelFactor * 0.1f;
     
    float fMetalic = 0.6f;
    float fRoughness = 0.2f;
    float fOpacity = 0.9f;
    
    float3 vReflectVec = normalize(reflect(-vToEye, vNormal));
    float fEnvLOD = fRoughness * 8.f;
    float3 vEnvColor = g_EnvTexture.SampleLevel(LinearWrapSampler, vReflectVec, fEnvLOD).rgb;
    

    Matrix matVP = g_ViewMatrix * g_ProjMatrix;
    float3 vRefract = normalize(refract(-vToEye, vNormal, 1.f / 2.f));
    float3 vRefractionWorldPos = In.vWorldPos.xyz + vRefract;
    float4 vRefractionProjPos = mul(float4(vRefractionWorldPos, 1.f), matVP);
    float2 vUV = vRefractionProjPos.xy / vRefractionProjPos.w;
    vUV.x = vUV.x * 0.5f + 0.5f;
    vUV.y = -vUV.y * 0.5f + 0.5f;
    
    float4 vRefractColor = g_OriginTexture.Sample(LinearClampSampler, vUV);
   
    float fEffectStrength = fGlassMask * fOpacity * 0.5f;
   
    float4 vFinalColor = float4(0, 0, 0, fOpacity);
    
    vFinalColor.rgb = lerp(vDiffuse.rgb, vRefractColor.rgb, fEffectStrength);
    
    float3 Cubemap_Color = float3(0.301961f, 1.f, 0.6f);
    float3 vEnvContribution = vEnvColor * Cubemap_Color.rgb * 1.5f;
    float fTotalReflect = saturate(fMetalic + fFresnelFactor) * fGlassMask;
    
    float fSmoothness = 1.f - fRoughness;
    vEnvContribution *= fSmoothness;
    
    vFinalColor.rgb = lerp(vFinalColor.rgb, vEnvContribution, fTotalReflect);
    vFinalColor.rgb += vFrensel;
    
    vFinalColor.rgb = lerp(vFinalColor.rgb, vFinalColor.rgb * fMetalic * 1.5f, fMetalic * fGlassMask);
    
    return vFinalColor;
}


technique11 DefaultTechinque
{
    pass SonicJuice // 0
    {
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SONICJUICE();
        ComputeShader = NULL;
    }
	
}


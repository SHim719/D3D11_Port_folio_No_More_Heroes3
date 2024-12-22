#include "Shader_Defines.hlsli"
#include "Shader_Defines_PBR.hlsli"
#include "Shader_PostProcess_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_BoneMatrices[256];

float       g_fAlpha;

Texture2D   g_DiffuseTexture;
Texture2D   g_NormalTexture;
Texture2D   g_MRAOTexture;
Texture2D   g_MRMTexture;
Texture2D   g_OpacityTexture;
Texture2D   g_EmissiveTexture;

Texture2D   g_DissolveTexture;
float       g_fDissolveAmount;
float4      g_vDissolveOrigin;
float4      g_vDissolveDirection;
float4      g_vDissolveDiffuse; // Add by BH

float4      g_vCamPosition;
float4      g_vRimColor;
float       g_fRimPower;

float       g_fFar = 1000.f;

int         g_iTextureFlag;
int         g_iFlag;

float4x4    g_CascadeViewProj[3];
matrix      g_ShadowViewProj;

float4      g_vMotionTrailColor;

float       g_fDistortionStrength;

float4      g_vLightDir;
float4      g_vLightDiffuse;
float       g_fLightStrength;



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
    float4 vProjPos : TEXCOORD2;
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
    //Out.vLocalPos = float4(In.vPosition, 1.f);
    Out.vWorldPos = mul(vBonePosition, g_WorldMatrix);
    Out.vProjPos = vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_TARGET3;
    float4 vRimLight : SV_TARGET4;
    float4 vEmissive : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
    clip(Out.vDiffuse.a - 0.1f);
        
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

    if (g_iTextureFlag & (1 << NORMALS))
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexUV);
        float3 vNormal = Decode_NormalMap(vNormalDesc);
    
        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
        vNormal = mul(vNormal, WorldMatrix);
        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
    }
    
    Out.vORM = float4(1.f, 0.5f, 0.f, 1.f);
    
    
    if (g_iTextureFlag & (1 << MRAO))
    {
       float4 vMRAO = g_MRAOTexture.Sample(LinearWrapSampler, In.vTexUV);
       Out.vORM = float4(vMRAO.z, vMRAO.y, vMRAO.x, 1.f);
    }    
        
    if (g_iTextureFlag & (1 << EMISSIVE))
    {
        Out.vEmissive = g_EmissiveTexture.Sample(LinearWrapSampler, In.vTexUV);
        Out.vEmissive.w = 1.f;
    }
    
    Out.vRimLight = g_vRimColor;
    Out.vRimLight.a = 1.f;
    
    if (g_iFlag & (1 << FLAG_REFLECTION))
        Out.vDepth.w = 1.f;
    
    return Out;
}

PS_OUT PS_MAIN_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
    Out.vDiffuse.a *= g_fAlpha;
   

    return Out;
}


PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

    float fDissolve = g_DissolveTexture.Sample(LinearWrapSampler, In.vTexUV).r;
    float fClip = fDissolve - g_fDissolveAmount;
    
    float fEdgeWidth = 0.05f;
	
    float4 vDissolveColor = g_vDissolveDiffuse;
    
    clip(fClip - 0.001f);

    if (fClip < fEdgeWidth)
    {
        float fEdgeFactor = smoothstep(0.f, fEdgeWidth, fClip);
        Out.vDiffuse = lerp(vDissolveColor, g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV), fEdgeFactor);
    }
    else
    {
        Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
    }
    

    if (g_iTextureFlag & (1 << NORMALS))
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexUV);
        float3 vNormal = Decode_NormalMap(vNormalDesc);
    
        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
        vNormal = mul(vNormal, WorldMatrix);
        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
    }
    
    Out.vORM = float4(1.f, 0.5f, 0.f, 1.f);
    
    if (g_iTextureFlag & (1 << MRAO))
    {
        float4 vMRAO = g_MRAOTexture.Sample(LinearWrapSampler, In.vTexUV);
        Out.vORM = float4(vMRAO.z, vMRAO.y, vMRAO.x, 1.f);
    }
        
    if (g_iTextureFlag & (1 << EMISSIVE))
    {
        Out.vEmissive = g_EmissiveTexture.Sample(LinearWrapSampler, In.vTexUV);
        Out.vEmissive.w = 1.f;
    }
    
    Out.vRimLight = g_vRimColor * 0.6f;
    Out.vRimLight.a = 1.f;

    return Out;
}


struct VS_OUT_MOTIONTRAIL
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN_MOTIONTRAIL
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_MOTIONTRAIL
{
    float4 vColor : SV_TARGET0;
};

VS_OUT_MOTIONTRAIL VS_MAIN_MOTIONTRAIL(VS_IN In)
{
    VS_OUT_MOTIONTRAIL Out = (VS_OUT_MOTIONTRAIL) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

    vector vBonePosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vPosition = mul(vBonePosition, matWVP);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexUV;

    return Out;
}


PS_OUT_MOTIONTRAIL PS_MAIN_MOTIONTRAIL(PS_IN_MOTIONTRAIL In)
{
    PS_OUT_MOTIONTRAIL Out = (PS_OUT_MOTIONTRAIL) 0;
    
    //float4 vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    //vDiffuse.rgb = (vDiffuse.r + vDiffuse.g + vDiffuse.b) / 3.f;
    //vDiffuse *
    Out.vColor =  g_vMotionTrailColor;

    return Out;
}

float4 PS_MAIN_DISTORTION(PS_IN_MOTIONTRAIL In) : SV_TARGET0
{
    return g_fDistortionStrength;
}

struct VS_OUT_CASCADE
{
    float4 vPosWorld : POSITION;
};

VS_OUT_CASCADE VS_MAIN_CASCADE(VS_IN In)
{
    VS_OUT_CASCADE Out;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

    vector vBonePosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

    Out.vPosWorld = mul(vBonePosition, g_WorldMatrix);
    
    return Out;
}


struct GS_IN_CASCADE
{
    float4 vPosWorld : POSITION;
};

struct GS_OUT_CASCADE
{
    float4 vPosition : SV_POSITION;
    uint iRTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_MAIN_CASCADE(triangle GS_IN_CASCADE In[3], inout TriangleStream<GS_OUT_CASCADE> OutputStream)
{
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT_CASCADE Out = (GS_OUT_CASCADE) 0;
        Out.iRTIndex = i;
        for (int j = 0; j < 3; ++j)
        {
            Out.vPosition = mul(In[j].vPosWorld, g_CascadeViewProj[i]);
            OutputStream.Append(Out);
        }
        OutputStream.RestartStrip();
    }
}

struct PS_IN_CASCADE
{
    float4  vPosition : SV_POSITION;
    uint    iRTIndex : SV_RenderTargetArrayIndex;
};

void PS_MAIN_CASCADE(PS_IN_CASCADE In)
{
  
}


struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOWDEPTH(VS_IN In)
{
    VS_OUT_SHADOW Out;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

    vector vBonePosition = mul(float4(In.vPosition, 1.f), BoneMatrix);

    Out.vPosition = mul(vBonePosition, g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ShadowViewProj);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}


void PS_MAIN_SHADOWDEPTH(PS_IN_SHADOW In)
{
  
}

float4 PS_MAIN_FORWARD_PBR(PS_IN In) : SV_TARGET0
{
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
    clip(vDiffuse.a - 0.1f);
    
    vDiffuse = pow(vDiffuse, 2.2f);
    float4 vNormal = 0.f;
    float4 vORM = float4(1.f, 0.5f, 0.f, 1.f);
    float4 vEmissive = 0.f;
        
    if (g_iTextureFlag & (1 << NORMALS))
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexUV);
        float3 vCalcNormal = Decode_NormalMap(vNormalDesc);
    
        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
        vCalcNormal = mul(vCalcNormal, WorldMatrix);
        vNormal = vector(vCalcNormal, 0.f);
    }
    else
    {
        vNormal = vector(In.vNormal, 0.f);
    }
   
    if (g_iTextureFlag & (1 << MRAO))
    {
        float4 vMRAO = g_MRAOTexture.Sample(LinearWrapSampler, In.vTexUV);
        vORM = float4(vMRAO.z, vMRAO.y, vMRAO.x, 1.f);
    }
        
    if (g_iTextureFlag & (1 << EMISSIVE))
    {
        vEmissive = g_EmissiveTexture.Sample(LinearWrapSampler, In.vTexUV);
        vEmissive.w = 1.f;
    }
    
    vector vLightDir = normalize(g_vLightDir) * -1.f;

    float fRoughness = vORM.y;
    float fMetallic = vORM.z;

    float3 ToEye = normalize((g_vCamPosition - In.vWorldPos).xyz);
    float3 HalfWay = normalize(ToEye + vLightDir.xyz);

    float NdotL = max(0.1f, dot(vNormal.xyz, vLightDir.xyz));
    float NdotH = max(0.f, dot(vNormal.xyz, HalfWay));
    float NdotV = max(0.f, dot(vNormal.xyz, ToEye));
  
    float3 F0 = lerp(Fdielectric, vDiffuse.rgb, fMetallic);
    float3 F = SchlickFresnel(F0, max(0.f, dot(HalfWay, ToEye)));
    float3 kd = lerp(float3(1.f, 1.f, 1.f) - F, float3(0.f, 0.f, 0.f), fMetallic);
    float3 DiffuseBRDF = kd * vDiffuse.xyz;

    float D = NdfGGX(NdotH, fRoughness);
    float3 G = SchlickGGX(NdotL, NdotV, fRoughness);
    float3 SpecularBRDF = (F * D * G) / max(1e-5, 4.f * NdotL * NdotV);
  
    float3 Radiance = g_vLightDiffuse.rgb * g_fLightStrength;

    float3 DirectLighting = (DiffuseBRDF + SpecularBRDF) * Radiance * NdotL;
    
    float3 vCombined = ACESFilmTonemap(DirectLighting.rgb);
    
    return float4(vCombined, 1.f);
}


technique11 DefaultTechinque
{
    pass Default // 0
    {
        SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass Dissolve // 1
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default_With_WriteStencil, 1);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
        ComputeShader = NULL;
    }

    pass Cascade // 2
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_CASCADE();
        HullShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN_CASCADE();
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CASCADE();
        ComputeShader = NULL;
    }
	
    pass ShadowDepth // 3
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOWDEPTH();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOWDEPTH();
        ComputeShader = NULL;
    }


    pass MotionTrail // 4
    {
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_MOTIONTRAIL();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONTRAIL();
        ComputeShader = NULL;
    }


    pass Distortion // 5
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_MOTIONTRAIL();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
        ComputeShader = NULL;
    }

    pass Forward_PBR // 6
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FORWARD_PBR();
        ComputeShader = NULL;
    }

    pass WithStencil // 7
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default_With_WriteStencil, 1);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass AlphaBlend // 7
    {
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
        ComputeShader = NULL;
    }


	
}


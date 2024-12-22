#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ORMTexture;
Texture2D g_MRAOTexture;
Texture2D g_RMATexture;
Texture2D g_MetalTexture;
Texture2D g_RoughnessTexture;
Texture2D g_EmissiveTexture;

int g_iTextureFlag;

float4x4 g_CascadeViewProj[3];
matrix g_ShadowViewProj;

float g_fFar = 1000.f;
float g_fMipLevel = 0.f;

float4 g_vCamPosition;

struct VS_IN
{
    float3  vPosition : POSITION;
    float3  vNormal : NORMAL;
    float2  vTexUV : TEXCOORD0;
    float3  vTangent : TANGENT;
    
    float4  vRight : TEXCOORD1;
    float4  vUp : TEXCOORD2;
    float4  vLook : TEXCOORD3;
    float4  vTranslation : TEXCOORD4;
    int     iObjFlag : INDEX0;
};

struct VS_OUT
{
    float4  vPosition : SV_POSITION;
    float3  vNormal : NORMAL;
    float2  vTexUV : TEXCOORD0;
    float4  vProjPos : TEXCOORD1;
    float4  vWorldPos : TEXCOORD2;
    float3  vTangent : TANGENT;
    float3  vBinormal : BINORMAL;
    int     iObjFlag : INDEX0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vWorldPos = vPosition;
    
    Out.vPosition = mul(vPosition, matVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), TransformMatrix).xyz);
    Out.vTexUV = In.vTexUV;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), TransformMatrix).xyz);
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
    Out.vProjPos = Out.vPosition;
    Out.iObjFlag = In.iObjFlag;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    int    iObjFlag : INDEX0;
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

    float fMipLevel = g_fMipLevel;
    float fDist = length(g_vCamPosition - In.vWorldPos);
    float fDistMin = 5.f;
    float fDistMax = 150.f;
    
    if (0.f == g_fMipLevel)
         fMipLevel = 5.f * saturate(fDist / (fDistMax - fDistMin));
    
    Out.vDiffuse = g_DiffuseTexture.SampleLevel(LinearWrapSampler, In.vTexUV, fMipLevel);
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

    if ((g_iTextureFlag & (1 << NORMALS)))
    {
        vector vNormalDesc = g_NormalTexture.SampleLevel(LinearWrapSampler, In.vTexUV, fMipLevel);
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
        
    if (g_iTextureFlag & (1 << ORM))
    {
        Out.vORM = g_ORMTexture.Sample(LinearWrapSampler, In.vTexUV);
    }
        
    if (g_iTextureFlag & (1 << RMA))
    {
        float4 vRMA = g_RMATexture.Sample(LinearWrapSampler, In.vTexUV);
            
        Out.vORM = float4(vRMA.z, vRMA.x, vRMA.y, 1.f);
    }
   
    
    if (g_iTextureFlag & (1 << METAL))
    {
        float fMetal = g_MetalTexture.Sample(LinearWrapSampler, In.vTexUV);
        Out.vORM.z = fMetal;
    }
    
    if (g_iTextureFlag & (1 << ROUGHNESS))
    {
        float fRoughness = g_RoughnessTexture.Sample(LinearWrapSampler, In.vTexUV);
        Out.vORM.y = fRoughness;
    }
        
    if (g_iTextureFlag & (1 << EMISSIVE))
    {
        Out.vEmissive = g_EmissiveTexture.Sample(LinearWrapSampler, In.vTexUV);
        Out.vEmissive.w = 1.f;
    }
    
    // Reflection Masking
    if (In.iObjFlag & (1 << FLAG_REFLECTION))
        Out.vDepth.w = 1.f;
    
   Out.vRimLight = 0.f;

    return Out;
}


struct VS_OUT_CASCADE
{
    float4  vPosWorld : POSITION;
    int     iObjFlag : FLAG;
};

VS_OUT_CASCADE VS_MAIN_CASCADE(VS_IN In)
{
    VS_OUT_CASCADE Out = (VS_OUT_CASCADE) 0;
    
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosWorld = vPosition;
    Out.iObjFlag = In.iObjFlag;
    
    return Out;
}

struct GS_IN_CASCADE
{
    float4  vPosWorld : POSITION;
    int     iObjFlag  : FLAG;
};

struct GS_OUT_CASCADE
{
    float4  vPosition : SV_POSITION;
    uint    iRTIndex : SV_RenderTargetArrayIndex;
    int     iObjFlag : FLAG;
};

[maxvertexcount(9)]
void GS_MAIN_CASCADE(triangle GS_IN_CASCADE In[3], inout TriangleStream<GS_OUT_CASCADE> OutputStream)
{
    GS_OUT_CASCADE Out = (GS_OUT_CASCADE) 0;
    Out.iObjFlag = In[0].iObjFlag;
    
    for (int i = 0; i < 3; ++i)
    {      
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
    int     iObjFlag : FLAG;
};

void PS_MAIN_CASCADE(PS_IN_CASCADE In)
{
   if (!(In.iObjFlag & (1 << FLAG_SHADOW)))
       discard;
}


struct VS_OUT_SHADOW
{
    float4  vPosition : SV_POSITION;
    float4  vProjPos : TEXCOORD0;
    int     iObjFlag : TEXCOORD1;
};

struct PS_IN_SHADOW
{
    float4  vPosition : SV_POSITION;
    float4  vProjPos : TEXCOORD0;
    int     iObjFlag : TEXCOORD1;
};


VS_OUT_SHADOW VS_MAIN_SHADOWDEPTH(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    Out.vPosition = mul(vPosition, g_ShadowViewProj);
    Out.vProjPos = Out.vPosition;
    Out.iObjFlag = In.iObjFlag;
    
    return Out;
}

void PS_MAIN_SHADOWDEPTH(PS_IN_SHADOW In) 
{
    if (!(In.iObjFlag & (1 << FLAG_SHADOW)))
        discard;
       
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

    pass Cascade // 1
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

    pass ShadowDepth // 2
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOWDEPTH();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOWDEPTH();
        ComputeShader = NULL;
    }
}




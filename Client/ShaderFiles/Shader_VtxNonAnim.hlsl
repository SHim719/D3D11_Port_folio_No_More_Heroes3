
#include "Shader_Defines.hlsli"

// float4x4
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ORMTexture;
Texture2D g_EmissiveTexture;
Texture2D g_DissolveTexture;
Texture2D g_MRAOTexture;
Texture2D g_RMATexture;
Texture2D g_MetalTexture;
Texture2D g_RoughnessTexture;

int       g_iTextureFlag;

float4x4  g_CascadeViewProj[3];
float4x4  g_ShadowViewProj;

float     g_fFar = 1000.f;

float g_fDissolveAmount;

float g_fAlphaValue;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
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

    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix).xyz);
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix).xyz);
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
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
	
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
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
    
    bool bORM = g_iTextureFlag & (1 << MRAO) && g_iTextureFlag & (1 << ORM) && g_iTextureFlag & (1 << RMA);
    
    if (bORM)
    {
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

    return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);

    float fDissolve = g_DissolveTexture.Sample(LinearWrapSampler, In.vTexUV).r;
    float fClip = fDissolve - g_fDissolveAmount;
    
    float fEdgeWidth = 0.1f;
	
    float4 vDissolveColor = float4(3.f, 1.f, 1.f, 1.f);
    vector vTempDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
    
    clip(fClip - 0.001f);

    if (fClip < fEdgeWidth)
    {
        float fEdgeFactor = smoothstep(0.f, fEdgeWidth, fClip);
        
        if (vTempDiffuse.a < 0.1f)
            discard;
        
        Out.vDiffuse = lerp(vDissolveColor, vTempDiffuse, fEdgeFactor);
    }
    else
    {
        Out.vDiffuse = vTempDiffuse;
        
        if (Out.vDiffuse.a < 0.1f)
            discard;
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

    return Out;
}

float4 PS_MAIN_MASK_ALPHABLEND(PS_IN In) : SV_TARGET0
{
    float4 vColor = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
   
    vColor.a = vColor.r;
    
    vColor.rgb = pow(vColor.rgb, 2.5f);
    
    return vColor;
}

float4 PS_MAIN_SIMPLE_ALPHABLEND(PS_IN In) : SV_TARGET0
{
    vector vColor = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexUV);
   
    vColor.a = g_fAlphaValue;
    
    if(vColor.a <0.1f)
        discard;
    
    return vColor;
}

struct VS_OUT_CASCADE
{
    float4 vPosWorld : POSITION;
};


VS_OUT_CASCADE VS_MAIN_CASCADE(VS_IN In)
{
    VS_OUT_CASCADE Out = (VS_OUT_CASCADE) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    Out.vPosWorld = vPosition;
    
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
    float4 vPosition : SV_POSITION;
    uint iRTIndex : SV_RenderTargetArrayIndex;
};

void PS_MAIN_CASCADE(PS_IN_CASCADE In)
{
   
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
};

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
};

VS_OUT_SHADOW VS_MAIN_SHADOWDEPTH(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

    Out.vPosition = mul(vPosition, g_ShadowViewProj);
    
    return Out;
}

void PS_MAIN_SHADOWDEPTH(PS_IN_SHADOW In)
{
    
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
        SetDepthStencilState(DSS_Default, 0);
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

    pass Mask_AlpahaBlend // 4
    {
        SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MASK_ALPHABLEND();
        ComputeShader = NULL;
    }

    pass Simple_AlpahaBlend // 5
    {
        SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SIMPLE_ALPHABLEND();
        ComputeShader = NULL;
    }

    pass Default_With_WriteStencil // 6
    {
        SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default_With_WriteStencil, 1);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }
}


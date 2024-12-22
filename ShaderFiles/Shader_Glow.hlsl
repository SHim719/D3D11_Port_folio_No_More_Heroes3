#include "Shader_Defines.hlsli"

static const float BlurWeights[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
};

int     g_iDiv;
int     g_iWeightWidth = 13;

texture2D g_PrevBlurXTexture;
texture2D g_PrevBlurYTexture;

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


PS_OUT PS_MAIN_BlurX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fdx = 1.f / 1280.f;
    
    int iHalf = g_iWeightWidth / 2;
    uint iIndex = 0;
   
    [unroll]
    for (int i = 0; i < g_iWeightWidth; ++i)
    {
        float4 vSampleColor = g_PrevBlurXTexture.Sample(LinearClampSampler, In.vTexcoord + float2(fdx, 0.f) * float(i - iHalf));
        Out.vColor += BlurWeights[i] * vSampleColor;
    }
    
    Out.vColor.rgb += float3(0.0001f, 0.0001f, 0.0001f);
    Out.vColor /= 2.f;
    Out.vColor.a = 0.f;
    
    return Out;
}


PS_OUT PS_MAIN_BlurY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fdy = 1.f / 720.f;
    
    int iHalf = g_iWeightWidth / 2;
    uint iIndex = 0;
   
    [unroll]
    for (int i = 0; i < g_iWeightWidth; ++i)
    {
        float4 vSampleColor = g_PrevBlurYTexture.Sample(LinearClampSampler, In.vTexcoord + float2(0.f, fdy) * float(i - iHalf));
        Out.vColor += BlurWeights[i] * vSampleColor; 
    }
    
    Out.vColor.rgb += float3(0.0001f, 0.0001f, 0.0001f);
    Out.vColor /= 2.f;
    Out.vColor.a = 0.f;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass BlurX // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BlurX();
    }

    pass BlurY // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BlurY();
    }


}


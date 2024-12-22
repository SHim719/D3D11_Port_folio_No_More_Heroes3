#include "Shader_Defines.hlsli"

matrix  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;

float4  g_vColor;
float   g_fAlpha;
uint    g_fColorAlpha;

struct VS_IN
{
    float3 vPosition : POSITION;
    
    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
    uint vColorAlpha : COLORALPHA;
    uint vColorOff : COLOROFF;
    float4 vColor : COLOR;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    uint vColorAlpha : COLORALPHA;
    uint vColorOff : COLOROFF;
    float4 vColor : COLOR;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	/* 그리기용 정점의 데이터를 로컬스페이스 내에서 TransformMatrix상태로 변환한다. */
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    
    Out.vColorAlpha = In.vColorAlpha;
    Out.vColor = In.vColor;
    Out.vColorOff = In.vColorOff;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    uint vColorAlpha : COLORALPHA;
    uint vColorOff : COLOROFF;
    float4 vColor : COLOR;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = float4(In.vColor);
    
    if (In.vColorOff == 1)      /* CUI_InstanceBase::INSUI_COLOR READY */
        Out.vColor += float4(0.7f, 0.7f, 0.7f, 1.f);
    
    if (In.vColorOff == 2)      /* CUI_InstanceBase::INSUI_COLOR INSUI_BLACK */
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    if (In.vColorOff == 3)      /* CUI_InstanceBase::INSUI_COLOR DARK */
        Out.vColor *= float4(0.6f, 0.6f, 0.6f, 1.f);
    
    /* Alpha */
    if (In.vColorAlpha == 0)
        Out.vColor.a = 0.f;
    
    return Out;
}

PS_OUT PS_SPECIFIC_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_vColor;
    
    /* Alpha */
    if (In.vColorAlpha == 0)
        Out.vColor.a = 0.f;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultUIInstance //0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ColorSetUIInstance //1
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_SPECIFIC_COLOR();
    }



}
#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture;
Texture2D g_CutTexture;

Texture2D g_DiffuseTexture1;
Texture2D g_DiffuseTexture2;

float4 g_vColor;
float g_fAlpha;
float g_fTimeDelta;

float g_fGauge;
float g_fVOffSet;
float g_fAlpha_Timer;
float3 g_vUIColor;
float g_fVOffSet_Move;

float2 g_FadeIn_And_Move;
float2 g_vSpeedGauge;
float2 g_MovePos;
bool m_bColorChangeWhite;

float g_fProgressRatio;

Texture2D g_PachinkoTexture1;
Texture2D g_PachinkoTexture2;

float g_fPachinkoRatio;

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float3 vPosition : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = In.vPosition;

    return Out;
}

struct GS_IN
{
    float3 vPosition : POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(4)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutputStream)
{
    GS_OUT Out = (GS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(-0.5f, 0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(0.f, 0.f);
    
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(0.5f, 0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(1.f, 0.f);
  
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(-0.5f, -0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(0.f, 1.f);
  
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(0.5f, -0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(1.f, 1.f);
    
    OutputStream.Append(Out);
}


[maxvertexcount(4)]
void GS_MAIN_FULL(point GS_IN In[1], inout TriangleStream<GS_OUT> OutputStream)
{
    GS_OUT Out = (GS_OUT) 0;
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(-1.f, 1.f, 0.f, 0.f);
    Out.vTexcoord = float2(0.f, 0.f);
    
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(1.f, 1.f, 0.f, 0.f);
    Out.vTexcoord = float2(1.f, 0.f);
  
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(-1.f, -1.f, 0.f, 0.f);
    Out.vTexcoord = float2(0.f, 1.f);
  
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(1.f, -1.f, 0.f, 0.f);
    Out.vTexcoord = float2(1.f, 1.f);
    
    OutputStream.Append(Out);
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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}

PS_OUT PS_MOVE_UV(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 UV = In.vTexcoord;
    UV.x += g_fTimeDelta;

    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, UV);

    if (Out.vColor.a < 0.3f)
        discard;

    return Out;
}

PS_OUT PS_CHANGE_WHITE_TO_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);

    float fColor = (saturate(vColor.r) + saturate(vColor.g) + saturate(vColor.b)) / 3.f;
    
    // UI rgb 평균색이 흰색(1.f)에 가까우면 해당색으로 (흰색의 범주를 널널하게 주기 위해 0.3f로 줌), 아니면 원래색으로
    if (fColor >= 0.3f)
        Out.vColor.rgb = g_vUIColor;
    else
        Out.vColor.rgb = vColor;
    
    // 그외 알파가 들어간건 알파값 그대로
    Out.vColor.a = vColor.a;
    
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}

PS_OUT PS_GAUGE_DECREASE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);

    // 텍스쳐 가로 길이가 현재 체력보다 크면 그 부분 지우기
    if (g_fGauge < In.vTexcoord.x)
        discard;
        
    return Out;
}

PS_OUT PS_FADE_IO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_vColor;
    Out.vColor.a = g_fAlpha;
    
    return Out;
}

PS_OUT PS_IMAGE_UP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, float2(In.vTexcoord.x, In.vTexcoord.y + g_fVOffSet));
    
    if (0.467f < In.vTexcoord.y)
        discard;
    
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}

PS_OUT PS_FADE_OUT(PS_IN In)
{
    // Client - 사라짐(+) / 나타남(-)
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
 
    if (Out.vColor.a < 0.1f)
        discard;
    
    if (g_fAlpha_Timer != 0.f)
        Out.vColor.a -= saturate(g_fAlpha_Timer);
    
    return Out;
}

PS_OUT PS_FADE_IN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, float2(In.vTexcoord.x + g_FadeIn_And_Move.y, In.vTexcoord.y));

    /*
    float fScroll = 0.2f;
    
    if (In.vTexcoord < 1.f - fScroll)
        Out.vColor = g_DiffuseTexture1.Sample(LinearWrapSampler, In.vTexcoord + float2(fScroll, 0.f));
    else
        Out.vColor = g_DiffuseTexture2.Sample(LinearWrapSampler, In.vTexcoord - float2(1.f - fScroll, 0.f));
    
    float2 vTexUV1 = In.vTexcoord - float2(0.2f, 0.f);
    Out.vColor = g_DiffuseTexture1.Sample(LinearClampSampler, vTexUV1);
    */      

    if (Out.vColor.a < 0.55f)
        discard;
    
    if (g_FadeIn_And_Move.x != 0.f)
        Out.vColor.a = saturate(g_FadeIn_And_Move.x);
    
    return Out;
}

PS_OUT PS_CHANGE_COLOR_TO_WHITE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    
    if(m_bColorChangeWhite)
    {
        float fColor = (saturate(vColor.r) + saturate(vColor.g) + saturate(vColor.b)) / 3.f;
    
        if (fColor <= 0.6f)
            Out.vColor.rgb = (1.f, 1.f, 1.f, 1.f);
        else
            Out.vColor.rgb = vColor;
    }
    else
        Out.vColor.rgb = vColor;
    
    Out.vColor.a = vColor.a;
    
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}

float4 PS_MAIN_STAGE_PROGRESS_BAR(PS_IN In) : SV_TARGET0
{
    float4 vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    
    clip(vColor.a - 0.1f);
    
    if (1.f - In.vTexcoord.y > g_fProgressRatio)
        discard;

    return vColor;
}

float4 PS_MAIN_PACHINKO(PS_IN In) : SV_TARGET0
{
    float2 vUV1 = In.vTexcoord - float2(0.f, g_fPachinkoRatio);
    float2 vUV2 = In.vTexcoord + float2(0.f, 1.f - g_fPachinkoRatio);
    
    float4 vColor1 = g_PachinkoTexture1.Sample(LinearWrapSampler, vUV1);
    float4 vColor2 = g_PachinkoTexture2.Sample(LinearWrapSampler, vUV2);

    float4 vColor;
    
    if (In.vTexcoord.y > g_fPachinkoRatio)
        vColor = vColor1;
    else
        vColor = vColor2;
    
    clip(vColor.a - 0.1f);

    return vColor;
}

PS_OUT PS_ALPHA_IMGCUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    float4 CutImg = g_CutTexture.Sample(LinearWrapSampler, In.vTexcoord);
 
    if (Out.vColor.a < 0.1f)
        discard;
    
    if (CutImg.r <= 0.1f)
        discard;
    
    if (g_fAlpha_Timer != 0.f)
        Out.vColor.a -= saturate(g_fAlpha_Timer);
    
    return Out;
}

PS_OUT PS_CHANGE_GAUGE_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);

    float fColor = (saturate(vColor.r) + saturate(vColor.g) + saturate(vColor.b)) / 3.f;
    
    // UI rgb 평균색이 흰색(1.f)에 가까우면 해당색으로 (흰색의 범주를 널널하게 주기 위해 0.3f로 줌), 
    if (fColor >= 0.3f)
    {
        if (g_vSpeedGauge.y == 0.f) //  가로
        {
            if (g_vSpeedGauge.x > In.vTexcoord.x)
                Out.vColor.rgb = float3(1.f, 0.f, 0.f); // 게이지 온
            else
                Out.vColor.rgb = float3(0.5f, 0.9f, 0.2f); // 게이지 없음
        }
        else if (g_vSpeedGauge.y == 1.f) // 세로
        {
            if (1.f - g_vSpeedGauge.x < 1.f - In.vTexcoord.y)
                Out.vColor.rgb = float3(1.f, 0.5f, 0.f);
            else
                Out.vColor.rgb = float3(1.f, 0.5f - g_vSpeedGauge.x, 0.f);
        }
    }
    // 흰색 외 다른색은 원래색으로
    else
        Out.vColor.rgb = vColor;
    
    // 그외 알파가 들어간건 알파값 그대로
    Out.vColor.a = vColor.a;
    
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}

PS_OUT PS_MINIMAP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    
    float2 fCenterUI = float2(0.5f + g_MovePos.x, 0.5f + g_MovePos.y);
    float fRange = 0.14f;
    
    if (length(In.vTexcoord - fCenterUI) > fRange)
        Out.vColor.a = 0.f;
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_IMAGE_MOVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, float2(In.vTexcoord.x + g_fVOffSet_Move, In.vTexcoord.y));
    
    //if (0.467f < In.vTexcoord.y)
    //    discard;
    
    if (Out.vColor.a < 0.1f)
        discard;

    return Out;
}


float4 PS_MAIN_ALPHABLEND(PS_IN In) : SV_Target0
{
    float4 vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    vColor.a = g_fAlpha;

    return vColor;
}


technique11 DefaultTechnique
{
    pass DefaultUI //0
    {
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlendUI //1
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass MoveUVUI //2
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MOVE_UV();
    }

    pass ChangeColorUI //3
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_CHANGE_WHITE_TO_COLOR();
    }

    pass Gauge_Decrease //4
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_GAUGE_DECREASE();
    }

    pass FadeIO //5
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN_FULL();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_FADE_IO();
    }

    pass ImgUp //6
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_IMAGE_UP();
    }

    pass Fade_Out //7
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_FADE_OUT();
    }

    pass Fade_In //8
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_FADE_IN();
    }

    pass ChangeColorToWhite //9
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_CHANGE_COLOR_TO_WHITE();
    }

    pass StageProgressBar_AW // 10
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_STAGE_PROGRESS_BAR();
    }

    pass Pachinko // 11
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PACHINKO();
    }

    pass AlphaAndImgCut // 12
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_ALPHA_IMGCUT();
    }

    pass ChangeGaugeColor // 13
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_CHANGE_GAUGE_COLOR();
    }

    pass Minimap // 14
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MINIMAP();

    }

    pass ImgMove //15
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_IMAGE_MOVE();
    }

    pass AlphaBlend //16
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
    }
}
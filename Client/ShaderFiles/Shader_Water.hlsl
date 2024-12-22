#include "Shader_Defines.hlsli"

Matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float2 g_fFlowSpeed = float2(0.1f, 0.1f);
float g_fVoronoiScale = 200.f; // 18
float g_fWaterNoiseScale = 0.21f; //0.21f
float4 g_vWaterColor = float4(0.01f, 0.286667f, 1.f, 1.f);
float4 g_vLightColor = float4(0.7f, 0.7f, 0.7f, 1.f);
float g_fTime;
 
float2 Get_WaterRandom(float2 vPos)
{
    return frac(sin(float2(
        dot(vPos, float2(12.9898, 78.233)),
        dot(vPos, float2(-148.998, -65.233))
    )) * 43758.5453);
}

float Get_Value_WaterNoise(float2 pos)
{
    float2 p = floor(pos);
    float2 f = frac(pos);

    float v00 = Get_WaterRandom(p + float2(0.0, 0.0)).x;
    float v10 = Get_WaterRandom(p + float2(1.0, 0.0)).x;
    float v01 = Get_WaterRandom(p + float2(0.0, 1.0)).x;
    float v11 = Get_WaterRandom(p + float2(1.0, 1.0)).x;

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(lerp(v00, v10, u.x), lerp(v01, v11, u.x), u.y);
}

float Voronoi(float2 v)
{
    float2 v_floor = floor(v);
    float2 v_fract = frac(v);

    float min_dist = 2.0;

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 n = float2(float(x), float(y));
            float2 p = Get_WaterRandom(v_floor + n);
            float2 diff = p + n;
            float d = distance(v_fract, diff);

            min_dist = min(min_dist, d);
        }
    }

    return min_dist;
}


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
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
   
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

PS_OUT PS_WATER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 noise_uv = In.vTexcoord + g_fFlowSpeed * g_fTime * 0.1;

    In.vTexcoord.x += (
        Get_Value_WaterNoise(noise_uv * 5.45432) * 0.5 +
        Get_Value_WaterNoise(noise_uv * 15.754824) * 0.25 +
        Get_Value_WaterNoise(noise_uv * 35.4274729) * 0.125 +
        Get_Value_WaterNoise(noise_uv * 95.65347829) * 0.0625 +
        Get_Value_WaterNoise(noise_uv * 285.528934) * 0.03125 +
        Get_Value_WaterNoise(noise_uv * 585.495328) * 0.015625 +
        Get_Value_WaterNoise(noise_uv * 880.553426553) * 0.0078125 +
        Get_Value_WaterNoise(noise_uv * 2080.5483905843) * 0.00390625
    ) * g_fWaterNoiseScale;

    float2 v1 = In.vTexcoord * g_fVoronoiScale + (100.f + g_fTime) * g_fFlowSpeed;
    float2 v2 = In.vTexcoord * g_fVoronoiScale + (-100.0 + g_fTime * 0.83) * g_fFlowSpeed;

    float f = lerp(Voronoi(v1) * 1.1, Voronoi(v2) * 1.1, sin(g_fTime * 0.758) * 0.5 + 0.5);

    Out.vColor = lerp(g_vLightColor, g_vWaterColor, saturate(1.0 - f * f * f));
    
    return Out;
}



technique11 DefaultTechnique
{
    pass Water // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_WATER();
    }



}


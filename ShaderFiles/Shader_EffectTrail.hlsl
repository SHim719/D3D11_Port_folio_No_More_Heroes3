#include "Shader_Defines.hlsli"
#include "Shader_Effect_Defines.hlsli"

// float4x4
matrix /*g_WorldMatrix,*/ g_ViewMatrix, g_ProjMatrix;
float4 g_vColor;
vector g_vStartColor;
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

    matrix matWV, matWVP;

   // matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWV = g_ViewMatrix;
    matWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
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
    float4 vDistortion : SV_TARGET1;
    float4 vFlag : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fAlphaMask = 1.f;
    float fNoise = 1.f;
    float fNoiseAlpha = 1.f;
    float fDissolve = 0.f;
    float fClip;
    float4 vColor, vAlphaMask, vNoise, vDissolve;
    if (g_TextureFlag & BASE)
    {
        vColor = g_BaseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    }
    if (g_TextureFlag & ALPHA) // Mask
    {
        float2 vMaskUV = In.vTexcoord + g_vAlphaMaskUVOffset;
        vAlphaMask = 0 == g_iAlphaMaskSampler ? g_AlphaMask.Sample(LinearWrapSampler, vMaskUV)
        : g_AlphaMask.Sample(LinearClampSampler, vMaskUV);
        
        fAlphaMask = vAlphaMask.r;
    }
    if (g_TextureFlag & NOISE)    // Noise
    {
        float2 vNoiseUV = In.vTexcoord + g_vNoiseUVOffset;
        vNoise = 0 == g_iNoiseSampler ? g_NoiseTexture.Sample(LinearWrapSampler, vNoiseUV)
        : g_NoiseTexture.Sample(LinearClampSampler, vNoiseUV);
        fNoise = vNoise.r;
        
        if (true == g_bNoiseMultiplyAlpha)
            fNoiseAlpha = fNoise;
    }
    if (g_TextureFlag & DISSOLVETEX)
    {
        float2 vDissolveUV = In.vTexcoord + g_vDissolveUVOffset;
        vDissolve = 0 == g_iDissolveSampler ? g_DissolveTexture.Sample(LinearWrapSampler, vDissolveUV)
            : g_DissolveTexture.Sample(LinearClampSampler, vDissolveUV);
        fDissolve = vDissolve.r;
        
        fClip = fDissolve - g_fDissolveAmount;
        
        clip(fClip - 0.001f);
    }

    if (g_bColorLerp)
    {
        Out.vColor.rgb = mul(vColor.rgb * g_vLerpColor.rgb * g_vMulColor.rgb, fNoise) + g_vAddColor.rgb;
        Out.vColor.a = vColor.a * fAlphaMask * g_vLerpColor.a * g_vMulColor.a * fNoiseAlpha + g_vAddColor.a;
    }
    else
    {
        if (0.f != g_vStartColor.r || 0.f != g_vStartColor.g || 0.f != g_vStartColor.b)
            Out.vColor.rgb = mul(vColor.rgb * g_vStartColor.rgb * g_vMulColor.rgb, fNoise) + g_vAddColor.rgb;
        else
            Out.vColor.rgb = mul(vColor.rgb * g_vMulColor.rgb, fNoise) + g_vAddColor.rgb;
        
        if (0.f != g_vStartColor.a)
            Out.vColor.a = vColor.a * fAlphaMask * g_vStartColor.a * g_vMulColor.a * fNoiseAlpha + g_vAddColor.a;
        else
            Out.vColor.a = vColor.a * fAlphaMask * g_vMulColor.a * fNoiseAlpha + g_vAddColor.a;
    }
    

    if (Out.vColor.r <= g_vColor_Clip.r && Out.vColor.g <= g_vColor_Clip.g && Out.vColor.b <= g_vColor_Clip.b)
        discard;
    
    if (Out.vColor.a <= g_vColor_Clip.a)
        discard;
     
   
    if (g_bDistortion)
    {
        Out.vDistortion = fAlphaMask * fNoise * g_fDistortionIntensity;
    }
   
    if (true == g_bDissolveEdge)
    {
        float fEdgeWidth = g_fDissolveEdgeWidth;
        float4 vDissolveColor = g_vDissolveDiffuse;
        
        if (fClip < fEdgeWidth)
        {
            float fEdgeFactor = smoothstep(0.f, fEdgeWidth, fClip);
            Out.vColor = lerp(vDissolveColor, Out.vColor, fEdgeFactor);
        }
    }
    Out.vFlag.w = 1.f;
    
    return Out;
}



technique11 DefaultTechinque
{
    pass DefaultWrap // 0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }


    pass Nonblend // 1
    {
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

}


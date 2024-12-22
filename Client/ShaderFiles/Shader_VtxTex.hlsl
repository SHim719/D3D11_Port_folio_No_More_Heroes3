#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D   g_DiffuseTexture;

float4 g_vColor;

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

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
    
    Out.vColor.rgb = pow(Out.vColor.rgb, 2.2f);

    return Out;
}

technique11 DefaultTechnique
{
	//0
    pass Default
    {
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    //1
    pass AlphaBlend
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

	//2
   //pass SoftEffect
   //{
   //    SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
   //    SetDepthStencilState(DSS_Default, 0);
   //    SetRasterizerState(RS_Default);
	//	
   //    VertexShader = compile vs_5_0 VS_MAIN_SOFT();
   //    HullShader = NULL;
   //    DomainShader = NULL;
   //    GeometryShader = NULL;
   //    PixelShader = compile ps_5_0 PS_MAIN_SOFT();
   //}
	
	//3
   //pass Background
   //{
   //    SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
   //    SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
   //    SetRasterizerState(RS_Default);
   //
   //    VertexShader = compile vs_5_0 VS_MAIN();
   //    HullShader = NULL;
   //    DomainShader = NULL;
   //    GeometryShader = NULL;
   //    PixelShader = compile ps_5_0 PS_MAIN();
   //}


	/* pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}*/
}
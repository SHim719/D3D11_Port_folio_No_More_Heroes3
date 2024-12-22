#ifndef SHADER_DEFINE
#define SHADER_DEFINE

sampler LinearWrapSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler LinearBorderSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

sampler PointWrapSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};


sampler PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};

SamplerComparisonState ComparisionSampler = sampler_state
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    ComparisonFunc = LESS_EQUAL;
};

RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
    FillMode = solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_CullNone
{
    FillMode = solid;
    CullMode = none;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;

    StencilEnable = false;
};

DepthStencilState DSS_NoZTest_And_Write
{
    DepthEnable = false;
    DepthWriteMask = zero;

    StencilEnable = false;
};

DepthStencilState DSS_Effect
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFail = keep;

    BackFaceStencilFunc = always;
    BackFaceStencilPass = replace;
    BackFaceStencilFail = keep;
};

DepthStencilState DSS_Sky
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;

    StencilEnable = false;
};

DepthStencilState DSS_Default_With_WriteStencil
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFail = keep;

    BackFaceStencilFunc = always;
    BackFaceStencilPass = replace;
    BackFaceStencilFail = keep;
};

DepthStencilState DSS_WriteStencil
{
    DepthEnable = true;
    DepthWriteMask = zero; 
    DepthFunc = always;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFail = keep;

    BackFaceStencilFunc = always;
    BackFaceStencilPass = replace;
    BackFaceStencilFail = keep;
};

BlendState BS_None
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp[0] = Add;
};

BlendState BS_EffectBlend
{
    BlendEnable[0] = true;
	// BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = one;
    BlendOp = Add;
};

BlendState BS_OneBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

float3 Decode_NormalMap(float4 vNormalDesc)
{
    float3 vNormal;
    vNormal.xy = vNormalDesc.xy * 2.f - 1.f;
    vNormal.z = sqrt(1.f - saturate(dot(vNormal.xy, vNormal.xy)));
    return vNormal;
}

#define NORMALS 0
#define MRAO 1
#define ORM 2
#define RMA 3
#define MRM 4
#define METAL 5
#define ROUGHNESS 6
#define EMISSIVE 7

#define FLAG_SHADOW 0
#define FLAG_REFLECTION 1 


#endif

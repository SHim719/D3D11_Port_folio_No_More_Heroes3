#include "Shader_Defines.hlsli"
#include "Shader_Effect_Defines.hlsli"

#define BILLBOARD 0
#define STRETCHED_BILLBOARD 1
#define NONE_BILLBOARD 3

#define FOLLOW 11
#define MOVE 12


matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 g_vCamPosition;
float4 g_vCameraUp;
int g_iParticleMode;

float g_fRotationZAngle;
bool g_bBillboardWithoutUp;
float g_fStretchFactor;

bool g_bXStretch;

float3 g_vLightDir;
//vector g_vLightDIffuse;
vector g_vLightAmbient;

Texture2D g_DepthTexture;
float     g_fFar = 1000.f;


struct VS_IN
{
    float3 vPosition : POSITION;

    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
    
    float4 vColor : COLOR0;
    float4 vVelocity : VELOCITY;
    float fLifeTime : LIFETIME;
    int iSpriteIndex : INDEX;
    float fDissolve : DISSOLVE;
    float fRotation : ROTATION;
    

};

struct VS_OUT
{
    float3 vPosition : POSITION;

    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
    
    float4 vColor : COLOR0;
    float3 vVelocity : VELOCITY;
    float fLifeTime : LIFETIME;
    int iSpriteIndex : INDEX;
    float fDissolve : DISSOLVE;
    float fRotation : ROTATION;
    
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = In.vPosition;
    
    Out.vRight = In.vRight;
    Out.vUp = In.vUp;
    Out.vLook = In.vLook;
    Out.vTranslation = In.vTranslation;
    
    Out.vColor = In.vColor;
    Out.vVelocity = In.vVelocity;
    Out.fLifeTime = In.fLifeTime;
    Out.iSpriteIndex = In.iSpriteIndex;
    Out.fDissolve = In.fDissolve;
    Out.fRotation = In.fRotation;
    
    return Out;
}

struct GS_IN
{
    float3 vPosition : POSITION;

    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
    
    float4 vColor : COLOR0;
    float3 vVelocity : VELOCITY;
    float fLifeTime : LIFETIME;
    int iSpriteIndex : INDEX;
    float fDissolve : DISSOLVE;
    float fRotation : ROTATION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float fLifeTime : TEXCOORD2;
    int iSpriteIndex : INDEX;
    float fDissolve : DISSOLVE;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
};

[maxvertexcount(4)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutputStream)
{
    GS_OUT Out = (GS_OUT) 0;
    
    float4 vRight = float4(0.5f, 0.f, 0.f, 0.f);
    float4 vUp = float4(0.f, 0.5f, 0.f, 0.f);
    float4 vLook = float4(0.f, 0.f, 0.5f, 0.f);
    float2 vPSize = float2(length(In[0].vRight), length(In[0].vUp));
  
   
    matrix TransformMatrix = float4x4(In[0].vRight, In[0].vUp, In[0].vLook, In[0].vTranslation);
    matrix WorldMatrix = g_WorldMatrix;
    
    if (FOLLOW == g_iTransformType)
    {
        WorldMatrix[0] = float4(1.f, 0.f, 0.f, 0.f);
        WorldMatrix[1] = float4(0.f, 1.f, 0.f, 0.f);
        WorldMatrix[2] = float4(0.f, 0.f, 1.f, 0.f);
        WorldMatrix[3] = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    matrix matTW = mul(TransformMatrix, WorldMatrix);
    float4 vCentralPos = mul(float4(In[0].vPosition, 1.f), matTW);
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    if (g_iParticleMode == BILLBOARD)
    {
        vLook = normalize(g_vCamPosition - vCentralPos);
        vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)), 0.f) * 0.5f * vPSize.x;
        if (true == g_bBillboardWithoutUp) 
            vUp = float4(0.f, 1.f, 0.f, 0.f) * 0.5f * vPSize.y;
        else
            vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)), 0.f) * 0.5f * vPSize.y;
        
        float4x4 RotMat = CreateAxisRotMat(vLook, In[0].fRotation);
        
        vRight = mul(vRight, RotMat);
        vUp = mul(vUp, RotMat);
        
        Out.vNormal = vLook.xyz;
        Out.vTangent = vRight.xyz;
   
        Out.vPosition = vCentralPos + vRight + vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        
        Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), matVP)).xyz;
        Out.vTangent = normalize(mul(float4(Out.vTangent, 0.f), matVP)).xyz;
        Out.vBiNormal = cross(Out.vNormal, Out.vTangent);
        
        Out.fDissolve = In[0].fDissolve;
        Out.fLifeTime = In[0].fLifeTime;
        Out.vColor = In[0].vColor;
        Out.iSpriteIndex = In[0].iSpriteIndex;
        Out.vTexcoord = float2(0.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = vCentralPos - vRight + vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(1.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = vCentralPos + vRight - vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(0.f, 1.f);
    
        OutputStream.Append(Out);

        Out.vPosition = vCentralPos - vRight - vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(1.f, 1.f);
    
        OutputStream.Append(Out);

    }
    else if ((g_iParticleMode == STRETCHED_BILLBOARD) && 0.f != length(In[0].vVelocity))
    {
        float3 vVelocityDir = mul(normalize(In[0].vVelocity), g_fStretchFactor);
        vVelocityDir = mul(float4(vVelocityDir, 0.f), WorldMatrix);

        vLook = normalize(g_vCamPosition - vCentralPos);
        vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)), 0.f) * 0.5f * vPSize.x;
        vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)), 0.f) * 0.5f * vPSize.y;
    
        Out.vTangent = vRight.xyz;
        Out.vBiNormal = vUp.xyz;
        Out.vNormal = cross(Out.vTangent, Out.vBiNormal);
       
        
        float4 p1, p2, p3, p4;
        
        if (g_bXStretch)
        {
            p1 = vCentralPos + vRight + vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p2 = vCentralPos - vRight + vUp - float4(vVelocityDir, 0.f) * 0.5f;
            p3 = vCentralPos + vRight - vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p4 = vCentralPos - vRight - vUp - float4(vVelocityDir, 0.f) * 0.5f;
        }
        else
        {
            p1 = vCentralPos + vRight + vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p2 = vCentralPos - vRight + vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p3 = vCentralPos + vRight - vUp - float4(vVelocityDir, 0.f) * 0.5f;
            p4 = vCentralPos - vRight - vUp - float4(vVelocityDir, 0.f) * 0.5f;
        }
        
       
        Out.vPosition = p1;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), matVP)).xyz;
        Out.vTangent = normalize(mul(float4(Out.vTangent, 0.f), matVP)).xyz;
        Out.vBiNormal = cross(Out.vNormal, Out.vTangent);
        Out.fDissolve = In[0].fDissolve;
        Out.fLifeTime = In[0].fLifeTime;
        Out.vColor = In[0].vColor;
        Out.iSpriteIndex = In[0].iSpriteIndex;
        Out.vTexcoord = float2(0.f, 0.f);
        
        OutputStream.Append(Out);
         
        Out.vPosition = p2;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(1.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = p3;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(0.f, 1.f);
    
        OutputStream.Append(Out);

        Out.vPosition = p4;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(1.f, 1.f);
    
        OutputStream.Append(Out);
    }
    else if (g_iParticleMode == NONE_BILLBOARD)
    {
        float4 vRight = In[0].vRight;
        float4 vUp = In[0].vUp;
        float4 vLook = In[0].vLook;
        float2 vPSize = float2(length(In[0].vRight), length(In[0].vUp));
  
        matrix TransformMatrix = float4x4(In[0].vRight, In[0].vUp, In[0].vLook, In[0].vTranslation);

        matTW = mul(TransformMatrix, WorldMatrix);
        vCentralPos = mul(float4(In[0].vPosition, 1.f), matTW);
        
        matrix NormMat =
        {
            float4(normalize(WorldMatrix[0])),
            float4(normalize(WorldMatrix[1])),
            float4(normalize(WorldMatrix[2])),
            float4(normalize(WorldMatrix[3])),
        };
        
        vRight = mul(vRight, NormMat);
        vUp = mul(vUp, NormMat);
    
        matVP = mul(g_ViewMatrix, g_ProjMatrix);
        
        Out.vNormal = vLook.xyz;
        Out.vTangent = vRight.xyz;
   
        Out.vPosition = vCentralPos + vRight + vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        
        Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), matVP)).xyz;
        Out.vTangent = normalize(mul(float4(Out.vTangent, 0.f), matVP)).xyz;
        Out.vBiNormal = cross(Out.vNormal, Out.vTangent);
        
        Out.fDissolve = In[0].fDissolve;
        Out.fLifeTime = In[0].fLifeTime;
        Out.vColor = In[0].vColor;
        Out.iSpriteIndex = In[0].iSpriteIndex;
        Out.vTexcoord = float2(0.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = vCentralPos - vRight + vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(1.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = vCentralPos + vRight - vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(0.f, 1.f);
    
        OutputStream.Append(Out);

        Out.vPosition = vCentralPos - vRight - vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vTexcoord = float2(1.f, 1.f);
    
        OutputStream.Append(Out);
    }
}

struct GS_OUT_SOFT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float fLifeTime : TEXCOORD2;
    int iSpriteIndex : INDEX;
    float fDissolve : DISSOLVE;
    float4 vProjPos : TEXCOORD3;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
};

[maxvertexcount(4)]
void GS_MAIN_SOFT(point GS_IN In[1], inout TriangleStream<GS_OUT_SOFT> OutputStream)
{
    GS_OUT_SOFT Out = (GS_OUT_SOFT) 0;
    
    float4 vRight = float4(0.5f, 0.f, 0.f, 0.f);
    float4 vUp = float4(0.f, 0.5f, 0.f, 0.f);
    float4 vLook = float4(0.f, 0.f, 0.5f, 0.f);
    float2 vPSize = float2(length(In[0].vRight), length(In[0].vUp));
  
   
    matrix TransformMatrix = float4x4(In[0].vRight, In[0].vUp, In[0].vLook, In[0].vTranslation);
    matrix WorldMatrix = g_WorldMatrix;
    
    if (FOLLOW == g_iTransformType)
    {
        WorldMatrix[0] = float4(1.f, 0.f, 0.f, 0.f);
        WorldMatrix[1] = float4(0.f, 1.f, 0.f, 0.f);
        WorldMatrix[2] = float4(0.f, 0.f, 1.f, 0.f);
        WorldMatrix[3] = float4(0.f, 0.f, 0.f, 1.f);
    }
    
    matrix matTW = mul(TransformMatrix, WorldMatrix);
    float4 vCentralPos = mul(float4(In[0].vPosition, 1.f), matTW);
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matrix matWVP = mul(WorldMatrix, matVP);
    
    if (g_iParticleMode == BILLBOARD)
    {
        vLook = normalize(g_vCamPosition - vCentralPos);
        vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)), 0.f) * 0.5f * vPSize.x;
        if (true == g_bBillboardWithoutUp) 
            vUp = float4(0.f, 1.f, 0.f, 0.f) * 0.5f * vPSize.y;
        else
            vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)), 0.f) * 0.5f * vPSize.y;
        
        float4x4 RotMat = CreateAxisRotMat(vLook, In[0].fRotation);
        
        vRight = mul(vRight, RotMat);
        vUp = mul(vUp, RotMat);
        
        Out.vNormal = vLook.xyz;
        Out.vTangent = vRight.xyz;
   
        Out.vPosition = vCentralPos + vRight + vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        
        Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), matVP)).xyz;
        Out.vTangent = normalize(mul(float4(Out.vTangent, 0.f), matVP)).xyz;
        Out.vBiNormal = cross(Out.vNormal, Out.vTangent);
        
        Out.fDissolve = In[0].fDissolve;
        Out.fLifeTime = In[0].fLifeTime;
        Out.vColor = In[0].vColor;
        Out.iSpriteIndex = In[0].iSpriteIndex;
        Out.vTexcoord = float2(0.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = vCentralPos - vRight + vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        Out.vTexcoord = float2(1.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = vCentralPos + vRight - vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        Out.vTexcoord = float2(0.f, 1.f);
    
        OutputStream.Append(Out);

        Out.vPosition = vCentralPos - vRight - vUp;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        Out.vTexcoord = float2(1.f, 1.f);
    
        OutputStream.Append(Out);

    }
    else if ((g_iParticleMode == STRETCHED_BILLBOARD) && 0.f != length(In[0].vVelocity))
    {
        float3 vVelocityDir = mul(normalize(In[0].vVelocity), g_fStretchFactor);
        vVelocityDir = mul(float4(vVelocityDir, 0.f), WorldMatrix);

        vLook = normalize(g_vCamPosition - vCentralPos);
        vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)), 0.f) * 0.5f * vPSize.x;
        vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)), 0.f) * 0.5f * vPSize.y;
    
        Out.vTangent = vRight.xyz;
        Out.vBiNormal = vUp.xyz;
        Out.vNormal = cross(Out.vTangent, Out.vBiNormal);
       
        
        float4 p1, p2, p3, p4;
        
        if (g_bXStretch)
        {
            p1 = vCentralPos + vRight + vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p2 = vCentralPos - vRight + vUp - float4(vVelocityDir, 0.f) * 0.5f;
            p3 = vCentralPos + vRight - vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p4 = vCentralPos - vRight - vUp - float4(vVelocityDir, 0.f) * 0.5f;
        }
        else
        {
            p1 = vCentralPos + vRight + vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p2 = vCentralPos - vRight + vUp + float4(vVelocityDir, 0.f) * 0.5f;
            p3 = vCentralPos + vRight - vUp - float4(vVelocityDir, 0.f) * 0.5f;
            p4 = vCentralPos - vRight - vUp - float4(vVelocityDir, 0.f) * 0.5f;
        }
        
       
        Out.vPosition = p1;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), matVP)).xyz;
        Out.vTangent = normalize(mul(float4(Out.vTangent, 0.f), matVP)).xyz;
        Out.vBiNormal = cross(Out.vNormal, Out.vTangent);
        Out.fDissolve = In[0].fDissolve;
        Out.fLifeTime = In[0].fLifeTime;
        Out.vColor = In[0].vColor;
        Out.iSpriteIndex = In[0].iSpriteIndex;
        Out.vTexcoord = float2(0.f, 0.f);
        
        OutputStream.Append(Out);
         
        Out.vPosition = p2;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        Out.vTexcoord = float2(1.f, 0.f);

        OutputStream.Append(Out);

        Out.vPosition = p3;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        Out.vTexcoord = float2(0.f, 1.f);
    
        OutputStream.Append(Out);

        Out.vPosition = p4;
        Out.vPosition = mul(Out.vPosition, matVP);
        Out.vProjPos = Out.vPosition;
        Out.vTexcoord = float2(1.f, 1.f);
    
        OutputStream.Append(Out);
    }
  
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float fLifeTime : TEXCOORD2;
    int iSpriteIndex : INDEX;
    float fDissolve : DISSOLVE;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
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
    
    if (In.fLifeTime <= 0.f)
        discard;
    
    float fAlphaMask = 1.f;
    float fNoise = 1.f;
    float fNoiseAlpha = 1.f;
    float fDistortion = 0.f;
    float fColoScaleUV = 1.f;
    float fAlphaMaskNoiseUV = 0.f;
    float fBaseNoiseUV = 0.f;
    float fDissolve = 0.f;
    float4 fLight = { 1.f, 1.f, 1.f, 1.f };
    
    float4 vColor, vNormalDesc, vAlphaMask, vColorScaleUV, vColorScale;
    float4 vNoise, vDissolve, vBaseNoiseUV, vAlphaMaskNoiseUV;
    
    float2 vAlphaRadialUV, vBaseRadialUV;
    
    // Used AlphaMask Radial UV Mapping
    if (g_iRadialMappingFlag & ALPHARADIAL)
    {
        ///////////////////////////////
        
        float2 vCenterUV = mul(In.vTexcoord, 2.f) - float2(1.f, 1.f);
        
        float2 vRadius = length(vCenterUV);
        float fTheta = atan2(vCenterUV.y, vCenterUV.x);
        fTheta /= (2.f * 3.14159265359f) + 0.5f;

        if (g_TextureFlag & ALPHA_NOISE_UV)
        {
            vAlphaMaskNoiseUV = g_AlphaMaskNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord);
            fAlphaMaskNoiseUV = vAlphaMaskNoiseUV.r;
        }
        
        float2 vRadialUV;
        if (1.f == g_vAlphaRadiusFlag.x)
        {
        
            vRadialUV.x = vRadius + fAlphaMaskNoiseUV; // U : Radius
            vRadialUV.y = fTheta; // V : Theta;
        }
        else if (1.f == g_vAlphaRadiusFlag.y)
        {
            vRadialUV.y = vRadius + fAlphaMaskNoiseUV; // U : Radius
            vRadialUV.x = fTheta; // V : Theta;
        }
        
        vAlphaRadialUV = vRadialUV;
    }
    
    // Used Base Texture Radial UV Mapping
    if (g_iRadialMappingFlag & BASERADIAL)
    {
     
        ///////////////////////////////
        
        float2 vCenterUV = In.vTexcoord - float2(0.5f, 0.5f);
        float2 vRadius = length(vCenterUV);
        float fTheta = atan2(vCenterUV.y, vCenterUV.x);
        fTheta /= (2.f * 3.14159265359f);
        
        if (g_TextureFlag & BASE_NOISE_UV)
        {
            vBaseNoiseUV = g_BaseNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord);
            fBaseNoiseUV = vBaseNoiseUV.r;
        }
        
        float2 vRadialUV;
        if (1.f == g_vBaseRadiusFlag.x)
        {
        
            vRadialUV.x = vRadius + fBaseNoiseUV; // U : Radius
            vRadialUV.y = fTheta; // V : Theta;
        }
        else if (1.f == g_vBaseRadiusFlag.y)
        {
            vRadialUV.y = vRadius + fBaseNoiseUV; // U : Radius
            vRadialUV.x = fTheta; // V : Theta;
        }
        
        vBaseRadialUV = vRadialUV;
    }
    
    if (g_TextureFlag & BASE)
    {
        if (true == g_bUseSpriteBase)
        {
            int row = In.iSpriteIndex / g_iBaseCol;
            int col = In.iSpriteIndex % g_iBaseCol;
            
            float2 vBaseUV;
            vBaseUV.x = (col * g_fColBaseUVInterval) + (In.vTexcoord.x * g_fColBaseUVInterval);
            vBaseUV.y = (row * g_fRowBaseUVInterval) + (In.vTexcoord.y * g_fRowBaseUVInterval);
            vBaseUV += g_vBaseUVOffset;
            vColor = 0 == g_iBaseSampler ? g_BaseTexture.Sample(LinearWrapSampler, vBaseUV)
            : g_BaseTexture.Sample(LinearClampSampler, vBaseUV);
        }
        else
        {
            float2 vBaseUV;

            if (g_iRadialMappingFlag & BASERADIAL)
                vBaseUV = vBaseRadialUV;
            else
            {
                if (g_TextureFlag & BASE_NOISE_UV)
                {
                    vBaseNoiseUV = g_BaseNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord);
                    fBaseNoiseUV = vBaseNoiseUV.r;
                
                    if (g_vBaseNoiseInfoFlag.NORMALIZED_NOIOSE)
                        fBaseNoiseUV = fBaseNoiseUV * 2.f - 1.f;
                
                    if (g_vBaseNoiseInfoFlag.OFFSET_NOIOSE)
                    {
                        if (g_vBaseNoiseInfoFlag.ADD_NOISE)
                            vBaseUV = In.vTexcoord + fBaseNoiseUV;
                        else
                            vBaseUV = In.vTexcoord - fBaseNoiseUV;
                    }
                    else
                        vBaseUV = fBaseNoiseUV;

                }
                else
                {
                    vBaseUV = In.vTexcoord;
                }
            }

            vColor = 0 == g_iBaseSampler ? g_BaseTexture.Sample(LinearWrapSampler, vBaseUV + g_vBaseUVOffset)
            : g_BaseTexture.Sample(LinearClampSampler, vBaseUV + g_vBaseUVOffset);
        }
    }
    if (g_TextureFlag & NORMALTEX)
    {
        vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexcoord);
        
        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
        float3x3 NormaldMat = float3x3(In.vTangent, In.vBiNormal, In.vNormal);
    
        vNormal = mul(vNormal, NormaldMat);
        float fSahde = max(dot(normalize(g_vLightDir) * 1.f, normalize(vNormal)), 0.f);
        fLight = saturate(fSahde + (g_vLightAmbient * 1.f));
        
    }
    if (g_TextureFlag & COLORSCALE_UV)
    {
        float2 vUV = In.vTexcoord + g_vColorScaleUVOffset;
        vColorScaleUV = 0 == g_iColorScaleSampler ? g_ColorScaleUV.Sample(LinearWrapSampler, vUV)
            : g_ColorScaleUV.Sample(LinearClampSampler, vUV);
        
        fColoScaleUV = vColorScaleUV.r;
    }
    if (g_TextureFlag & ALPHA) // Mask
    {
        if (true == g_bUseSpriteMask)
        {
            int row = In.iSpriteIndex / g_iMaskCol;
            int col = In.iSpriteIndex % g_iMaskCol;
            
            float2 vMaskUV;
            vMaskUV.x = (col * g_fColMaskUVInterval) + (In.vTexcoord.x * g_fColMaskUVInterval);
            vMaskUV.y = (row * g_fRowMaskUVInterval) + (In.vTexcoord.y * g_fRowMaskUVInterval);
            vMaskUV += g_vAlphaMaskUVOffset;
            vAlphaMask = 0 == g_iAlphaMaskSampler ? g_AlphaMask.Sample(LinearWrapSampler, vMaskUV)
            : g_AlphaMask.Sample(LinearClampSampler, vMaskUV);
        }
        else
        {
            float2 vMaskUV;
        
            if (g_iRadialMappingFlag & ALPHARADIAL)
                vMaskUV = vAlphaRadialUV;
            else
            {
                if (g_TextureFlag & ALPHA_NOISE_UV)
                {
                    vAlphaMaskNoiseUV = g_AlphaMaskNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord);
                    fAlphaMaskNoiseUV = vAlphaMaskNoiseUV.r;
                
                    if (g_vAlphaNoiseInfoFlag.NORMALIZED_NOIOSE)
                        fAlphaMaskNoiseUV = fAlphaMaskNoiseUV * 2.f - 1.f;
                
                    if (g_vAlphaNoiseInfoFlag.OFFSET_NOIOSE)
                    {
                        if (g_vAlphaNoiseInfoFlag.ADD_NOISE)
                            vMaskUV = In.vTexcoord + fAlphaMaskNoiseUV;
                        else
                            vMaskUV = In.vTexcoord - fAlphaMaskNoiseUV;
                    }
                    else
                        vMaskUV = fAlphaMaskNoiseUV;
                }
                else
                {
                    vMaskUV = In.vTexcoord;
                }
            }
        
            vAlphaMask = 0 == g_iAlphaMaskSampler ? g_AlphaMask.Sample(LinearWrapSampler, vMaskUV + g_vAlphaMaskUVOffset)
                : g_AlphaMask.Sample(LinearClampSampler, vMaskUV + g_vAlphaMaskUVOffset);
        
        }
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
        
        float fClip = fDissolve - In.fDissolve;
        
        clip(fClip - 0.001f);
    }
    if (g_TextureFlag & COLORSCALE)    // ColorScale
    {
        vColorScale = g_ColorScaleTexture.Sample(LinearWrapSampler, fColoScaleUV);
    }
    else
        vColorScale = float4(1.f, 1.f, 1.f, 1.f);

    
    if (g_bColorLerp)
    {
        Out.vColor.rgb = mul(vColor.rgb * In.vColor.rgb * g_vMulColor.rgb * fLight.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        Out.vColor.a = vColor.a * fAlphaMask * In.vColor.a * g_vMulColor.a * fLight.a * fNoiseAlpha + g_vAddColor.a;
    }
    else
    {
        if (0.f != In.vColor.r || 0.f != In.vColor.g || 0.f != In.vColor.b)
            Out.vColor.rgb = mul(vColor.rgb * In.vColor.rgb * g_vMulColor.rgb * fLight.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        else
            Out.vColor.rgb = mul(vColor.rgb * g_vMulColor.rgb * fLight.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        
        if (0.f != In.vColor.a)
            Out.vColor.a = vColor.a * fAlphaMask * In.vColor.a * g_vMulColor.a * vColorScale.a * fLight.a * fNoiseAlpha + g_vAddColor.a;
        else
            Out.vColor.a = vColor.a * fAlphaMask * g_vMulColor.a * vColorScale.a * fLight.a * fNoiseAlpha + g_vAddColor.a;
    }
    
    
    
    if (Out.vColor.r <= g_vColor_Clip.r && Out.vColor.g <= g_vColor_Clip.g && Out.vColor.b <= g_vColor_Clip.b)
        discard;
    
    if (Out.vColor.a <= g_vColor_Clip.a)
        discard;
    
    
    if (g_bDistortion)
    {
        Out.vDistortion = fAlphaMask * fNoise * g_fDistortionIntensity;
    }

    Out.vFlag.w = 1.f;
    
    return Out;
}

struct PS_IN_SOFT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float fLifeTime : TEXCOORD2;
    int iSpriteIndex : INDEX;
    float fDissolve : DISSOLVE;
    float4 vProjPos : TEXCOORD3;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
};

PS_OUT PS_MAIN_SOFT(PS_IN_SOFT In) 
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.fLifeTime <= 0.f)
        discard;
    
    float fAlphaMask = 1.f;
    float fNoise = 1.f;
    float fNoiseAlpha = 1.f;
    float fDistortion = 0.f;
    float fColoScaleUV = 1.f;
    float fAlphaMaskNoiseUV = 0.f;
    float fBaseNoiseUV = 0.f;
    float fDissolve = 0.f;
    float4 fLight = { 1.f, 1.f, 1.f, 1.f };
    
    float4 vColor, vNormalDesc, vAlphaMask, vColorScaleUV, vColorScale;
    float4 vNoise, vDissolve,vBaseNoiseUV, vAlphaMaskNoiseUV;
    
    float2 vAlphaRadialUV, vBaseRadialUV;
    
    // Used AlphaMask Radial UV Mapping
    if (g_iRadialMappingFlag & ALPHARADIAL)
    {
        ///////////////////////////////
        
        float2 vCenterUV = mul(In.vTexcoord, 2.f) - float2(1.f, 1.f);
        
        float2 vRadius = length(vCenterUV);
        float fTheta = atan2(vCenterUV.y, vCenterUV.x);
        fTheta /= (2.f * 3.14159265359f) + 0.5f;

        if (g_TextureFlag & ALPHA_NOISE_UV)
        {
            vAlphaMaskNoiseUV = g_AlphaMaskNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vAlphaMaskNoiseUVOffset);
            fAlphaMaskNoiseUV = vAlphaMaskNoiseUV.r;
        }
        
        float2 vRadialUV;
        if (1.f == g_vAlphaRadiusFlag.x)
        {
        
            vRadialUV.x = vRadius + fAlphaMaskNoiseUV; // U : Radius
            vRadialUV.y = fTheta; // V : Theta;
        }
        else if (1.f == g_vAlphaRadiusFlag.y)
        {
            vRadialUV.y = vRadius + fAlphaMaskNoiseUV; // U : Radius
            vRadialUV.x = fTheta; // V : Theta;
        }
        
        vAlphaRadialUV = vRadialUV;
    }
    
    // Used Base Texture Radial UV Mapping
    if (g_iRadialMappingFlag & BASERADIAL)
    {
        ///////////////////////////////
        
        float2 vCenterUV = In.vTexcoord - float2(0.5f, 0.5f);
        float2 vRadius = length(vCenterUV);
        float fTheta = atan2(vCenterUV.y, vCenterUV.x);
        fTheta /= (2.f * 3.14159265359f);
        
        if (g_TextureFlag & BASE_NOISE_UV)
        {
            vBaseNoiseUV = g_BaseNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vBaseNoiseUVOffset);
            fBaseNoiseUV = vBaseNoiseUV.r;
        }
        
        float2 vRadialUV;
        if (1.f == g_vBaseRadiusFlag.x)
        {
        
            vRadialUV.x = vRadius + fBaseNoiseUV; // U : Radius
            vRadialUV.y = fTheta; // V : Theta;
        }
        else if (1.f == g_vBaseRadiusFlag.y)
        {
            vRadialUV.y = vRadius + fBaseNoiseUV; // U : Radius
            vRadialUV.x = fTheta; // V : Theta;
        }
        
        vBaseRadialUV = vRadialUV;
    }
    
    if (g_TextureFlag & BASE)
    {
        if (true == g_bUseSpriteBase)
        {
            int row = In.iSpriteIndex / g_iBaseCol;
            int col = In.iSpriteIndex % g_iBaseCol;
            
            float2 vBaseUV;
            vBaseUV.x = (col * g_fColBaseUVInterval) + (In.vTexcoord.x * g_fColBaseUVInterval);
            vBaseUV.y = (row * g_fRowBaseUVInterval) + (In.vTexcoord.y * g_fRowBaseUVInterval);
            vBaseUV += g_vBaseUVOffset;
            vColor = 0 == g_iBaseSampler ? g_BaseTexture.Sample(LinearWrapSampler, vBaseUV)
            : g_BaseTexture.Sample(LinearClampSampler, vBaseUV);
        }
        else
        {
            float2 vBaseUV;

            if (g_iRadialMappingFlag & BASERADIAL)
                vBaseUV = vBaseRadialUV;
            else
            {
                if (g_TextureFlag & BASE_NOISE_UV)
                {
                    vBaseNoiseUV = g_BaseNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vBaseNoiseUVOffset);
                    fBaseNoiseUV = vBaseNoiseUV.r;
                
                    if (g_vBaseNoiseInfoFlag.NORMALIZED_NOIOSE)
                        fBaseNoiseUV = fBaseNoiseUV * 2.f - 1.f;
                
                    if (g_vBaseNoiseInfoFlag.OFFSET_NOIOSE)
                    {
                        if (g_vBaseNoiseInfoFlag.ADD_NOISE)
                            vBaseUV = In.vTexcoord + fBaseNoiseUV;
                        else
                            vBaseUV = In.vTexcoord - fBaseNoiseUV;
                    }
                    else
                        vBaseUV = fBaseNoiseUV;

                }
                else
                {
                    vBaseUV = In.vTexcoord;
                }
            }

            vColor = 0 == g_iBaseSampler ? g_BaseTexture.Sample(LinearWrapSampler, vBaseUV + g_vBaseUVOffset)
            : g_BaseTexture.Sample(LinearClampSampler, vBaseUV + g_vBaseUVOffset);
        }
    }
    if (g_TextureFlag & NORMALTEX)
    {
        vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexcoord);
        
        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
        float3x3 NormaldMat = float3x3(In.vTangent, In.vBiNormal, In.vNormal);
    
        vNormal = mul(vNormal, NormaldMat);
        float fSahde = max(dot(normalize(g_vLightDir) * 1.f, normalize(vNormal)), 0.f);
        fLight = saturate(fSahde + (g_vLightAmbient * 1.f));
        
    }
    if (g_TextureFlag & COLORSCALE_UV)
    {
        float2 vUV = In.vTexcoord + g_vColorScaleUVOffset;
        vColorScaleUV = 0 == g_iColorScaleSampler ? g_ColorScaleUV.Sample(LinearWrapSampler, vUV)
            : g_ColorScaleUV.Sample(LinearClampSampler, vUV);
        
        fColoScaleUV = vColorScaleUV.r;
    }
    if (g_TextureFlag & ALPHA) // Mask
    {
        if (true == g_bUseSpriteMask)
        {
            int row = In.iSpriteIndex / g_iMaskCol;
            int col = In.iSpriteIndex % g_iMaskCol;
            
            float2 vMaskUV;
            vMaskUV.x = (col * g_fColMaskUVInterval) + (In.vTexcoord.x * g_fColMaskUVInterval);
            vMaskUV.y = (row * g_fRowMaskUVInterval) + (In.vTexcoord.y * g_fRowMaskUVInterval);
            vMaskUV += g_vAlphaMaskUVOffset;
            vAlphaMask = 0 == g_iAlphaMaskSampler ? g_AlphaMask.Sample(LinearWrapSampler, vMaskUV)
            : g_AlphaMask.Sample(LinearClampSampler, vMaskUV);
        }
        else
        {
            float2 vMaskUV;
        
            if (g_iRadialMappingFlag & ALPHARADIAL)
                vMaskUV = vAlphaRadialUV;
            else
            {
                if (g_TextureFlag & ALPHA_NOISE_UV)
                {
                    vAlphaMaskNoiseUV = g_AlphaMaskNoiseUVTexture.Sample(LinearWrapSampler, In.vTexcoord + g_vAlphaMaskNoiseUVOffset);
                    fAlphaMaskNoiseUV = vAlphaMaskNoiseUV.r;
                
                    if (g_vAlphaNoiseInfoFlag.NORMALIZED_NOIOSE)
                        fAlphaMaskNoiseUV = fAlphaMaskNoiseUV * 2.f - 1.f;
                
                    if (g_vAlphaNoiseInfoFlag.OFFSET_NOIOSE)
                    {
                        if (g_vAlphaNoiseInfoFlag.ADD_NOISE)
                            vMaskUV = In.vTexcoord + fAlphaMaskNoiseUV;
                        else
                            vMaskUV = In.vTexcoord - fAlphaMaskNoiseUV;
                    }
                    else
                        vMaskUV = fAlphaMaskNoiseUV;
                }
                else
                {
                    vMaskUV = In.vTexcoord;
                }
            }
        
            vAlphaMask = 0 == g_iAlphaMaskSampler ? g_AlphaMask.Sample(LinearWrapSampler, vMaskUV + g_vAlphaMaskUVOffset)
                : g_AlphaMask.Sample(LinearClampSampler, vMaskUV + g_vAlphaMaskUVOffset);
        
        }
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
        
        float fClip = fDissolve - In.fDissolve;
        
        clip(fClip - 0.001f);
    }
    if (g_TextureFlag & COLORSCALE)    // ColorScale
    {
        vColorScale = g_ColorScaleTexture.Sample(LinearWrapSampler, fColoScaleUV);
    }
    else
        vColorScale = float4(1.f, 1.f, 1.f, 1.f);

  

    if (g_bColorLerp)
    {
        Out.vColor.rgb = mul(vColor.rgb * In.vColor.rgb * g_vMulColor.rgb * fLight.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        Out.vColor.a = vColor.a * fAlphaMask * In.vColor.a * g_vMulColor.a * fLight.a * fNoiseAlpha + g_vAddColor.a;
    }
    else
    {
        if (0.f != In.vColor.r || 0.f != In.vColor.g || 0.f != In.vColor.b)
            Out.vColor.rgb = mul(vColor.rgb * In.vColor.rgb * g_vMulColor.rgb * fLight.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        else
            Out.vColor.rgb = mul(vColor.rgb * g_vMulColor.rgb * fLight.rgb * vColorScale.rgb, fNoise) + g_vAddColor.rgb;
        
        if (0.f != In.vColor.a)
            Out.vColor.a = vColor.a * fAlphaMask * In.vColor.a * g_vMulColor.a * vColorScale.a * fLight.a * fNoiseAlpha + g_vAddColor.a;
        else
            Out.vColor.a = vColor.a * fAlphaMask * g_vMulColor.a * vColorScale.a * fLight.a * fNoiseAlpha + g_vAddColor.a;
    }
    
    
    if (Out.vColor.r <= g_vColor_Clip.r && Out.vColor.g <= g_vColor_Clip.g && Out.vColor.b <= g_vColor_Clip.b)
        discard;
    
    if (Out.vColor.a <= g_vColor_Clip.a)
        discard;
    
    float fViewZ = In.vProjPos.w;

    float2 vDepthUV;
    vDepthUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vDepthUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float fOldViewZ = g_DepthTexture.Sample(PointClampSampler, vDepthUV).y * g_fFar;

    Out.vColor.a = Out.vColor.a * saturate(fOldViewZ - fViewZ);

    if (g_bDistortion)
    {
        Out.vDistortion = fAlphaMask * fNoise * g_fDistortionIntensity;
    }
    
    return Out;
}



technique11 DefaultTechnique
{
    pass AlphaBlend //0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass Soft // 1
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN_SOFT();
        PixelShader = compile ps_5_0 PS_MAIN_SOFT();
        ComputeShader = NULL;
    }

    pass AlphaTest // 2
    {
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass OneBlend // 3
    {
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 1);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

}

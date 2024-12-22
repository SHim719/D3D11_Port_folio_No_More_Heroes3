#include "Shader_Defines.hlsli"
#include "Shader_Defines_PBR.hlsli"
#include "Shader_Defines_SSAO.hlsli"
#include "Shader_Defines_SSR.hlsli"

float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4    g_ViewMatrixInv, g_ProjMatrixInv;
float4      g_vCamPosition;

float       g_fFar = 1000.f;

vector      g_vLightDir;
vector      g_vLightPos;
float       g_fLightRange;
float4      g_vLightAmbient;
float4      g_vLightDiffuse;
float4      g_vLightSpecular;
float       g_fLightStrength;
float       g_fSpotPower;

float4      g_vAmbientColor;
float       g_fAmbientStrength;

Texture2D   g_DiffuseTexture;
Texture2D   g_NormalTexture;
Texture2D   g_DepthTexture;
Texture2D   g_ORMTexture;
Texture2D   g_EmissiveTexture;
Texture2D   g_RimLightTexture;
Texture2D   g_SSAOTexture;
Texture2D   g_AmbientTexture;

Texture2D   g_ShadeTexture;
Texture2D   g_Texture;

float       RimThresholdMin = 0.6;
float       RimThresholdMax = 0.7;

bool        g_bSSAO = false;
bool        g_bShadow = false;
bool        g_bCascade = false;
bool        g_bIBL = false;

///////////////////Shadow//////////////////////////
Texture2DArray          g_CascadeShadowMap;
matrix                  g_CascadeViewProj[3];
float                   g_CascadeEnd[3];

Texture2D               g_ShadowDepthTexture;
matrix                  g_ShadowViewProj;
float                   g_fShadowBias;
///////////////////////////////////////////////////


///////////////Blur///////////////////////////////
static const float BlurWeights[13] =
{ 
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
    //	0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
};

int       g_iWeightWidth = 13;

Texture2D g_PrevBlurXTexture;
Texture2D g_PrevBlurYTexture;

/////////////////////////////////////////////////


//////////////////Fog/////////////////////////////
bool         g_bFog;
float4       g_vFogColor;
float        g_fFogDensity;
float        g_fFogTime;
float        g_fFogHeightStart;
float        g_fFogHeightEnd;
float        g_fFogDistStart;
float        g_fFogDistEnd;
bool         g_bNoiseFog;
float        g_fFogSpeed;
float        g_fNoiseScale;

Texture2D    g_FogNoiseTexture;
////////////////////////////////////////////////

/////////////////Motion Blur///////////////////////
Texture2D   g_OriginTexture;
matrix      g_PrevViewMatrix;

float       g_fMotionBlurStrength;
//////////////////////////////////////////////////


///////////////DOF///////////////////////////////
Texture2D   g_BlurredTexture;
float       g_fDOFMinDepth = 10.f; 
float       g_fDOFMaxDepth = 200.f;
//////////////////////////////////////////////////



float3 Calc_RimLight(float3 vRimColor, float3 vNormal, float3 vToCamera)
{
    float fRim = saturate(dot(vToCamera, vNormal));
    fRim = 1.f - fRim;
    
    fRim = smoothstep(0.f, 1.f, fRim);

    //fRim = fRim >= RimThresholdMin && fRim <= RimThresholdMax ? fRim : 0.f;
    
    fRim = pow(fRim, 2.f);
    
    return vRimColor * fRim;
}

float4 Get_WorldPosByDepth(float2 vTexcoord, float fDepth)
{
    float4 vWorldPos;
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = fDepth;
    vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * ������� */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vWorldPos = vWorldPos / vWorldPos.w;
    
    return vWorldPos;
}

float4 Get_ViewPosByDepth(float2 vTexcoord, float fDepth)
{
    float4 vViewPos;
    vViewPos.x = vTexcoord.x * 2.f - 1.f;
    vViewPos.y = vTexcoord.y * -2.f + 1.f;
    vViewPos.z = fDepth;
    vViewPos.w = 1.f;

    vViewPos = mul(vViewPos, g_ProjMatrixInv);
    vViewPos /= vViewPos.w;
    
    return vViewPos;
}

float Get_FogFactor(float4 vWorldPos, float fViewZ)
{
    float fCurHeight = vWorldPos.y;
    float fFogFactor = 0.5f;
    
    float fDistFactor = saturate((fViewZ - g_fFogDistStart) / (g_fFogDistEnd - g_fFogDistStart));
    
    if (true == g_bNoiseFog && fCurHeight < g_fFogHeightEnd)
    {
        float2 vNoiseUV = vWorldPos.xz * g_fNoiseScale + g_fFogTime * g_fFogSpeed;
        float fNoise = g_FogNoiseTexture.Sample(LinearWrapSampler, vNoiseUV);
   
        float fVerticalNoise = g_FogNoiseTexture.Sample(LinearWrapSampler, vWorldPos.xy * g_fNoiseScale * 0.5f + g_fFogTime * g_fFogSpeed * 0.7).r;
        fNoise = (fNoise + fVerticalNoise) * 0.5;
    
        //float fNoisyFog = fDistFactor * (1.f + (fNoise - 0.5f) * 0.3f);
         //
        fFogFactor = exp(-fDistFactor * fNoise * g_fFogDensity);
    }
    else
    {
        fFogFactor = saturate(exp(-fDistFactor * g_fFogDensity));
    }
    
    return fFogFactor;
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

    Out.vPosition = float4(In.vPosition, 1.f);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT VS_MAIN_DEBUG(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix MatWVP = mul(g_WorldMatrix, mul(g_ViewMatrix, g_ProjMatrix));
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), MatWVP);
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

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearWrapSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointClampSampler, In.vTexcoord);

	/* 0 ~ 1 -> -1 ~ 1 */
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
   
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient);
    Out.vShade *= g_fLightStrength;
    Out.vShade.a = 1.f;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointClampSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointClampSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vWorldPos;

	/* ������ġ * ������� * ����� * ������� * 1/w == ���������̽� ���� ��ġ�� ���ϳ�.*/
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * ������� */
    vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);

	/* ������ġ * ������� * ����� */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* ������ġ * ������� */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;

    float fDistance = length(vLightDir);

	/* �ȼ��� ������ġ�� ������ ��ġ�� ������ 1�� ������. ������ �������� �־����� 0����. */
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient) * fAtt;
    Out.vShade *= g_fLightStrength;

    //vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	
   // vector vLook = vWorldPos - g_vCamPosition;

    //Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

    return Out;
}

PS_OUT PS_MAIN_DEFERRED_PHONG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
 
    vector vShade = g_ShadeTexture.Sample(LinearClampSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade;
    
    Out.vColor = pow(Out.vColor, 2.2f);

    return Out;
}

float Calc_CascadeShadow(float4 vPosWorld, float fViewZ, float3 vNormal)
{
    float4 vLightTexcoord;
    
    for (uint i = 0; i < 3; ++i)
    {
        if (fViewZ < g_CascadeEnd[i])
        {
            vLightTexcoord = mul(vPosWorld, g_CascadeViewProj[i]);
            vLightTexcoord /= vLightTexcoord.w;
            
            vLightTexcoord.x = vLightTexcoord.x * 0.5f + 0.5f;
            vLightTexcoord.y = -vLightTexcoord.y * 0.5f + 0.5f;
         
            float fCurrentDepth = vLightTexcoord.z;
            float fBias = 0.0011f;
            float fShadow = 0.f;
            
            float3 vSamplePos = float3(vLightTexcoord.x, vLightTexcoord.y, i);
            
            fCurrentDepth -= fBias;
            
            for (int x = -1; x <= 1; ++x)
            {
                for (int y = -1; y <= 1; ++y)
                {
                    fShadow += g_CascadeShadowMap.SampleCmpLevelZero(ComparisionSampler, vSamplePos, fCurrentDepth, int2(x, y));
                }
            }
            
            fShadow /= 9.f;
            return fShadow;
        }
    }
      
    return 1.f;
}

float Calc_ShadowFactor(float4 vPosWorld)
{
    float fShadowFactor = 0.f;
    float4 vLightTexcoord = mul(vPosWorld, g_ShadowViewProj);
    vLightTexcoord /= vLightTexcoord.w;
    
    float fCurDepth = vLightTexcoord.z - g_fShadowBias;

    vLightTexcoord.x = vLightTexcoord.x * 0.5f + 0.5f;
    vLightTexcoord.y = -vLightTexcoord.y * 0.5f + 0.5f;
    
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            fShadowFactor += g_ShadowDepthTexture.SampleCmpLevelZero(ComparisionSampler, vLightTexcoord.xy, fCurDepth, int2(x, y));
        }
    }

    fShadowFactor /= 9.f;
    
    return fShadowFactor;
}

PS_OUT PS_MAIN_DEFERRED_PBR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a && false == g_bFog)
        discard;
    
    float4 vShade = g_ShadeTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vEmissive = pow(g_EmissiveTexture.Sample(LinearClampSampler, In.vTexcoord), 2.2f);
    float4 vRimLightColor = pow(g_RimLightTexture.Sample(LinearClampSampler, In.vTexcoord), 2.2f);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vNormalDesc = g_NormalTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vORM = g_ORMTexture.Sample(LinearClampSampler, In.vTexcoord);
        
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
        
    float4 vWorldPos = Get_WorldPosByDepth(In.vTexcoord, vDepthDesc.x);
    float fViewZ = vDepthDesc.y * g_fFar;
 
    
    float4 vRimLight = (float4) 0;
    float3 vToCamera = (g_vCamPosition - vWorldPos).xyz;
    if (0.f != vRimLightColor.a)
    {
        vToCamera = normalize(vToCamera);
        vRimLight = float4(Calc_RimLight(vRimLightColor.rgb, vNormal.xyz, vToCamera.xyz), 1.f);
    }
    
    float4 vAmbient = g_AmbientTexture.Sample(LinearClampSampler, In.vTexcoord);
  
    float fShadow = 1.f;;
    if (g_bShadow)
        fShadow = g_bCascade == true ? Calc_CascadeShadow(vWorldPos, fViewZ, vNormal.xyz) : Calc_ShadowFactor(vWorldPos);
    float fAO = g_bSSAO == true ? g_SSAOTexture.Sample(LinearClampSampler, In.vTexcoord).r : 1.f;
         
    Out.vColor = (vAmbient + ((vShade + vEmissive + vRimLight) * fShadow)) * fAO;
    Out.vColor.a = 1.f;
   
    //Out.vColor = pow(Out.vColor, 2.2f);
    
    if (g_bFog)
    {
        //float fFogMin = 5.f;
        //float fFogMax = 30.f;
        //
        //float distFog = saturate((fViewZ - fFogMin) / (fFogMax - fFogMin));
        //exp(-distFog * g_fFogStrength);
        float fFogFactor = Get_FogFactor(vWorldPos, fViewZ);
       
        float3 color = lerp(g_vFogColor.rgb, Out.vColor.rgb, fFogFactor);
    
        Out.vColor.rgb = color;
    }
    return Out;
}  

struct PS_OUT_PBR
{
    float4 vDirectLighting : SV_TARGET0;
    float4 vAmbient : SV_TARGET1;
};

PS_OUT_PBR PS_MAIN_DIRECTIONAL_PBR(PS_IN In)
{
    PS_OUT_PBR Out = (PS_OUT_PBR) Out;
  
    float4 vDiffuse = pow(g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord), 2.2f);
    float4 vNormalDesc = g_NormalTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vORM = g_ORMTexture.Sample(LinearClampSampler, In.vTexcoord);
  
    // (0, 1) -> (-1, 1)
    float4 vNormal = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f));

    vector vLightDir = normalize(g_vLightDir) * -1.f;

    float4 vWorldPos = Get_WorldPosByDepth(In.vTexcoord, vDepthDesc.x);
    
    float fRoughness = vORM.y;
    float fMetallic = vORM.z;

    float3 ToEye = normalize((g_vCamPosition - vWorldPos).xyz);
    float3 HalfWay = normalize(ToEye + vLightDir.xyz);

    float NdotL = max(0.1f, dot(vNormal.xyz, vLightDir.xyz));
    float NdotH = max(0.f, dot(vNormal.xyz, HalfWay));
    float NdotV = max(0.f, dot(vNormal.xyz, ToEye));
  
    float3 F0 = lerp(Fdielectric, vDiffuse.rgb, fMetallic);
    float3 F = SchlickFresnel(F0, max(0.f, dot(HalfWay, ToEye)));
    float3 kd = lerp(float3(1.f, 1.f, 1.f) - F, float3(0.f, 0.f, 0.f), fMetallic);
    float3 DiffuseBRDF = kd * vDiffuse.xyz;

    float D = NdfGGX(NdotH, fRoughness);
    float3 G = SchlickGGX(NdotL, NdotV, fRoughness);
    float3 SpecularBRDF = (F * D * G) / max(1e-5, 4.f * NdotL * NdotV);
  
    float3 Radiance = g_vLightDiffuse.rgb * g_fLightStrength;

    float3 DirectLighting = (DiffuseBRDF + SpecularBRDF) * Radiance * NdotL;
 
    Out.vDirectLighting = float4(DirectLighting, 1.f);
    Out.vAmbient = g_bIBL == true ? float4(AmbientLightingByIBL(vDiffuse.rgb, vNormal.rgb, ToEye, vORM.b, vORM.g), 1.f) * vORM.r * g_fAmbientStrength
     : vDiffuse * g_fAmbientStrength * vORM.r;
    
    Out.vAmbient.a = 1.f;
    
    return Out;
}


PS_OUT_PBR PS_MAIN_POINT_PBR(PS_IN In)
{
    PS_OUT_PBR Out = (PS_OUT_PBR) Out;
    
    float4 vDiffuse = pow(g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord), 2.2f);
    float4 vNormalDesc = g_NormalTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vORM = g_ORMTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    // (0, 1) -> (-1, 1)
    float4 vNormal = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f));
 
    float4 vWorldPos;
 
 // (0, 1) -> (-1, 1);
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

   // vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);
 
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    vWorldPos /= vWorldPos.w;
    
    vector vLightDir = g_vLightPos - vWorldPos;

    float fDistance = length(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    vLightDir = normalize(vLightDir);
 
    float fRoughness = vORM.y;
    float fMetallic = vORM.z;

    float3 ToEye = normalize((g_vCamPosition - vWorldPos).xyz);
    float3 HalfWay = normalize(ToEye + vLightDir.xyz); 
 
    float NdotL = max(0.1f, dot(vNormal.xyz, vLightDir.xyz));
    float NdotH = max(0.f, dot(vNormal.xyz, HalfWay));
    float NdotV = max(0.f, dot(vNormal.xyz, ToEye));
    
    float3 F0 = lerp(Fdielectric, vDiffuse.rgb, fMetallic);
    float3 F = SchlickFresnel(F0, max(0.f, dot(HalfWay, ToEye)));
    float3 kd = lerp(float3(1.f, 1.f, 1.f) - F, float3(0.f, 0.f, 0.f), fMetallic);
    float3 DiffuseBRDF = kd * vDiffuse.xyz;

    float D = NdfGGX(NdotH, fRoughness);
    float3 G = SchlickGGX(NdotL, NdotV, fRoughness);
    float3 SpecularBRDF = (F * D * G) / max(1e-5, 4.f * NdotL * NdotV);
 
    float3 Radiance = g_vLightDiffuse.rgb * g_fLightStrength * fAtt;
 
    float3 DirectLighting = (DiffuseBRDF + SpecularBRDF) * Radiance * NdotL;
    Out.vDirectLighting = float4(DirectLighting, 1.f);
    Out.vAmbient = g_bIBL == true ? float4(AmbientLightingByIBL(vDiffuse.rgb, vNormal.rgb, ToEye, vORM.b, vORM.g) * vORM.r * g_fAmbientStrength, 1.f)
    : vDiffuse * g_fAmbientStrength * vORM.r;
    
    Out.vAmbient *= fAtt;
  
    Out.vAmbient.a = 1.f;
      
    return Out;
}

PS_OUT_PBR PS_MAIN_SPOT_PBR(PS_IN In)
{
    PS_OUT_PBR Out = (PS_OUT_PBR) Out;

    float4 vDiffuse = pow(g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord), 2.2f);
    float4 vNormalDesc = g_NormalTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vORM = g_ORMTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    // (0, 1) -> (-1, 1)
    float4 vNormal = normalize(float4(vNormalDesc.xyz * 2.f - 1.f, 0.f));
 
    float4 vWorldPos;
 
 // (0, 1) -> (-1, 1);
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

   // vWorldPos = vWorldPos * (vDepthDesc.y * g_fFar);
 
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    vWorldPos /= vWorldPos.w;
    
    vector vLightDir = g_vLightPos - vWorldPos;

    float fDistance = length(vLightDir);
    vLightDir = normalize(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    vector vSpotDir = normalize(g_vLightDir);
    
    float fSpotFactor = pow(max(-dot(vLightDir, vSpotDir), 0.f), g_fSpotPower);
    
    float fRoughness = vORM.y;
    float fMetallic = vORM.z;

    float3 ToEye = normalize((g_vCamPosition - vWorldPos).xyz);
    float3 HalfWay = normalize(ToEye + vLightDir.xyz);
 
    float NdotL = max(0.1f, dot(vNormal.xyz, vLightDir.xyz));
    float NdotH = max(0.f, dot(vNormal.xyz, HalfWay));
    float NdotV = max(0.f, dot(vNormal.xyz, ToEye));
    
    float3 F0 = lerp(Fdielectric, vDiffuse.rgb, fMetallic);
    float3 F = SchlickFresnel(F0, max(0.f, dot(HalfWay, ToEye)));
    float3 kd = lerp(float3(1.f, 1.f, 1.f) - F, float3(0.f, 0.f, 0.f), fMetallic);
    float3 DiffuseBRDF = kd * vDiffuse.xyz;

    float D = NdfGGX(NdotH, fRoughness);
    float3 G = SchlickGGX(NdotL, NdotV, fRoughness);
    float3 SpecularBRDF = (F * D * G) / max(1e-5, 4.f * NdotL * NdotV);
 
    float3 Radiance = g_vLightDiffuse.rgb * g_fLightStrength * fAtt * fSpotFactor;
 
    float3 DirectLighting = (DiffuseBRDF + SpecularBRDF) * Radiance * NdotL;
    
    Out.vDirectLighting = float4(DirectLighting, 1.f);
    Out.vAmbient = g_bIBL == true ? float4(AmbientLightingByIBL(vDiffuse.rgb, vNormal.rgb, ToEye, vORM.b, vORM.g) * vORM.r * g_fAmbientStrength, 1.f)
    : vDiffuse * g_fAmbientStrength * vORM.r;
    Out.vAmbient *= fAtt * fSpotFactor;
    Out.vAmbient.a = 1.f;
    
    return Out;
}

float4 PS_MAIN_SSAO(PS_IN In) : SV_TARGET0
{
    vector vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord); // vDepthDesc.x = ProjPos.z /  ProjPos.w // vDepthDesc.y = ProjPos.w / Far
    vector vNormalDesc = g_NormalTexture.Sample(LinearClampSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    vNormal = normalize(mul(vNormal, g_ViewMatrix));
     
    vector vViewPos = Get_ViewPosByDepth(In.vTexcoord, vDepthDesc.x);

    float3 vRandomVec = g_SSAONoiseTexture.Sample(LinearWrapSampler, In.vTexcoord * g_vNoiseScale).xyz;
    
    float3 vTangent = normalize(vRandomVec - vNormal.xyz * dot(vRandomVec, vNormal.xyz));
    float3 vBiTangent = cross(vNormal.xyz, vTangent);
    float3x3 TBN = float3x3(vTangent, vBiTangent, vNormal.xyz);
    
    float fOcculusion = 0.f;
    [unroll]
    for (int i = 0; i < 16; ++i)
    {
        float3 vSample = vViewPos.xyz + mul(g_vSampleKernel[i].xyz, TBN) * g_fSampleRadius;
        float4 vSampleUV = mul(float4(vSample, 1.f), g_ProjMatrix);
        vSampleUV.xy /= vSampleUV.w;
        vSampleUV.xy = vSampleUV.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        
        float3 vSampleDir = normalize(vSample - vViewPos.xyz);
        float NdotS = max(dot(vNormal.xyz, vSampleDir), 0);
       
        float fSampleViewZ = g_DepthTexture.Sample(LinearClampSampler, vSampleUV.xy).y * g_fFar;

        float fRangeCheck = smoothstep(0.f, 1.f, g_fSampleRadius / abs(vViewPos.z - fSampleViewZ));
 
       //fOcculusion += (vSample.z >= fSampleViewZ + 0.025f ? 1.0 : 0.0) * fRangeCheck;
        fOcculusion += (vSample.z >= fSampleViewZ + 0.04f ? 1.0 : 0.0) * NdotS * fRangeCheck;
    }
   
    fOcculusion = 1.f - (fOcculusion / 16.0);
    fOcculusion = pow(fOcculusion, g_fSSAOPower);
    
    return fOcculusion;
}




float4 PS_MAIN_BlurX(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    float fdx = 1.f / 1280.f;
    
    int iHalf = g_iWeightWidth / 2;
    uint iIndex = 0;
   
    [unroll]
    for (int i = 0; i < g_iWeightWidth; ++i)
    {
        float4 vSampleColor = g_PrevBlurXTexture.Sample(LinearClampSampler, In.vTexcoord + float2(fdx, 0.f) * float(i - iHalf));
        vColor += BlurWeights[i] * vSampleColor;
    }
    
    vColor.rgb += float3(0.0001f, 0.0001f, 0.0001f);
    vColor /= 6.2108;
    vColor.a = 1.f;
    
    return vColor;
}


float4 PS_MAIN_BlurY(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    float fdy = 1.f / 720.f;
    
    int iHalf = g_iWeightWidth / 2;
    uint iIndex = 0;
   
    [unroll]
    for (int i = 0; i < g_iWeightWidth; ++i)
    {
        float4 vSampleColor = g_PrevBlurYTexture.Sample(LinearClampSampler, In.vTexcoord + float2(0.f, fdy) * float(i - iHalf));
        vColor += BlurWeights[i] * vSampleColor;
    }
    
    vColor.rgb += float3(0.0001f, 0.0001f, 0.0001f);
    vColor /= 6.2108;
    vColor.a = 1.f;
 
    return vColor;
}

float4 PS_MAIN_MOTIONBLUR(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    float4 vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    vWorldPos /= vWorldPos.w;
    
    vector vPrevTexcoord = mul(vWorldPos, g_PrevViewMatrix);
    vPrevTexcoord = mul(vPrevTexcoord, g_ProjMatrix);
    vPrevTexcoord /= vPrevTexcoord.w;
    vPrevTexcoord.xy = vPrevTexcoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
     
    float2 vVelocity = (In.vTexcoord - vPrevTexcoord.xy) * g_fMotionBlurStrength;
    //clip(length(vVelocity) - 0.001f);
    
    int iSampleCount = 8;
    
    float2 vForwardUV = In.vTexcoord;
    float2 vBackwardUV = In.vTexcoord;
    
    vColor = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    [unroll]
    for (int i = 0; i < iSampleCount; ++i)
    {
        vForwardUV += vVelocity;
        vBackwardUV -= vVelocity;
        
        vColor += g_OriginTexture.Sample(LinearClampSampler, vForwardUV);
        vColor += g_OriginTexture.Sample(LinearClampSampler, vBackwardUV);
    }
    
    vColor /= iSampleCount * 2 + 1;
    
    return vColor;
}

float4 SSRBinarySearch(float3 vRayDir, in float3 vHitCoord)
{
    float fDepth;
    for (int i = 0; i < SSR_BINARY_SEARCH_STEPS; i++)
    {
        float4 vProjCoord = mul(float4(vHitCoord, 1.f), g_ProjMatrix);
        vProjCoord.xy /= vProjCoord.w;
        vProjCoord.xy = vProjCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

        fDepth = g_DepthTexture.SampleLevel(PointClampSampler, vProjCoord.xy, 0).x;
        float3 vViewPos = Get_ViewPosByDepth(vProjCoord.xy, fDepth).xyz;
        float fDiff = vHitCoord.z - vViewPos.z;

        if (fDiff <= 0.0f)
            vHitCoord += vRayDir;

        vRayDir *= 0.5f;
        vHitCoord -= vRayDir;
    }

    float4 vProjCoord = mul(float4(vHitCoord, 1.0f), g_ProjMatrix);
    vProjCoord.xy /= vProjCoord.w;
    vProjCoord.xy = vProjCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    
    fDepth = g_DepthTexture.SampleLevel(PointClampSampler, vProjCoord.xy, 0).x;
    float3 vViewPos = Get_ViewPosByDepth(vProjCoord.xy, fDepth).xyz;
    float fDiff = vHitCoord.z - vViewPos.z;

    return float4(vProjCoord.xy, fDepth, abs(fDiff) < g_fSSRThreshold ? 1.f : 0.f);
}

float4 SSRRayMarch(float3 vRayDir, in float3 vHitCoord)
{
    float fDepth;
    for (int i = 0; i < SSR_MAX_STEPS; i++)
    {
        vHitCoord += vRayDir;
        float4 vProjCoord = mul(float4(vHitCoord, 1.0f), g_ProjMatrix);
        vProjCoord.xy /= vProjCoord.w;
        vProjCoord.xy = vProjCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

        fDepth = g_DepthTexture.SampleLevel(PointClampSampler, vProjCoord.xy, 0).x;
        float3 vViewPos = Get_ViewPosByDepth(vProjCoord.xy, fDepth).xyz;
        float fDiff = vHitCoord.z - vViewPos.z;

		[branch]
        if (fDiff > 0.0f)
        {
            return SSRBinarySearch(vRayDir, vHitCoord);
        }

        vRayDir *= g_fSSRStep;
    }
    return float4(0.0f, 0.0f, 0.f, 0.f);
}



float4 PS_MAIN_SSR(PS_IN In) : SV_TARGET0
{
    float fMask = g_DepthTexture.Sample(PointClampSampler, In.vTexcoord).w;
    clip(fMask - 0.1f);
    
    float4 vNormal = g_NormalTexture.Sample(LinearClampSampler, In.vTexcoord);
    vNormal = normalize(vector(vNormal.xyz * 2.f - 1.f, 0.f));
   
    float4 vDepth = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    float4 vViewPos = Get_ViewPosByDepth(In.vTexcoord, vDepth.x);   
    float4 vViewNormal = mul(vNormal, g_ViewMatrix);
    float3 vReflect = normalize(reflect(vViewPos.xyz, vViewNormal.xyz)).xyz;
    
    float4 vHitPos = vViewPos;
    float4 vCoords = SSRRayMarch(vReflect, vHitPos.xyz);
    
    float2 vCoordsEdgeFactor = float2(1, 1) - pow(saturate(abs(vCoords.xy - float2(0.5f, 0.5f)) * 2), 8);
    float vScreenEdgeFactor = saturate(min(vCoordsEdgeFactor.x, vCoordsEdgeFactor.y));
    float fReflectionIntensity = saturate(vScreenEdgeFactor * saturate(vReflect.z) * (vCoords.w));

    float4 vOriginColor = g_OriginTexture.SampleLevel(LinearClampSampler, vCoords.xy, 0);
    float4 vReflectionColor = fReflectionIntensity * vOriginColor;
    
    vReflectionColor.a = 0.6f;
    
    return vReflectionColor;

}

float4 PS_MAIN_DOF(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    float4 vDepthDesc = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    float4 vOrigin = g_PrevBlurXTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vBlurred = g_BlurredTexture.Sample(LinearClampSampler, In.vTexcoord);
	
    float fViewZ = (vDepthDesc.y * g_fFar);
    if (g_fFar - fViewZ < 1.f)
        vColor = vOrigin;
    else
    {
        float fLerpRatio = clamp((fViewZ - g_fDOFMinDepth) / (g_fDOFMaxDepth - g_fDOFMinDepth), 0.f, 1.f);
        vColor = lerp(vOrigin, vBlurred, fLerpRatio);
        vColor.a = 1.f;
    
    }


    return vColor;
}


technique11 DefaultTechnique
{
    pass RenderTarget_Debug // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN_DEBUG();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Deferred //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_PHONG();
    }

    pass Deferred_PBR //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_PBR();
    }

    pass DIRECTIONAL_PBR // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL_PBR();
    }

    pass POINT_PBR // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT_PBR();
    }


    pass SSAO // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }


    pass BlurX // 8
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

    pass BlurY // 9
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

    pass MotionBlur // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
    }

    pass SSR // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSR();
    }

    pass DOF //12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOF();
    }

    pass SPOT_PBR // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT_PBR();
    }

}


#pragma once

namespace Engine
{
	class CGameObject;
}

typedef struct tagRadialBlurDescs
{
	Engine::CGameObject*		pActor;
	float						fBlurStrength;
	float						fBlurRadius;
} RADIALBLUR_DESCS;

typedef struct tagFogDescs
{
	XMFLOAT4		vFogColor;
	float			fFogHeightStart;
	float			fFogHeightEnd;
	float			fFogDistStart;
	float			fFogDistEnd;
	float			fFogDensity;
	bool			bNoisedFog;
	float			fNoiseScale;
	float			fFogSpeed;
} FOG_DESCS;

typedef struct tagSkyDescs
{
	XMFLOAT4	vTopColor = { 0.f, 0.4f, 0.9f, 1.f };
	XMFLOAT4	vBottomColor = { 0.2f, 0.3f, 0.8f, 1.f };

	XMFLOAT3	vHorizon_Color = { 0.f, 0.7f, 0.8f };
	float		fHorizon_Blur = { 0.05f };

	XMFLOAT3	vSunDir;
	XMFLOAT3	vSunColor = XMFLOAT3(10.f, 8.f, 1.f);
	float		fSunSize = 2.f;
	float		fSunBlur = 10.f;

	XMFLOAT3	vCloud_EdgeColor = { 0.8f, 0.8f, 0.98f };
	XMFLOAT3	vCloud_TopColor = { 1.f, 1.f, 1.f };
	XMFLOAT3	vCloud_MiddleColor = { 0.92f, 0.92f, 0.98f };
	XMFLOAT3	vCloud_BottomColor = { 0.83f, 0.83f, 0.94f };
	float		fCloud_Speed = 2.f;
	float		fCloud_Direction = 0.2f;
	float		fCloud_Scale =  1.f;
	float		fCloud_CutOff =  0.3f;
	float		fCloud_Fuzziness =  0.5f;
	float		fCloud_Weight =  0.f;
	float		fCloud_Blur =  0.25f;

	bool operator!=(const tagSkyDescs& other) const {
		return memcmp(this, &other, sizeof(tagSkyDescs)) != 0;
	}

} SKY_DESCS;


typedef struct tagWaterDescs
{
	XMFLOAT2	vFlowSpeed = { 0.1f, 0.1f };
	float		fVoronoiScale = 200.f; // 18
	float		fWaterNoiseScale = 0.21f; //0.21f
	XMFLOAT4	fWaterColor = { 0.01f, 0.286667f, 1.f, 1.f };
	XMFLOAT4	fLightColor = { 0.7f, 0.7f, 0.7f, 1.f };
} WATER_DESCS;





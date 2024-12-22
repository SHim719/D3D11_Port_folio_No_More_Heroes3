#include "Effect.h"
#include "Bone.h"
#include "Transform.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_eEffect_Type{ rhs.m_eEffect_Type }
	, m_tCommon_Desc{ rhs.m_tCommon_Desc }
	, m_tCommon_UV_Desc{ rhs.m_tCommon_UV_Desc }
	, m_AttachWorldMat{ rhs.m_AttachWorldMat }
	, m_eTF_Type{ rhs.m_eTF_Type }
	, m_iTextureFlag{ rhs.m_iTextureFlag }
	, m_tAttachDesc{ rhs.m_tAttachDesc }
{
	if (strcmp(rhs.m_szEffectTag, "") && strcmp(rhs.m_szEffectTag, "Unknown"))
		strcpy_s(m_szEffectTag, MAX_PATH, rhs.m_szEffectTag);
	for (_uint i = 0; i < TEX_END; ++i)
	{
		if (strcmp(rhs.m_szTextureNames[i], ""))
		{
			strcpy_s(m_szTextureNames[i], MAX_PATH, rhs.m_szTextureNames[i]);
			Set_TextureName(TEX_TYPE(i), m_szTextureNames[i], (LEVEL)GET_CURLEVEL);
		}
	}

	if (strcmp(rhs.m_tAttachDesc.szBoneName, ""))
		strcpy_s(m_tAttachDesc.szBoneName, rhs.m_tAttachDesc.szBoneName);
	if (strcmp(rhs.m_tAttachDesc.szTargetBoneName, ""))
		strcpy_s(m_tAttachDesc.szTargetBoneName, rhs.m_tAttachDesc.szTargetBoneName);

	Safe_AddRef(m_tAttachDesc.pBone);
	Safe_AddRef(m_tAttachDesc.pParentTransform);
	Safe_AddRef(m_tAttachDesc.pTargetBone);
}

HRESULT CEffect::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_AttachWorldMat, XMMatrixIdentity());
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

void CEffect::PriorityTick(_float fDeltaTime)
{
}

void CEffect::Tick(_float fDeltaTime)
{
}

void CEffect::LateTick(_float fDeltaTime)
{
}

HRESULT CEffect::Render()
{
	return S_OK;
}

HRESULT CEffect::Save_Effect(const _char* pTag, nlohmann::ordered_json& JsonData)
{

	JsonData[pTag]["Effect Type"] = m_eEffect_Type;

	JsonData[pTag]["Common Description"]["iTransformType"] = m_tCommon_Desc.iTransformType;
	JsonData[pTag]["Common Description"]["iRenderGroup"] = m_tCommon_Desc.iRenderGroup;
	JsonData[pTag]["Common Description"]["iPassIdx"] = m_tCommon_Desc.iPassIdx;
	JsonData[pTag]["Common Description"]["bFllowOnlyPos"] = m_tCommon_Desc.bFllowOnlyPos;
	JsonData[pTag]["Common Description"]["bEffectLoop"] = m_tCommon_Desc.bEffectLoop;
	JsonData[pTag]["Common Description"]["bLerpSize"] = m_tCommon_Desc.bLerpSize;
	JsonData[pTag]["Common Description"]["vStartPosMin"] = { m_tCommon_Desc.vStartPosMin.x,m_tCommon_Desc.vStartPosMin.y,m_tCommon_Desc.vStartPosMin.z };
	JsonData[pTag]["Common Description"]["vStartPosMax"] = { m_tCommon_Desc.vStartPosMax.x,m_tCommon_Desc.vStartPosMax.y,m_tCommon_Desc.vStartPosMax.z };
	JsonData[pTag]["Common Description"]["vSpeedMinMax"] = { m_tCommon_Desc.vSpeedMinMax.x, m_tCommon_Desc.vSpeedMinMax.y };
	JsonData[pTag]["Common Description"]["fSpeedOverLifeTime"] = m_tCommon_Desc.fSpeedOverLifeTime;
	JsonData[pTag]["Common Description"]["iSpeedLerpRatioNum"] = m_tCommon_Desc.iSpeedLerpRatioNum;
	JsonData[pTag]["Common Description"]["vSpeedDirMin"] = { m_tCommon_Desc.vSpeedDirMin.x,m_tCommon_Desc.vSpeedDirMin.y,m_tCommon_Desc.vSpeedDirMin.z };
	JsonData[pTag]["Common Description"]["vSpeedDirMax"] = { m_tCommon_Desc.vSpeedDirMax.x,m_tCommon_Desc.vSpeedDirMax.y,m_tCommon_Desc.vSpeedDirMax.z };
	JsonData[pTag]["Common Description"]["bEnableColorOverLife"] = m_tCommon_Desc.bEnableColorOverLife;

	JsonData[pTag]["Common Description"]["vStartColorMin"] = { m_tCommon_Desc.vStartColorMin.x,m_tCommon_Desc.vStartColorMin.y,m_tCommon_Desc.vStartColorMin.z,m_tCommon_Desc.vStartColorMin.w };
	JsonData[pTag]["Common Description"]["vStartColorMax"] = { m_tCommon_Desc.vStartColorMax.x,m_tCommon_Desc.vStartColorMax.y,m_tCommon_Desc.vStartColorMax.z,m_tCommon_Desc.vStartColorMax.w };
	JsonData[pTag]["Common Description"]["vColorOverLife"] = { m_tCommon_Desc.vColorOverLife.x,m_tCommon_Desc.vColorOverLife.y,m_tCommon_Desc.vColorOverLife.z,m_tCommon_Desc.vColorOverLife.w };
	JsonData[pTag]["Common Description"]["vClipColor"] = { m_tCommon_Desc.vClipColor.x,m_tCommon_Desc.vClipColor.y,m_tCommon_Desc.vClipColor.z,m_tCommon_Desc.vClipColor.w };
	JsonData[pTag]["Common Description"]["vMultipleColor"] = { m_tCommon_Desc.vMultipleColor.x,m_tCommon_Desc.vMultipleColor.y,m_tCommon_Desc.vMultipleColor.z,m_tCommon_Desc.vMultipleColor.w };
	JsonData[pTag]["Common Description"]["vAddColor"] = { m_tCommon_Desc.vAddColor.x,m_tCommon_Desc.vAddColor.y,m_tCommon_Desc.vAddColor.z,m_tCommon_Desc.vAddColor.w };
	JsonData[pTag]["Common Description"]["vStartRotationMin"] = { m_tCommon_Desc.vStartRotationMin.x,m_tCommon_Desc.vStartRotationMin.y,m_tCommon_Desc.vStartRotationMin.z };
	JsonData[pTag]["Common Description"]["vStartRotationMax"] = { m_tCommon_Desc.vStartRotationMax.x,m_tCommon_Desc.vStartRotationMax.y,m_tCommon_Desc.vStartRotationMax.z };
	JsonData[pTag]["Common Description"]["vRotationOverLife"] = { m_tCommon_Desc.vRotationOverLife.x,m_tCommon_Desc.vRotationOverLife.y,m_tCommon_Desc.vRotationOverLife.z };
	JsonData[pTag]["Common Description"]["bRotationLocal"] = m_tCommon_Desc.bRotationLocal;
	JsonData[pTag]["Common Description"]["vPowerDirMin"] = { m_tCommon_Desc.vPowerDirMin.x,m_tCommon_Desc.vPowerDirMin.y,m_tCommon_Desc.vPowerDirMin.z };
	JsonData[pTag]["Common Description"]["vPowerDirMax"] = { m_tCommon_Desc.vPowerDirMax.x,m_tCommon_Desc.vPowerDirMax.y,m_tCommon_Desc.vPowerDirMax.z };

	JsonData[pTag]["Common Description"]["fLifeTimeMin"] = m_tCommon_Desc.fLifeTimeMin;
	JsonData[pTag]["Common Description"]["fLifeTimeMax"] = m_tCommon_Desc.fLifeTimeMax;
	JsonData[pTag]["Common Description"]["fTotalPlayTime"] = m_tCommon_Desc.fTotalPlayTime;
	JsonData[pTag]["Common Description"]["fStartDelayMin"] = m_tCommon_Desc.fStartDelayMin;
	JsonData[pTag]["Common Description"]["fStartDelayMax"] = m_tCommon_Desc.fStartDelayMax;
	JsonData[pTag]["Common Description"]["bUseDistortion"] = m_tCommon_Desc.bUseDistortion;
	JsonData[pTag]["Common Description"]["fDistortionPower"] = m_tCommon_Desc.fDistortionPower;
	JsonData[pTag]["Common Description"]["bDissolveReverse"] = m_tCommon_Desc.bDissolveReverse;
	JsonData[pTag]["Common Description"]["fDissolveSpeed"] = m_tCommon_Desc.fDissolveSpeed;
	JsonData[pTag]["Common Description"]["fDissolveStartTime"] = m_tCommon_Desc.fDissolveStartTime;

	JsonData[pTag]["Common Description"]["bUseGreaterThan_U"] = m_tCommon_Desc.bUseGreaterThan_U;
	JsonData[pTag]["Common Description"]["bUseGreaterThan_V"] = m_tCommon_Desc.bUseGreaterThan_V;
	JsonData[pTag]["Common Description"]["bUseLessThan_U"] = m_tCommon_Desc.bUseLessThan_U;
	JsonData[pTag]["Common Description"]["bUseLessThan_V"] = m_tCommon_Desc.bUseLessThan_V;
	JsonData[pTag]["Common Description"]["fGreaterCmpVal_U"] = m_tCommon_Desc.fGreaterCmpVal_U;
	JsonData[pTag]["Common Description"]["fGreaterCmpVal_V"] = m_tCommon_Desc.fGreaterCmpVal_V;
	JsonData[pTag]["Common Description"]["fLessCmpVal_U"] = m_tCommon_Desc.fLessCmpVal_U;
	JsonData[pTag]["Common Description"]["fLessCmpVal_V"] = m_tCommon_Desc.fLessCmpVal_V;
	JsonData[pTag]["Common Description"]["fClipFactor"] = m_tCommon_Desc.fClipFactor;

	JsonData[pTag]["Common Description"]["bBaseSprite"] = m_tCommon_Desc.bBaseSprite;
	JsonData[pTag]["Common Description"]["iBaseSpriteMaxRow"] = m_tCommon_Desc.iBaseSpriteMaxRow;
	JsonData[pTag]["Common Description"]["iBaseSpriteMaxCol"] = m_tCommon_Desc.iBaseSpriteMaxCol;
	JsonData[pTag]["Common Description"]["ChangeBaseSprite"] = m_tCommon_Desc.ChangeBaseSprite;
	JsonData[pTag]["Common Description"]["fBaseSpriteChangeTime"] = m_tCommon_Desc.fBaseSpriteChangeTime;
	JsonData[pTag]["Common Description"]["bMaskSprite"] = m_tCommon_Desc.bMaskSprite;
	JsonData[pTag]["Common Description"]["iMaskSpriteMaxRow"] = m_tCommon_Desc.iMaskSpriteMaxRow;
	JsonData[pTag]["Common Description"]["iMaskSpriteMaxCol"] = m_tCommon_Desc.iMaskSpriteMaxCol;
	JsonData[pTag]["Common Description"]["ChangeMaskSprite"] = m_tCommon_Desc.ChangeMaskSprite;
	JsonData[pTag]["Common Description"]["fMaskSpriteChangeTime"] = m_tCommon_Desc.fMaskSpriteChangeTime;
	JsonData[pTag]["Common Description"]["bProportionalAlphaToSpeed"] = m_tCommon_Desc.bProportionalAlphaToSpeed;
	JsonData[pTag]["Common Description"]["fProportionalAlphaMaxSpeed"] = m_tCommon_Desc.fProportionalAlphaMaxSpeed;
	JsonData[pTag]["Common Description"]["bNoiseMultiplyAlpha"] = m_tCommon_Desc.bNoiseMultiplyAlpha;

	JsonData[pTag]["Common Description"]["iDissolveFlag"] = m_tCommon_Desc.iDissolveFlag;
	JsonData[pTag]["Common Description"]["vEdgeColor"] = { m_tCommon_Desc.vEdgeColor.x,m_tCommon_Desc.vEdgeColor.y,m_tCommon_Desc.vEdgeColor.z,m_tCommon_Desc.vEdgeColor.w };
	JsonData[pTag]["Common Description"]["fStopDissolveAmount"] = m_tCommon_Desc.fStopDissolveAmount;
	JsonData[pTag]["Common Description"]["fDissolveLoopMin"] = m_tCommon_Desc.fDissolveLoopMin;
	JsonData[pTag]["Common Description"]["fDissolveLoopMax"] = m_tCommon_Desc.fDissolveLoopMax;
	JsonData[pTag]["Common Description"]["iDirDissolveFlag"] = m_tCommon_Desc.iDirDissolveFlag;
	JsonData[pTag]["Common Description"]["fDissolveEdgeWidth"] = m_tCommon_Desc.fDissolveEdgeWidth;

	JsonData[pTag]["Common UV Description"]["vStartBaseUVOffset"] = { m_tCommon_UV_Desc.vStartBaseUVOffset.x, m_tCommon_UV_Desc.vStartBaseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vBaseUVSpeed"] = { m_tCommon_UV_Desc.vBaseUVSpeed.x, m_tCommon_UV_Desc.vBaseUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinBaseUVOffset"] = { m_tCommon_UV_Desc.vMinBaseUVOffset.x, m_tCommon_UV_Desc.vMinBaseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxBaseUVOffset"] = { m_tCommon_UV_Desc.vMaxBaseUVOffset.x, m_tCommon_UV_Desc.vMaxBaseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iBaseSampler"] = m_tCommon_UV_Desc.iBaseSampler;

	JsonData[pTag]["Common UV Description"]["vStartAlphaMaskUVOffset"] = { m_tCommon_UV_Desc.vStartAlphaMaskUVOffset.x, m_tCommon_UV_Desc.vStartAlphaMaskUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vAlphaMaskUVSpeed"] = { m_tCommon_UV_Desc.vAlphaMaskUVSpeed.x, m_tCommon_UV_Desc.vAlphaMaskUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinAlphaMaskUVOffset"] = { m_tCommon_UV_Desc.vMinAlphaMaskUVOffset.x, m_tCommon_UV_Desc.vMinAlphaMaskUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxAlphaMaskUVOffset"] = { m_tCommon_UV_Desc.vMaxAlphaMaskUVOffset.x, m_tCommon_UV_Desc.vMaxAlphaMaskUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iAlphaMaskSampler"] = m_tCommon_UV_Desc.iAlphaMaskSampler;

	JsonData[pTag]["Common UV Description"]["vStartColorScaleUVOffset"] = { m_tCommon_UV_Desc.vStartColorScaleUVOffset.x, m_tCommon_UV_Desc.vStartColorScaleUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vColorScaleUVSpeed"] = { m_tCommon_UV_Desc.vColorScaleUVSpeed.x, m_tCommon_UV_Desc.vColorScaleUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinColorScaleUVOffset"] = { m_tCommon_UV_Desc.vMinColorScaleUVOffset.x, m_tCommon_UV_Desc.vMinColorScaleUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxColorScaleUVOffset"] = { m_tCommon_UV_Desc.vMaxColorScaleUVOffset.x, m_tCommon_UV_Desc.vMaxColorScaleUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iColorScaleUVSampler"] = m_tCommon_UV_Desc.iColorScaleUVSampler;

	JsonData[pTag]["Common UV Description"]["vStartNoiseUVOffset"] = { m_tCommon_UV_Desc.vStartNoiseUVOffset.x, m_tCommon_UV_Desc.vStartNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vNoiseUVSpeed"] = { m_tCommon_UV_Desc.vNoiseUVSpeed.x, m_tCommon_UV_Desc.vNoiseUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinNoiseUVOffset"] = { m_tCommon_UV_Desc.vMinNoiseUVOffset.x, m_tCommon_UV_Desc.vMinNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxNoiseUVOffset"] = { m_tCommon_UV_Desc.vMaxNoiseUVOffset.x, m_tCommon_UV_Desc.vMaxNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iNoiseSampler"] = m_tCommon_UV_Desc.iNoiseSampler;

	JsonData[pTag]["Common UV Description"]["vStartDissolveUVOffset"] = { m_tCommon_UV_Desc.vStartDissolveUVOffset.x, m_tCommon_UV_Desc.vStartDissolveUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vDissolveUVSpeed"] = { m_tCommon_UV_Desc.vDissolveUVSpeed.x, m_tCommon_UV_Desc.vDissolveUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinDissolveUVOffset"] = { m_tCommon_UV_Desc.vMinDissolveUVOffset.x, m_tCommon_UV_Desc.vMinDissolveUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxDissolveUVOffset"] = { m_tCommon_UV_Desc.vMaxDissolveUVOffset.x, m_tCommon_UV_Desc.vMaxDissolveUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iDissolveSampler"] = m_tCommon_UV_Desc.iDissolveSampler;

	JsonData[pTag]["Common UV Description"]["vStartBaseNoiseUVOffset"] = { m_tCommon_UV_Desc.vStartBaseNoiseUVOffset.x, m_tCommon_UV_Desc.vStartBaseNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vBaseNoiseUVSpeed"] = { m_tCommon_UV_Desc.vBaseNoiseUVSpeed.x, m_tCommon_UV_Desc.vBaseNoiseUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinBaseNoiseUVOffset"] = { m_tCommon_UV_Desc.vMinBaseNoiseUVOffset.x, m_tCommon_UV_Desc.vMinBaseNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxBaseNoiseUVOffset"] = { m_tCommon_UV_Desc.vMaxBaseNoiseUVOffset.x, m_tCommon_UV_Desc.vMaxBaseNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iBaseNoiseSampler"] = m_tCommon_UV_Desc.iBaseNoiseSampler;

	JsonData[pTag]["Common UV Description"]["vStartAlphaMaskNoiseUVOffset"] = { m_tCommon_UV_Desc.vStartAlphaMaskNoiseUVOffset.x, m_tCommon_UV_Desc.vStartAlphaMaskNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vAlphaMaskNoiseUVSpeed"] = { m_tCommon_UV_Desc.vAlphaMaskNoiseUVSpeed.x, m_tCommon_UV_Desc.vAlphaMaskNoiseUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinAlphaMaskNoiseUVOffset"] = { m_tCommon_UV_Desc.vMinAlphaMaskNoiseUVOffset.x, m_tCommon_UV_Desc.vMinAlphaMaskNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxAlphaMaskNoiseUVOffset"] = { m_tCommon_UV_Desc.vMaxAlphaMaskNoiseUVOffset.x, m_tCommon_UV_Desc.vMaxAlphaMaskNoiseUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iAlphaMaskNoiseSampler"] = m_tCommon_UV_Desc.iAlphaMaskNoiseSampler;

	JsonData[pTag]["Common UV Description"]["vStartDistortionUVOffset"] = { m_tCommon_UV_Desc.vStartDistortionUVOffset.x, m_tCommon_UV_Desc.vStartDistortionUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vDistortionUVSpeed"] = { m_tCommon_UV_Desc.vDistortionUVSpeed.x, m_tCommon_UV_Desc.vDistortionUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinDistortionUVOffset"] = { m_tCommon_UV_Desc.vMinDistortionUVOffset.x, m_tCommon_UV_Desc.vMinDistortionUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxDistortionUVOffset"] = { m_tCommon_UV_Desc.vMaxDistortionUVOffset.x, m_tCommon_UV_Desc.vMaxDistortionUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iDistortionSampler"] = m_tCommon_UV_Desc.iDistortionSampler;

	JsonData[pTag]["Common UV Description"]["vStartUVSliceClipUVOffset"] = { m_tCommon_UV_Desc.vStartUVSliceClipUVOffset.x, m_tCommon_UV_Desc.vStartUVSliceClipUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vUVSliceClipUVSpeed"] = { m_tCommon_UV_Desc.vUVSliceClipUVSpeed.x, m_tCommon_UV_Desc.vUVSliceClipUVSpeed.y };
	JsonData[pTag]["Common UV Description"]["vMinUVSliceClipUVOffset"] = { m_tCommon_UV_Desc.vMinUVSliceClipUVOffset.x, m_tCommon_UV_Desc.vMinUVSliceClipUVOffset.y };
	JsonData[pTag]["Common UV Description"]["vMaxUVSliceClipUVOffset"] = { m_tCommon_UV_Desc.vMaxUVSliceClipUVOffset.x, m_tCommon_UV_Desc.vMaxUVSliceClipUVOffset.y };
	JsonData[pTag]["Common UV Description"]["iUVSliceClipSampler"] = m_tCommon_UV_Desc.iUVSliceClipSampler;

	JsonData[pTag]["Common UV Description"]["iRadialMappingFlag"] = m_tCommon_UV_Desc.iRadialMappingFlag;
	JsonData[pTag]["Common UV Description"]["vBaseNoiseUVInfoFlag"] = { m_tCommon_UV_Desc.vBaseNoiseUVInfoFlag.x, m_tCommon_UV_Desc.vBaseNoiseUVInfoFlag.y, m_tCommon_UV_Desc.vBaseNoiseUVInfoFlag.z };
	JsonData[pTag]["Common UV Description"]["vAlphaNoiseUVInfoFlag"] = { m_tCommon_UV_Desc.vAlphaNoiseUVInfoFlag.x, m_tCommon_UV_Desc.vAlphaNoiseUVInfoFlag.y, m_tCommon_UV_Desc.vAlphaNoiseUVInfoFlag.z };
	JsonData[pTag]["Common UV Description"]["vBaseNoiseRadiusFlag"] = { m_tCommon_UV_Desc.vBaseNoiseRadiusFlag.x, m_tCommon_UV_Desc.vBaseNoiseRadiusFlag.y };
	JsonData[pTag]["Common UV Description"]["vAlphaNoiseRadiusFlag"] = { m_tCommon_UV_Desc.vAlphaNoiseRadiusFlag.x, m_tCommon_UV_Desc.vAlphaNoiseRadiusFlag.y };
	JsonData[pTag]["Common UV Description"]["vNoise_RadiusFlag"] = { m_tCommon_UV_Desc.vNoise_RadiusFlag.x, m_tCommon_UV_Desc.vNoise_RadiusFlag.y };
	JsonData[pTag]["Common UV Description"]["vDistortion_RadiusFlag"] = { m_tCommon_UV_Desc.vDistortion_RadiusFlag.x, m_tCommon_UV_Desc.vDistortion_RadiusFlag.y };


	JsonData[pTag]["iTextureFlag"] = m_iTextureFlag;

	JsonData[pTag]["Texture Component Tag"]["BASE"] = m_szTextureNames[TEX_BASE];
	JsonData[pTag]["Texture Component Tag"]["NORM"] = m_szTextureNames[TEX_NORM];
	JsonData[pTag]["Texture Component Tag"]["ALPHAMASK"] = m_szTextureNames[TEX_ALPHAMASK];
	JsonData[pTag]["Texture Component Tag"]["COLOR_SCALE_UV"] = m_szTextureNames[TEX_COLOR_SCALE_UV];
	JsonData[pTag]["Texture Component Tag"]["NOISE"] = m_szTextureNames[TEX_NOISE];
	JsonData[pTag]["Texture Component Tag"]["COLORSCALE"] = m_szTextureNames[TEX_COLORSCALE];
	JsonData[pTag]["Texture Component Tag"]["DISTORTION"] = m_szTextureNames[TEX_DISTORTION];
	JsonData[pTag]["Texture Component Tag"]["DISSOLVE"] = m_szTextureNames[TEX_DISSOLVE];
	JsonData[pTag]["Texture Component Tag"]["BASE_NOISE_UV"] = m_szTextureNames[TEX_BASE_NOISE_UV];
	JsonData[pTag]["Texture Component Tag"]["ALPHAMASK_NOISE_UV"] = m_szTextureNames[TEX_ALPHAMASK_NOISE_UV];
	JsonData[pTag]["Texture Component Tag"]["TEX_UV_SLICE_CLIP"] = m_szTextureNames[TEX_UV_SLICE_CLIP];

	JsonData[pTag]["Effect Tag"] = m_szEffectTag;

	JsonData[pTag]["Attach Description"]["Bone Name"] = m_tAttachDesc.szBoneName;
	JsonData[pTag]["Attach Description"]["Target Bone Name"] = m_tAttachDesc.szTargetBoneName;
	JsonData[pTag]["Attach Description"]["Bone Offset"] = { m_tAttachDesc.vOffSet.x, m_tAttachDesc.vOffSet.y, m_tAttachDesc.vOffSet.z };
	JsonData[pTag]["Attach Description"]["Only Position"] = m_tAttachDesc.bOnlyPosition;
	JsonData[pTag]["Attach Description"]["World Offset"] = m_tAttachDesc.bWorldOffset;

	return S_OK;
}

HRESULT CEffect::Load_Effect(const _char* pTag, json& JsonData)
{
	m_tCommon_Desc.iTransformType = JsonData[pTag]["Common Description"]["iTransformType"];
	m_tCommon_Desc.iRenderGroup = JsonData[pTag]["Common Description"]["iRenderGroup"];
	m_tCommon_Desc.iPassIdx = JsonData[pTag]["Common Description"]["iPassIdx"];
	m_tCommon_Desc.bFllowOnlyPos = JsonData[pTag]["Common Description"]["bFllowOnlyPos"];
	m_tCommon_Desc.bEffectLoop = JsonData[pTag]["Common Description"]["bEffectLoop"];
	m_tCommon_Desc.bLerpSize = JsonData[pTag]["Common Description"]["bLerpSize"];
	memcpy(&m_tCommon_Desc.vStartPosMin, JsonData[pTag]["Common Description"]["vStartPosMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vStartPosMax, JsonData[pTag]["Common Description"]["vStartPosMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vSpeedMinMax, JsonData[pTag]["Common Description"]["vSpeedMinMax"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_Desc.fSpeedOverLifeTime = JsonData[pTag]["Common Description"]["fSpeedOverLifeTime"];
	m_tCommon_Desc.iSpeedLerpRatioNum = JsonData[pTag]["Common Description"]["iSpeedLerpRatioNum"];
	memcpy(&m_tCommon_Desc.vSpeedDirMin, JsonData[pTag]["Common Description"]["vSpeedDirMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vSpeedDirMax, JsonData[pTag]["Common Description"]["vSpeedDirMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tCommon_Desc.bEnableColorOverLife = JsonData[pTag]["Common Description"]["bEnableColorOverLife"];

	memcpy(&m_tCommon_Desc.vStartColorMin, JsonData[pTag]["Common Description"]["vStartColorMin"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vStartColorMax, JsonData[pTag]["Common Description"]["vStartColorMax"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vColorOverLife, JsonData[pTag]["Common Description"]["vColorOverLife"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vClipColor, JsonData[pTag]["Common Description"]["vClipColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vMultipleColor, JsonData[pTag]["Common Description"]["vMultipleColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vAddColor, JsonData[pTag]["Common Description"]["vAddColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vStartRotationMin, JsonData[pTag]["Common Description"]["vStartRotationMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vStartRotationMax, JsonData[pTag]["Common Description"]["vStartRotationMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vRotationOverLife, JsonData[pTag]["Common Description"]["vRotationOverLife"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tCommon_Desc.bRotationLocal = JsonData[pTag]["Common Description"]["bRotationLocal"];						
	memcpy(&m_tCommon_Desc.vPowerDirMin, JsonData[pTag]["Common Description"]["vPowerDirMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vPowerDirMax, JsonData[pTag]["Common Description"]["vPowerDirMax"].get<array<_float, 3>>().data(), sizeof(_float3));

	m_tCommon_Desc.fLifeTimeMin = JsonData[pTag]["Common Description"]["fLifeTimeMin"];
	m_tCommon_Desc.fLifeTimeMax = JsonData[pTag]["Common Description"]["fLifeTimeMax"];
	m_tCommon_Desc.fTotalPlayTime = JsonData[pTag]["Common Description"]["fTotalPlayTime"];
	m_tCommon_Desc.fStartDelayMin = JsonData[pTag]["Common Description"]["fStartDelayMin"];
	m_tCommon_Desc.fStartDelayMax = JsonData[pTag]["Common Description"]["fStartDelayMax"];
	m_tCommon_Desc.bUseDistortion = JsonData[pTag]["Common Description"]["bUseDistortion"];
	m_tCommon_Desc.fDistortionPower = JsonData[pTag]["Common Description"]["fDistortionPower"];
	m_tCommon_Desc.bDissolveReverse = JsonData[pTag]["Common Description"]["bDissolveReverse"];
	m_tCommon_Desc.fDissolveSpeed = JsonData[pTag]["Common Description"]["fDissolveSpeed"];
	m_tCommon_Desc.fDissolveStartTime = JsonData[pTag]["Common Description"]["fDissolveStartTime"];

	m_tCommon_Desc.bUseGreaterThan_U = JsonData[pTag]["Common Description"]["bUseGreaterThan_U"];
	m_tCommon_Desc.bUseGreaterThan_V = JsonData[pTag]["Common Description"]["bUseGreaterThan_V"];
	m_tCommon_Desc.bUseLessThan_U = JsonData[pTag]["Common Description"]["bUseLessThan_U"];
	m_tCommon_Desc.bUseLessThan_V = JsonData[pTag]["Common Description"]["bUseLessThan_V"];
	m_tCommon_Desc.fGreaterCmpVal_U = JsonData[pTag]["Common Description"]["fGreaterCmpVal_U"];
	m_tCommon_Desc.fGreaterCmpVal_V = JsonData[pTag]["Common Description"]["fGreaterCmpVal_V"];
	m_tCommon_Desc.fLessCmpVal_U = JsonData[pTag]["Common Description"]["fLessCmpVal_U"];
	m_tCommon_Desc.fLessCmpVal_V = JsonData[pTag]["Common Description"]["fLessCmpVal_V"];
	m_tCommon_Desc.fClipFactor = JsonData[pTag]["Common Description"]["fClipFactor"];

	m_tCommon_Desc.bBaseSprite = JsonData[pTag]["Common Description"]["bBaseSprite"];
	m_tCommon_Desc.iBaseSpriteMaxRow = JsonData[pTag]["Common Description"]["iBaseSpriteMaxRow"];
	m_tCommon_Desc.iBaseSpriteMaxCol = JsonData[pTag]["Common Description"]["iBaseSpriteMaxCol"];
	m_tCommon_Desc.ChangeBaseSprite = JsonData[pTag]["Common Description"]["ChangeBaseSprite"];
	m_tCommon_Desc.fBaseSpriteChangeTime = JsonData[pTag]["Common Description"]["fBaseSpriteChangeTime"];
	m_tCommon_Desc.bMaskSprite = JsonData[pTag]["Common Description"]["bMaskSprite"];
	m_tCommon_Desc.iMaskSpriteMaxRow = JsonData[pTag]["Common Description"]["iMaskSpriteMaxRow"];
	m_tCommon_Desc.iMaskSpriteMaxCol = JsonData[pTag]["Common Description"]["iMaskSpriteMaxCol"];
	m_tCommon_Desc.ChangeMaskSprite = JsonData[pTag]["Common Description"]["ChangeMaskSprite"];
	m_tCommon_Desc.fMaskSpriteChangeTime = JsonData[pTag]["Common Description"]["fMaskSpriteChangeTime"];
	m_tCommon_Desc.bProportionalAlphaToSpeed = JsonData[pTag]["Common Description"]["bProportionalAlphaToSpeed"];
	m_tCommon_Desc.fProportionalAlphaMaxSpeed = JsonData[pTag]["Common Description"]["fProportionalAlphaMaxSpeed"];
	m_tCommon_Desc.bNoiseMultiplyAlpha = JsonData[pTag]["Common Description"]["bNoiseMultiplyAlpha"];

	m_tCommon_Desc.iDissolveFlag = JsonData[pTag]["Common Description"]["iDissolveFlag"];
	memcpy(&m_tCommon_Desc.vEdgeColor, JsonData[pTag]["Common Description"]["vEdgeColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	m_tCommon_Desc.fStopDissolveAmount = JsonData[pTag]["Common Description"]["fStopDissolveAmount"];
	m_tCommon_Desc.fDissolveLoopMin = JsonData[pTag]["Common Description"]["fDissolveLoopMin"];
	m_tCommon_Desc.fDissolveLoopMax = JsonData[pTag]["Common Description"]["fDissolveLoopMax"];
	m_tCommon_Desc.iDirDissolveFlag = JsonData[pTag]["Common Description"]["iDirDissolveFlag"];
	m_tCommon_Desc.fDissolveEdgeWidth = JsonData[pTag]["Common Description"]["fDissolveEdgeWidth"];

	memcpy(&m_tCommon_UV_Desc.vStartBaseUVOffset, JsonData[pTag]["Common UV Description"]["vStartBaseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vBaseUVSpeed, JsonData[pTag]["Common UV Description"]["vBaseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinBaseUVOffset, JsonData[pTag]["Common UV Description"]["vMinBaseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxBaseUVOffset, JsonData[pTag]["Common UV Description"]["vMaxBaseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iBaseSampler = JsonData[pTag]["Common UV Description"]["iBaseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartAlphaMaskUVOffset, JsonData[pTag]["Common UV Description"]["vStartAlphaMaskUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vAlphaMaskUVSpeed, JsonData[pTag]["Common UV Description"]["vAlphaMaskUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinAlphaMaskUVOffset, JsonData[pTag]["Common UV Description"]["vMinAlphaMaskUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxAlphaMaskUVOffset, JsonData[pTag]["Common UV Description"]["vMaxAlphaMaskUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iAlphaMaskSampler = JsonData[pTag]["Common UV Description"]["iAlphaMaskSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartColorScaleUVOffset, JsonData[pTag]["Common UV Description"]["vStartColorScaleUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vColorScaleUVSpeed, JsonData[pTag]["Common UV Description"]["vColorScaleUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinColorScaleUVOffset, JsonData[pTag]["Common UV Description"]["vMinColorScaleUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxColorScaleUVOffset, JsonData[pTag]["Common UV Description"]["vMaxColorScaleUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iColorScaleUVSampler = JsonData[pTag]["Common UV Description"]["iColorScaleUVSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vStartNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vNoiseUVSpeed, JsonData[pTag]["Common UV Description"]["vNoiseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vMinNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vMaxNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iNoiseSampler = JsonData[pTag]["Common UV Description"]["iNoiseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartDissolveUVOffset, JsonData[pTag]["Common UV Description"]["vStartDissolveUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vDissolveUVSpeed, JsonData[pTag]["Common UV Description"]["vDissolveUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinDissolveUVOffset, JsonData[pTag]["Common UV Description"]["vMinDissolveUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxDissolveUVOffset, JsonData[pTag]["Common UV Description"]["vMaxDissolveUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iDissolveSampler = JsonData[pTag]["Common UV Description"]["iDissolveSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartBaseNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vStartBaseNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vBaseNoiseUVSpeed, JsonData[pTag]["Common UV Description"]["vBaseNoiseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinBaseNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vMinBaseNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxBaseNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vMaxBaseNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iBaseNoiseSampler = JsonData[pTag]["Common UV Description"]["iBaseNoiseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartAlphaMaskNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vStartAlphaMaskNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vAlphaMaskNoiseUVSpeed, JsonData[pTag]["Common UV Description"]["vAlphaMaskNoiseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinAlphaMaskNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vMinAlphaMaskNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxAlphaMaskNoiseUVOffset, JsonData[pTag]["Common UV Description"]["vMaxAlphaMaskNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iAlphaMaskNoiseSampler = JsonData[pTag]["Common UV Description"]["iAlphaMaskNoiseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartDistortionUVOffset, JsonData[pTag]["Common UV Description"]["vStartDistortionUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vDistortionUVSpeed, JsonData[pTag]["Common UV Description"]["vDistortionUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinDistortionUVOffset, JsonData[pTag]["Common UV Description"]["vMinDistortionUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxDistortionUVOffset, JsonData[pTag]["Common UV Description"]["vMaxDistortionUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iDistortionSampler = JsonData[pTag]["Common UV Description"]["iDistortionSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartUVSliceClipUVOffset, JsonData[pTag]["Common UV Description"]["vStartUVSliceClipUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vUVSliceClipUVSpeed, JsonData[pTag]["Common UV Description"]["vUVSliceClipUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinUVSliceClipUVOffset, JsonData[pTag]["Common UV Description"]["vMinUVSliceClipUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxUVSliceClipUVOffset, JsonData[pTag]["Common UV Description"]["vMaxUVSliceClipUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iUVSliceClipSampler = JsonData[pTag]["Common UV Description"]["iUVSliceClipSampler"];

	m_tCommon_UV_Desc.iRadialMappingFlag = JsonData[pTag]["Common UV Description"]["iRadialMappingFlag"];
	memcpy(&m_tCommon_UV_Desc.vBaseNoiseUVInfoFlag, JsonData[pTag]["Common UV Description"]["vBaseNoiseUVInfoFlag"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_UV_Desc.vAlphaNoiseUVInfoFlag, JsonData[pTag]["Common UV Description"]["vAlphaNoiseUVInfoFlag"].get<array<_float, 3>>().data(), sizeof(_float3));

	memcpy(&m_tCommon_UV_Desc.vBaseNoiseRadiusFlag, JsonData[pTag]["Common UV Description"]["vBaseNoiseRadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vAlphaNoiseRadiusFlag, JsonData[pTag]["Common UV Description"]["vAlphaNoiseRadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vNoise_RadiusFlag, JsonData[pTag]["Common UV Description"]["vNoise_RadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vDistortion_RadiusFlag, JsonData[pTag]["Common UV Description"]["vDistortion_RadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));

	m_iTextureFlag = JsonData[pTag]["iTextureFlag"];

	strcpy_s(m_szTextureNames[TEX_BASE], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["BASE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_NORM], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["NORM"]).c_str());
	strcpy_s(m_szTextureNames[TEX_ALPHAMASK], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["ALPHAMASK"]).c_str());
	strcpy_s(m_szTextureNames[TEX_COLOR_SCALE_UV], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["COLOR_SCALE_UV"]).c_str());
	strcpy_s(m_szTextureNames[TEX_NOISE], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["NOISE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_COLORSCALE], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["COLORSCALE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_DISTORTION], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["DISTORTION"]).c_str());
	strcpy_s(m_szTextureNames[TEX_DISSOLVE], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["DISSOLVE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_BASE_NOISE_UV], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["BASE_NOISE_UV"]).c_str());
	strcpy_s(m_szTextureNames[TEX_ALPHAMASK_NOISE_UV], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["ALPHAMASK_NOISE_UV"]).c_str());
	strcpy_s(m_szTextureNames[TEX_UV_SLICE_CLIP], MAX_PATH, string(JsonData[pTag]["Texture Component Tag"]["TEX_UV_SLICE_CLIP"]).c_str());


	for (_uint i = 0; i < (_uint)TEX_TYPE::TEX_END; ++i)
	{
		if (strcmp(m_szTextureNames[i], ""))
		{
			Set_TextureName((TEX_TYPE)i, m_szTextureNames[i], LEVEL_ANIMTOOL);
		}
		else
		{
			if (m_iTextureFlag & (1 << (static_cast<_int>(i) + 1)))
				m_iTextureFlag ^= (1 << (static_cast<_int>(i) + 1));
		}
	}
	strcpy_s(m_szEffectTag, MAX_PATH, string(JsonData[pTag]["Effect Tag"]).c_str());

	strcpy_s(m_tAttachDesc.szBoneName, MAX_PATH, string(JsonData[pTag]["Attach Description"]["Bone Name"]).c_str());
	strcpy_s(m_tAttachDesc.szTargetBoneName, MAX_PATH, string(JsonData[pTag]["Attach Description"]["Target Bone Name"]).c_str());

	memcpy(&m_tAttachDesc.vOffSet, JsonData[pTag]["Attach Description"]["Bone Offset"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tAttachDesc.bOnlyPosition = JsonData[pTag]["Attach Description"]["Only Position"];
	m_tAttachDesc.bWorldOffset = JsonData[pTag]["Attach Description"]["World Offset"];

	return S_OK;
}
HRESULT CEffect::Load_Effect_CINEMA(const _char* pTag, json& JsonData)
{

	strcpy_s(m_szEffectTag, MAX_PATH, string(JsonData[pTag]["Effect Tag"]).c_str());

	m_tCommon_Desc.iTransformType = JsonData[m_szEffectTag]["Common Description"]["iTransformType"];
	m_tCommon_Desc.iRenderGroup = JsonData[m_szEffectTag]["Common Description"]["iRenderGroup"];
	m_tCommon_Desc.iPassIdx = JsonData[m_szEffectTag]["Common Description"]["iPassIdx"];
	m_tCommon_Desc.bFllowOnlyPos = JsonData[m_szEffectTag]["Common Description"]["bFllowOnlyPos"];
	m_tCommon_Desc.bEffectLoop = JsonData[m_szEffectTag]["Common Description"]["bEffectLoop"];
	m_tCommon_Desc.bLerpSize = JsonData[m_szEffectTag]["Common Description"]["bLerpSize"];
	memcpy(&m_tCommon_Desc.vStartPosMin, JsonData[m_szEffectTag]["Common Description"]["vStartPosMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vStartPosMax, JsonData[m_szEffectTag]["Common Description"]["vStartPosMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vSpeedMinMax, JsonData[m_szEffectTag]["Common Description"]["vSpeedMinMax"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_Desc.fSpeedOverLifeTime = JsonData[m_szEffectTag]["Common Description"]["fSpeedOverLifeTime"];
	m_tCommon_Desc.iSpeedLerpRatioNum = JsonData[m_szEffectTag]["Common Description"]["iSpeedLerpRatioNum"];
	memcpy(&m_tCommon_Desc.vSpeedDirMin, JsonData[m_szEffectTag]["Common Description"]["vSpeedDirMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vSpeedDirMax, JsonData[m_szEffectTag]["Common Description"]["vSpeedDirMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tCommon_Desc.bEnableColorOverLife = JsonData[m_szEffectTag]["Common Description"]["bEnableColorOverLife"];

	memcpy(&m_tCommon_Desc.vStartColorMin, JsonData[m_szEffectTag]["Common Description"]["vStartColorMin"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vStartColorMax, JsonData[m_szEffectTag]["Common Description"]["vStartColorMax"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vColorOverLife, JsonData[m_szEffectTag]["Common Description"]["vColorOverLife"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vClipColor, JsonData[m_szEffectTag]["Common Description"]["vClipColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vMultipleColor, JsonData[m_szEffectTag]["Common Description"]["vMultipleColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vAddColor, JsonData[m_szEffectTag]["Common Description"]["vAddColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tCommon_Desc.vStartRotationMin, JsonData[m_szEffectTag]["Common Description"]["vStartRotationMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vStartRotationMax, JsonData[m_szEffectTag]["Common Description"]["vStartRotationMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vRotationOverLife, JsonData[m_szEffectTag]["Common Description"]["vRotationOverLife"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tCommon_Desc.bRotationLocal = JsonData[m_szEffectTag]["Common Description"]["bRotationLocal"];
	memcpy(&m_tCommon_Desc.vPowerDirMin, JsonData[m_szEffectTag]["Common Description"]["vPowerDirMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_Desc.vPowerDirMax, JsonData[m_szEffectTag]["Common Description"]["vPowerDirMax"].get<array<_float, 3>>().data(), sizeof(_float3));

	m_tCommon_Desc.fLifeTimeMin = JsonData[m_szEffectTag]["Common Description"]["fLifeTimeMin"];
	m_tCommon_Desc.fLifeTimeMax = JsonData[m_szEffectTag]["Common Description"]["fLifeTimeMax"];
	m_tCommon_Desc.fTotalPlayTime = JsonData[m_szEffectTag]["Common Description"]["fTotalPlayTime"];
	m_tCommon_Desc.fStartDelayMin = JsonData[m_szEffectTag]["Common Description"]["fStartDelayMin"];
	m_tCommon_Desc.fStartDelayMax = JsonData[m_szEffectTag]["Common Description"]["fStartDelayMax"];
	m_tCommon_Desc.bUseDistortion = JsonData[m_szEffectTag]["Common Description"]["bUseDistortion"];
	m_tCommon_Desc.fDistortionPower = JsonData[m_szEffectTag]["Common Description"]["fDistortionPower"];
	m_tCommon_Desc.bDissolveReverse = JsonData[m_szEffectTag]["Common Description"]["bDissolveReverse"];
	m_tCommon_Desc.fDissolveSpeed = JsonData[m_szEffectTag]["Common Description"]["fDissolveSpeed"];
	m_tCommon_Desc.fDissolveStartTime = JsonData[m_szEffectTag]["Common Description"]["fDissolveStartTime"];

	m_tCommon_Desc.bUseGreaterThan_U = JsonData[m_szEffectTag]["Common Description"]["bUseGreaterThan_U"];
	m_tCommon_Desc.bUseGreaterThan_V = JsonData[m_szEffectTag]["Common Description"]["bUseGreaterThan_V"];
	m_tCommon_Desc.bUseLessThan_U = JsonData[m_szEffectTag]["Common Description"]["bUseLessThan_U"];
	m_tCommon_Desc.bUseLessThan_V = JsonData[m_szEffectTag]["Common Description"]["bUseLessThan_V"];
	m_tCommon_Desc.fGreaterCmpVal_U = JsonData[m_szEffectTag]["Common Description"]["fGreaterCmpVal_U"];
	m_tCommon_Desc.fGreaterCmpVal_V = JsonData[m_szEffectTag]["Common Description"]["fGreaterCmpVal_V"];
	m_tCommon_Desc.fLessCmpVal_U = JsonData[m_szEffectTag]["Common Description"]["fLessCmpVal_U"];
	m_tCommon_Desc.fLessCmpVal_V = JsonData[m_szEffectTag]["Common Description"]["fLessCmpVal_V"];
	m_tCommon_Desc.fClipFactor = JsonData[m_szEffectTag]["Common Description"]["fClipFactor"];

	m_tCommon_Desc.bBaseSprite = JsonData[m_szEffectTag]["Common Description"]["bBaseSprite"];
	m_tCommon_Desc.iBaseSpriteMaxRow = JsonData[m_szEffectTag]["Common Description"]["iBaseSpriteMaxRow"];
	m_tCommon_Desc.iBaseSpriteMaxCol = JsonData[m_szEffectTag]["Common Description"]["iBaseSpriteMaxCol"];
	m_tCommon_Desc.ChangeBaseSprite = JsonData[m_szEffectTag]["Common Description"]["ChangeBaseSprite"];
	m_tCommon_Desc.fBaseSpriteChangeTime = JsonData[m_szEffectTag]["Common Description"]["fBaseSpriteChangeTime"];
	m_tCommon_Desc.bMaskSprite = JsonData[m_szEffectTag]["Common Description"]["bMaskSprite"];
	m_tCommon_Desc.iMaskSpriteMaxRow = JsonData[m_szEffectTag]["Common Description"]["iMaskSpriteMaxRow"];
	m_tCommon_Desc.iMaskSpriteMaxCol = JsonData[m_szEffectTag]["Common Description"]["iMaskSpriteMaxCol"];
	m_tCommon_Desc.ChangeMaskSprite = JsonData[m_szEffectTag]["Common Description"]["ChangeMaskSprite"];
	m_tCommon_Desc.fMaskSpriteChangeTime = JsonData[m_szEffectTag]["Common Description"]["fMaskSpriteChangeTime"];
	m_tCommon_Desc.bProportionalAlphaToSpeed = JsonData[m_szEffectTag]["Common Description"]["bProportionalAlphaToSpeed"];
	m_tCommon_Desc.fProportionalAlphaMaxSpeed = JsonData[m_szEffectTag]["Common Description"]["fProportionalAlphaMaxSpeed"];
	m_tCommon_Desc.bNoiseMultiplyAlpha = JsonData[m_szEffectTag]["Common Description"]["bNoiseMultiplyAlpha"];

	m_tCommon_Desc.iDissolveFlag = JsonData[m_szEffectTag]["Common Description"]["iDissolveFlag"];
	memcpy(&m_tCommon_Desc.vEdgeColor, JsonData[m_szEffectTag]["Common Description"]["vEdgeColor"].get<array<_float, 4>>().data(), sizeof(_float4));
	m_tCommon_Desc.fStopDissolveAmount = JsonData[m_szEffectTag]["Common Description"]["fStopDissolveAmount"];
	m_tCommon_Desc.fDissolveLoopMin = JsonData[m_szEffectTag]["Common Description"]["fDissolveLoopMin"];
	m_tCommon_Desc.fDissolveLoopMax = JsonData[m_szEffectTag]["Common Description"]["fDissolveLoopMax"];
	m_tCommon_Desc.iDirDissolveFlag = JsonData[m_szEffectTag]["Common Description"]["iDirDissolveFlag"];

	memcpy(&m_tCommon_UV_Desc.vStartBaseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartBaseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vBaseUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vBaseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinBaseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinBaseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxBaseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxBaseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iBaseSampler = JsonData[m_szEffectTag]["Common UV Description"]["iBaseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartAlphaMaskUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartAlphaMaskUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vAlphaMaskUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vAlphaMaskUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinAlphaMaskUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinAlphaMaskUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxAlphaMaskUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxAlphaMaskUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iAlphaMaskSampler = JsonData[m_szEffectTag]["Common UV Description"]["iAlphaMaskSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartColorScaleUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartColorScaleUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vColorScaleUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vColorScaleUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinColorScaleUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinColorScaleUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxColorScaleUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxColorScaleUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iColorScaleUVSampler = JsonData[m_szEffectTag]["Common UV Description"]["iColorScaleUVSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vNoiseUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vNoiseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iNoiseSampler = JsonData[m_szEffectTag]["Common UV Description"]["iNoiseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartDissolveUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartDissolveUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vDissolveUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vDissolveUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinDissolveUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinDissolveUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxDissolveUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxDissolveUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iDissolveSampler = JsonData[m_szEffectTag]["Common UV Description"]["iDissolveSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartBaseNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartBaseNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vBaseNoiseUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vBaseNoiseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinBaseNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinBaseNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxBaseNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxBaseNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iBaseNoiseSampler = JsonData[m_szEffectTag]["Common UV Description"]["iBaseNoiseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartAlphaMaskNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartAlphaMaskNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vAlphaMaskNoiseUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vAlphaMaskNoiseUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinAlphaMaskNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinAlphaMaskNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxAlphaMaskNoiseUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxAlphaMaskNoiseUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iAlphaMaskNoiseSampler = JsonData[m_szEffectTag]["Common UV Description"]["iAlphaMaskNoiseSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartDistortionUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartDistortionUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vDistortionUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vDistortionUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinDistortionUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinDistortionUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxDistortionUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxDistortionUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iDistortionSampler = JsonData[m_szEffectTag]["Common UV Description"]["iDistortionSampler"];

	memcpy(&m_tCommon_UV_Desc.vStartUVSliceClipUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vStartUVSliceClipUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vUVSliceClipUVSpeed, JsonData[m_szEffectTag]["Common UV Description"]["vUVSliceClipUVSpeed"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMinUVSliceClipUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMinUVSliceClipUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vMaxUVSliceClipUVOffset, JsonData[m_szEffectTag]["Common UV Description"]["vMaxUVSliceClipUVOffset"].get<array<_float, 2>>().data(), sizeof(_float2));
	m_tCommon_UV_Desc.iUVSliceClipSampler = JsonData[m_szEffectTag]["Common UV Description"]["iUVSliceClipSampler"];

	m_tCommon_UV_Desc.iRadialMappingFlag = JsonData[m_szEffectTag]["Common UV Description"]["iRadialMappingFlag"];
	memcpy(&m_tCommon_UV_Desc.vBaseNoiseUVInfoFlag, JsonData[m_szEffectTag]["Common UV Description"]["vBaseNoiseUVInfoFlag"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tCommon_UV_Desc.vAlphaNoiseUVInfoFlag, JsonData[m_szEffectTag]["Common UV Description"]["vAlphaNoiseUVInfoFlag"].get<array<_float, 3>>().data(), sizeof(_float3));

	memcpy(&m_tCommon_UV_Desc.vBaseNoiseRadiusFlag, JsonData[m_szEffectTag]["Common UV Description"]["vBaseNoiseRadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vAlphaNoiseRadiusFlag, JsonData[m_szEffectTag]["Common UV Description"]["vAlphaNoiseRadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vNoise_RadiusFlag, JsonData[m_szEffectTag]["Common UV Description"]["vNoise_RadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tCommon_UV_Desc.vDistortion_RadiusFlag, JsonData[m_szEffectTag]["Common UV Description"]["vDistortion_RadiusFlag"].get<array<_float, 2>>().data(), sizeof(_float2));

	m_iTextureFlag = JsonData[m_szEffectTag]["iTextureFlag"];

	strcpy_s(m_szTextureNames[TEX_BASE], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["BASE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_NORM], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["NORM"]).c_str());
	strcpy_s(m_szTextureNames[TEX_ALPHAMASK], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["ALPHAMASK"]).c_str());
	strcpy_s(m_szTextureNames[TEX_COLOR_SCALE_UV], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["COLOR_SCALE_UV"]).c_str());
	strcpy_s(m_szTextureNames[TEX_NOISE], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["NOISE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_COLORSCALE], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["COLORSCALE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_DISTORTION], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["DISTORTION"]).c_str());
	strcpy_s(m_szTextureNames[TEX_DISSOLVE], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["DISSOLVE"]).c_str());
	strcpy_s(m_szTextureNames[TEX_BASE_NOISE_UV], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["BASE_NOISE_UV"]).c_str());
	strcpy_s(m_szTextureNames[TEX_ALPHAMASK_NOISE_UV], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["ALPHAMASK_NOISE_UV"]).c_str());
	strcpy_s(m_szTextureNames[TEX_UV_SLICE_CLIP], MAX_PATH, string(JsonData[m_szEffectTag]["Texture Component Tag"]["TEX_UV_SLICE_CLIP"]).c_str());

	for (_uint i = 0; i < (_uint)TEX_TYPE::TEX_END; ++i)
	{
		if (strcmp(m_szTextureNames[i], ""))
		{
			Set_TextureName((TEX_TYPE)i, m_szTextureNames[i], LEVEL_CINEMATOOL);
		}
		else
		{
			if (m_iTextureFlag & (1 << (static_cast<_int>(i) + 1)))
				m_iTextureFlag ^= (1 << (static_cast<_int>(i) + 1));
		}
	}

	strcpy_s(m_tAttachDesc.szBoneName, MAX_PATH, string(JsonData[m_szEffectTag]["Attach Description"]["Bone Name"]).c_str());
	strcpy_s(m_tAttachDesc.szTargetBoneName, MAX_PATH, string(JsonData[m_szEffectTag]["Attach Description"]["Target Bone Name"]).c_str());

	memcpy(&m_tAttachDesc.vOffSet, JsonData[m_szEffectTag]["Attach Description"]["Bone Offset"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tAttachDesc.bOnlyPosition = JsonData[m_szEffectTag]["Attach Description"]["Only Position"];
	m_tAttachDesc.bWorldOffset = JsonData[m_szEffectTag]["Attach Description"]["World Offset"];

	m_eTF_Type = (TRANSFORM_TYPE)m_tCommon_Desc.iTransformType;
	m_iPassIdx = m_tCommon_Desc.iPassIdx;

	m_bReturnToPool = true;
	return S_OK;
}
void CEffect::Reset_Effect(CINE_EFFECT_DESC* pDesc)
{
	m_isEnd = false;
	m_fTimeAcc = 0.f;

	m_fDissolveAmount = false == m_tCommon_Desc.bDissolveReverse ? 0.f : 1.f;
	m_fDissolveTimeAcc = 0.f;
	m_iBaseSpriteIndex = 0;
	m_iMaskSpriteIndex = 0;
	m_fBaseSpriteTimeAcc = 0.f;
	m_fMaskSpriteTimeAcc = 0.f;
	m_bDissolveLoopUp = true;

	if (nullptr != pDesc)
	{
		Safe_Release(m_tAttachDesc.pParentTransform);
		Safe_Release(m_tAttachDesc.pBone);
		Safe_Release(m_tAttachDesc.pTargetBone);

		m_tAttachDesc.pParentTransform = pDesc->pParentTransform;

		Safe_AddRef(m_tAttachDesc.pParentTransform);

		if (strcmp(m_tAttachDesc.szBoneName, ""))
		{
			m_tAttachDesc.pBone = pDesc->pParentModel->Get_Bone(m_tAttachDesc.szBoneName);
			Safe_AddRef(m_tAttachDesc.pBone);
		}
		if (strcmp(m_tAttachDesc.szTargetBoneName, ""))
		{
			m_tAttachDesc.pTargetBone = pDesc->pParentModel->Get_Bone(m_tAttachDesc.szTargetBoneName);
			Safe_AddRef(m_tAttachDesc.pTargetBone);
		}
	}
}
#pragma warning(default:6031)

void CEffect::Attach_Update()
{
	if (FIX == m_eTF_Type)
		return;

	_matrix WorldMat = XMMatrixIdentity();

	if (nullptr == m_tAttachDesc.pBone)
	{
		if (nullptr != m_tAttachDesc.pParentTransform)
		{
			CALC_TF->Set_WorldMatrix(m_tAttachDesc.pParentTransform->Get_WorldMatrix());
			CALC_TF->Set_Scale(1.f);
			WorldMat = CALC_TF->Get_WorldMatrix();
		}
	}
	else
	{
		if (true == m_tAttachDesc.bOnlyPosition)
		{

			CALC_TF->Set_WorldMatrix(m_tAttachDesc.pParentTransform->Get_WorldMatrix());
			CALC_TF->Attach_To_Bone(m_tAttachDesc.pBone, m_tAttachDesc.pParentTransform, XMMatrixIdentity(), true);
		}
		else
			CALC_TF->Attach_To_Bone(m_tAttachDesc.pBone, m_tAttachDesc.pParentTransform);

		WorldMat = CALC_TF->Get_WorldMatrix();

	}

	CALC_TF->Set_WorldMatrix(WorldMat);
	if (false == m_tAttachDesc.bWorldOffset)
		CALC_TF->Add_Position(XMLoadFloat3(&m_tAttachDesc.vOffSet), true);
	else
		CALC_TF->Add_Position(XMLoadFloat3(&m_tAttachDesc.vOffSet), false);

	m_AttachWorldMat = CALC_TF->Get_WorldFloat4x4();
}

void CEffect::Reset_Setting()
{
	CALC_TF->Set_WorldMatrix(XMMatrixIdentity());
	if (nullptr != m_tAttachDesc.pParentTransform)
	{
		if (strcmp(m_tAttachDesc.szBoneName, ""))
		{
			if (m_tAttachDesc.bOnlyPosition)
			{
				CALC_TF->Set_WorldMatrix(m_tAttachDesc.pParentTransform->Get_WorldMatrix());
				CALC_TF->Attach_To_Bone(m_tAttachDesc.pBone, m_tAttachDesc.pParentTransform, XMMatrixIdentity(), true);
			}
			else
				CALC_TF->Attach_To_Bone(m_tAttachDesc.pBone, m_tAttachDesc.pParentTransform);
		}
		else
		{
			CALC_TF->Set_WorldMatrix(m_tAttachDesc.pParentTransform->Get_WorldMatrix());
			CALC_TF->Set_Scale(_float3(1.f, 1.f, 1.f));
		}
	}

	if (false == m_tAttachDesc.bWorldOffset)
		CALC_TF->Add_Position(XMLoadFloat3(&m_tAttachDesc.vOffSet), true);
	else
		CALC_TF->Add_Position(XMLoadFloat3(&m_tAttachDesc.vOffSet), false);

	m_AttachWorldMat = CALC_TF->Get_WorldFloat4x4();

	//m_isEnd = false;
	m_fTimeAcc = 0.f;


	m_vAlphaMaskUVOffset = m_tCommon_UV_Desc.vStartAlphaMaskUVOffset;
	m_vColorScaleUVOffset = m_tCommon_UV_Desc.vStartColorScaleUVOffset;
	m_vNoiseUVOffset = m_tCommon_UV_Desc.vStartNoiseUVOffset;
	m_vBaseUVOffset = m_tCommon_UV_Desc.vStartBaseUVOffset;
	m_vBaseNoiseUVOffset = m_tCommon_UV_Desc.vStartBaseNoiseUVOffset;
	m_vAlphaMaskNoiseUVOffset = m_tCommon_UV_Desc.vStartAlphaMaskNoiseUVOffset;
	m_vDissolveUVOffset = m_tCommon_UV_Desc.vStartDissolveUVOffset;
	m_vDistortionUVOffset = m_tCommon_UV_Desc.vStartDistortionUVOffset;
	m_vUVSliceClipUVOffset = m_tCommon_UV_Desc.vStartDistortionUVOffset;
	m_vBaseUVOffset = m_tCommon_UV_Desc.vStartDistortionUVOffset;
	m_fDissolveAmount = false == m_tCommon_Desc.bDissolveReverse ? 0.f : 1.f;
	m_fDissolveTimeAcc = 0.f;

	m_iBaseSpriteIndex = 0;
	m_iMaskSpriteIndex = 0;
	m_fBaseSpriteTimeAcc = 0.f;
	m_fMaskSpriteTimeAcc = 0.f;
	m_bDissolveLoopUp = true;
}

void CEffect::Update_Effet_Desc(const HR::EFFECT_COMMON_DESC& tDesc)
{
	memcpy(&m_tCommon_Desc, &tDesc, sizeof(HR::EFFECT_COMMON_DESC));
	m_eTF_Type = (TRANSFORM_TYPE)m_tCommon_Desc.iTransformType;
	m_iPassIdx = m_tCommon_Desc.iPassIdx;
	m_isEnd = false;
}

void CEffect::Update_UV_Desc(const HR::EFFECT_COMMON_UV_DESC& tDesc)
{
	memcpy(&m_tCommon_UV_Desc, &tDesc, sizeof(HR::EFFECT_COMMON_UV_DESC));
}

void CEffect::Set_TextureName(TEX_TYPE eType, const _char* pTag, LEVEL eLevel)
{
	if (!strcmp(pTag, ""))
		return;

	strcpy_s(m_szTextureNames[eType], MAX_PATH, pTag);
	wstring wszPrototypeTag;
	wszPrototypeTag = Convert_StrToWStr(pTag);

	if (nullptr != m_pTexture[eType])
		Safe_Release(m_pTexture[eType]);

	m_pTexture[eType] = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(eLevel, wszPrototypeTag));

	if (0 == (m_iTextureFlag & (1 << (static_cast<_int>(eType) + 1))))
		m_iTextureFlag |= (1 << (static_cast<_int>(eType) + 1));
	//m_TextureUse[eType] = 1;
}

void CEffect::Set_AttachDesc(HR::ATTACH_DESC tDesc)
{
	Safe_Release(m_tAttachDesc.pBone);
	Safe_Release(m_tAttachDesc.pParentTransform);
	Safe_Release(m_tAttachDesc.pTargetBone);
	m_tAttachDesc = tDesc;
	if (m_tAttachDesc.pBone)
		Safe_AddRef(m_tAttachDesc.pBone);
	if (m_tAttachDesc.pParentTransform)
		Safe_AddRef(m_tAttachDesc.pParentTransform);
	if (m_tAttachDesc.pTargetBone)
		Safe_AddRef(m_tAttachDesc.pTargetBone);
}

void CEffect::Set_ParentInfo(CTransform* pParentTransform, CModel* pModel)
{
	if (nullptr == pParentTransform)
		return;

	Safe_Release(m_tAttachDesc.pParentTransform);
	Safe_Release(m_tAttachDesc.pBone);
	Safe_Release(m_tAttachDesc.pTargetBone);


	m_tAttachDesc.pParentTransform = pParentTransform;
	Safe_AddRef(pParentTransform);
	if (strcmp(m_tAttachDesc.szBoneName, ""))
	{
		m_tAttachDesc.pBone = pModel->Get_Bone(m_tAttachDesc.szBoneName);
		Safe_AddRef(m_tAttachDesc.pBone);
	}
	if (strcmp(m_tAttachDesc.szTargetBoneName, ""))
	{
		m_tAttachDesc.pTargetBone = pModel->Get_Bone(m_tAttachDesc.szTargetBoneName);
		Safe_AddRef(m_tAttachDesc.pTargetBone);
	}
}

void CEffect::Remove_Texture(TEX_TYPE eType)
{
	Safe_Release(m_pTexture[eType]);
	if (m_iTextureFlag & (1 << (static_cast<_int>(eType) + 1)))
		m_iTextureFlag ^= (1 << (static_cast<_int>(eType) + 1));

	strcpy_s(m_szTextureNames[eType], MAX_PATH, "");
}

void CEffect::Update_TexCoord(_float fTimeDelta)
{
	if (BASE & m_iTextureFlag)
	{

		m_vBaseUVOffset.x += m_tCommon_UV_Desc.vBaseUVSpeed.x * fTimeDelta;
		m_vBaseUVOffset.y += m_tCommon_UV_Desc.vBaseUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iBaseSampler) // Clamp
		{
			m_vBaseUVOffset.x = clamp(m_vBaseUVOffset.x, m_tCommon_UV_Desc.vMinBaseUVOffset.x, m_tCommon_UV_Desc.vMaxBaseUVOffset.x);
			m_vBaseUVOffset.y = clamp(m_vBaseUVOffset.y, m_tCommon_UV_Desc.vMinBaseUVOffset.y, m_tCommon_UV_Desc.vMaxBaseUVOffset.y);
		}
		else // Wrap
		{
			if (fabsf(m_vBaseUVOffset.x) >= 9999.f)
				m_vBaseUVOffset.x = 0.f;
			if (fabsf(m_vBaseUVOffset.y) >= 9999.f)
				m_vBaseUVOffset.y = 0.f;
		}
	}

	if (ALPHA_TEX & m_iTextureFlag)
	{

		m_vAlphaMaskUVOffset.x += m_tCommon_UV_Desc.vAlphaMaskUVSpeed.x * fTimeDelta;
		m_vAlphaMaskUVOffset.y += m_tCommon_UV_Desc.vAlphaMaskUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iAlphaMaskSampler) // Clamp
		{
			m_vAlphaMaskUVOffset.x = clamp(m_vAlphaMaskUVOffset.x, m_tCommon_UV_Desc.vMinAlphaMaskUVOffset.x, m_tCommon_UV_Desc.vMaxAlphaMaskUVOffset.x);
			m_vAlphaMaskUVOffset.y = clamp(m_vAlphaMaskUVOffset.y, m_tCommon_UV_Desc.vMinAlphaMaskUVOffset.y, m_tCommon_UV_Desc.vMaxAlphaMaskUVOffset.y);
		}
		else // Wrap
		{
			if (fabsf(m_vAlphaMaskUVOffset.x) >= 9999.f)
				m_vAlphaMaskUVOffset.x = 0.f;
			if (fabsf(m_vAlphaMaskUVOffset.y) >= 9999.f)
				m_vAlphaMaskUVOffset.y = 0.f;
		}
	}

	if (COLORSCALE_UV & m_iTextureFlag)
	{
		m_vColorScaleUVOffset.x += m_tCommon_UV_Desc.vColorScaleUVSpeed.x * fTimeDelta;
		m_vColorScaleUVOffset.y += m_tCommon_UV_Desc.vColorScaleUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iColorScaleUVSampler) // Clamp
		{
			m_vColorScaleUVOffset.x = clamp(m_vColorScaleUVOffset.x, m_tCommon_UV_Desc.vMinColorScaleUVOffset.x, m_tCommon_UV_Desc.vMaxColorScaleUVOffset.x);
			m_vColorScaleUVOffset.y = clamp(m_vColorScaleUVOffset.y, m_tCommon_UV_Desc.vMinColorScaleUVOffset.y, m_tCommon_UV_Desc.vMaxColorScaleUVOffset.y);
		}
		else // Wrap
		{
			if (fabsf(m_vColorScaleUVOffset.x) >= 9999.f)
				m_vColorScaleUVOffset.x = 0.f;
			if (fabsf(m_vColorScaleUVOffset.y) >= 9999.f)
				m_vColorScaleUVOffset.y = 0.f;
		}
	}

	if (NOISE & m_iTextureFlag)
	{
		m_vNoiseUVOffset.x += m_tCommon_UV_Desc.vNoiseUVSpeed.x * fTimeDelta;
		m_vNoiseUVOffset.y += m_tCommon_UV_Desc.vNoiseUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iNoiseSampler) // Clamp
		{
			m_vNoiseUVOffset.x = clamp(m_vNoiseUVOffset.x, m_tCommon_UV_Desc.vMinNoiseUVOffset.x, m_tCommon_UV_Desc.vMaxNoiseUVOffset.x);
			m_vNoiseUVOffset.y = clamp(m_vNoiseUVOffset.y, m_tCommon_UV_Desc.vMinNoiseUVOffset.y, m_tCommon_UV_Desc.vMaxNoiseUVOffset.y);
		}
		else
		{
			if (fabsf(m_vNoiseUVOffset.x) >= 9999.f)
				m_vNoiseUVOffset.x = 0.f;
			if (fabsf(m_vNoiseUVOffset.y) >= 9999.f)
				m_vNoiseUVOffset.y = 0.f;
		}
	}

	if (DISTORTIONTEX & m_iTextureFlag)
	{
		m_vDistortionUVOffset.x += m_tCommon_UV_Desc.vDistortionUVSpeed.x * fTimeDelta;
		m_vDistortionUVOffset.y += m_tCommon_UV_Desc.vDistortionUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iDistortionSampler) // Clamp
		{
			m_vDistortionUVOffset.x = clamp(m_vDistortionUVOffset.x, m_tCommon_UV_Desc.vMinDistortionUVOffset.x, m_tCommon_UV_Desc.vMaxDistortionUVOffset.x);
			m_vDistortionUVOffset.y = clamp(m_vDistortionUVOffset.y, m_tCommon_UV_Desc.vMinDistortionUVOffset.y, m_tCommon_UV_Desc.vMaxDistortionUVOffset.y);
		}
		else
		{
			if (fabsf(m_vDistortionUVOffset.x) >= 9999.f)
				m_vDistortionUVOffset.x = 0.f;
			if (fabsf(m_vDistortionUVOffset.y) >= 9999.f)
				m_vDistortionUVOffset.y = 0.f;
		}
	}

	if (DISSOLVE & m_iTextureFlag)
	{
		m_vDissolveUVOffset.x += m_tCommon_UV_Desc.vDissolveUVSpeed.x * fTimeDelta;
		m_vDissolveUVOffset.y += m_tCommon_UV_Desc.vDissolveUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iDissolveSampler) // Clamp
		{
			m_vDissolveUVOffset.x = clamp(m_vDissolveUVOffset.x, m_tCommon_UV_Desc.vMinDissolveUVOffset.x, m_tCommon_UV_Desc.vMaxDissolveUVOffset.x);
			m_vDissolveUVOffset.y = clamp(m_vDissolveUVOffset.y, m_tCommon_UV_Desc.vMinDissolveUVOffset.y, m_tCommon_UV_Desc.vMaxDissolveUVOffset.y);
		}
		else
		{
			if (fabsf(m_vDissolveUVOffset.x) >= 9999.f)
				m_vDissolveUVOffset.x = 0.f;
			if (fabsf(m_vDissolveUVOffset.y) >= 9999.f)
				m_vDissolveUVOffset.y = 0.f;
		}
	}

	if (BASE_NOISE_UV & m_iTextureFlag)
	{
		m_vBaseNoiseUVOffset.x += m_tCommon_UV_Desc.vBaseNoiseUVSpeed.x * fTimeDelta;
		m_vBaseNoiseUVOffset.y += m_tCommon_UV_Desc.vBaseNoiseUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iBaseNoiseSampler) // Clamp
		{
			m_vBaseNoiseUVOffset.x = clamp(m_vBaseNoiseUVOffset.x, m_tCommon_UV_Desc.vMinBaseNoiseUVOffset.x, m_tCommon_UV_Desc.vMaxBaseNoiseUVOffset.x);
			m_vBaseNoiseUVOffset.y = clamp(m_vBaseNoiseUVOffset.y, m_tCommon_UV_Desc.vMinBaseNoiseUVOffset.y, m_tCommon_UV_Desc.vMaxBaseNoiseUVOffset.y);
		}
		else
		{
			if (fabsf(m_vBaseNoiseUVOffset.x) >= 9999.f)
				m_vBaseNoiseUVOffset.x = 0.f;
			if (fabsf(m_vBaseNoiseUVOffset.y) >= 9999.f)
				m_vBaseNoiseUVOffset.y = 0.f;
		}
	}

	if (ALPHA_NOISE_UV & m_iTextureFlag)
	{
		m_vAlphaMaskNoiseUVOffset.x += m_tCommon_UV_Desc.vAlphaMaskNoiseUVSpeed.x * fTimeDelta;
		m_vAlphaMaskNoiseUVOffset.y += m_tCommon_UV_Desc.vAlphaMaskNoiseUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iAlphaMaskNoiseSampler) // Clamp
		{
			m_vAlphaMaskNoiseUVOffset.x = clamp(m_vAlphaMaskNoiseUVOffset.x, m_tCommon_UV_Desc.vMinAlphaMaskNoiseUVOffset.x, m_tCommon_UV_Desc.vMaxAlphaMaskNoiseUVOffset.x);
			m_vAlphaMaskNoiseUVOffset.y = clamp(m_vAlphaMaskNoiseUVOffset.y, m_tCommon_UV_Desc.vMinAlphaMaskNoiseUVOffset.y, m_tCommon_UV_Desc.vMaxAlphaMaskNoiseUVOffset.y);
		}
		else
		{
			if (fabsf(m_vAlphaMaskNoiseUVOffset.x) >= 9999.f)
				m_vAlphaMaskNoiseUVOffset.x = 0.f;
			if (fabsf(m_vAlphaMaskNoiseUVOffset.y) >= 9999.f)
				m_vAlphaMaskNoiseUVOffset.y = 0.f;
		}
	}

	if (UV_SLICE_CLIP & m_iTextureFlag)
	{
		m_vUVSliceClipUVOffset.x += m_tCommon_UV_Desc.vUVSliceClipUVSpeed.x * fTimeDelta;
		m_vUVSliceClipUVOffset.y += m_tCommon_UV_Desc.vUVSliceClipUVSpeed.y * fTimeDelta;

		if (1 == m_tCommon_UV_Desc.iUVSliceClipSampler) // Clamp
		{
			m_vUVSliceClipUVOffset.x = clamp(m_vUVSliceClipUVOffset.x, m_tCommon_UV_Desc.vMinUVSliceClipUVOffset.x, m_tCommon_UV_Desc.vMaxUVSliceClipUVOffset.x);
			m_vUVSliceClipUVOffset.y = clamp(m_vUVSliceClipUVOffset.y, m_tCommon_UV_Desc.vMinUVSliceClipUVOffset.y, m_tCommon_UV_Desc.vMaxUVSliceClipUVOffset.y);
		}
		else 
		{
			if (fabsf(m_vUVSliceClipUVOffset.x) >= 9999.f)
				m_vUVSliceClipUVOffset.x = 0.f;
			if (fabsf(m_vUVSliceClipUVOffset.y) >= 9999.f)
				m_vUVSliceClipUVOffset.y = 0.f;
		}
	}
}

void CEffect::Update_Dissolve(_float fTimeDelta)
{
	if (0 == (m_iTextureFlag & DISSOLVE))
		return;

	if (m_tCommon_Desc.fDissolveStartTime > m_fTimeAcc)
		return;

	if ((m_tCommon_Desc.iDissolveFlag & DISSOLVE_STOP) && (m_fDissolveAmount >= m_tCommon_Desc.fStopDissolveAmount))
	{
		m_fDissolveAmount = m_tCommon_Desc.fStopDissolveAmount;
		return;
	}



	if (m_tCommon_Desc.iDissolveFlag & DISSOLVE_LOOP)
	{
		if (m_fDissolveAmount <= m_tCommon_Desc.fDissolveLoopMin)
		{
			m_fDissolveAmount = m_tCommon_Desc.fDissolveLoopMin;
		}
		
		if (true == m_bDissolveLoopUp)
		{
			m_fDissolveAmount += m_tCommon_Desc.fDissolveSpeed * fTimeDelta;

			if (m_fDissolveAmount >= m_tCommon_Desc.fDissolveLoopMax)
			{
				m_fDissolveAmount = m_tCommon_Desc.fDissolveLoopMax;
				m_bDissolveLoopUp = false;
			}
		}
		else
		{
			m_fDissolveAmount -= m_tCommon_Desc.fDissolveSpeed * fTimeDelta;

			if (m_fDissolveAmount <= m_tCommon_Desc.fDissolveLoopMin)
			{
				m_fDissolveAmount = m_tCommon_Desc.fDissolveLoopMin;
				m_bDissolveLoopUp = true;
			}
		}
	}
	else
	{

		m_fDissolveAmount += m_tCommon_Desc.fDissolveSpeed * fTimeDelta;
	}

}

void CEffect::Update_Sprite(_float fTimeDelta)
{
	if (true == m_tCommon_Desc.bBaseSprite && true == m_tCommon_Desc.ChangeBaseSprite)
	{
		m_fBaseSpriteTimeAcc += fTimeDelta;
		if (m_fBaseSpriteTimeAcc >= m_tCommon_Desc.fBaseSpriteChangeTime)
		{
			m_fBaseSpriteTimeAcc = 0.f;
			++m_iBaseSpriteIndex;
			if (m_tCommon_Desc.iBaseSpriteMaxCol * m_tCommon_Desc.iBaseSpriteMaxRow
				<= m_iBaseSpriteIndex)
			{
				m_iBaseSpriteIndex = 0;
			}
		}
	}

	if (true == m_tCommon_Desc.bMaskSprite && true == m_tCommon_Desc.ChangeMaskSprite)
	{
		m_fMaskSpriteTimeAcc += fTimeDelta;
		if (m_fMaskSpriteTimeAcc >= m_tCommon_Desc.fMaskSpriteChangeTime)
		{
			m_fMaskSpriteTimeAcc = 0.f;
			++m_iMaskSpriteIndex;
			if (m_tCommon_Desc.iMaskSpriteMaxCol * m_tCommon_Desc.iMaskSpriteMaxRow
				<= m_iMaskSpriteIndex)
			{
				m_iMaskSpriteIndex = 0;
			}
		}
	}
}


CGameObject* CEffect::Clone(void* pArg)
{
	return nullptr;
}

void CEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShader);
	Safe_Release(m_tAttachDesc.pBone);
	Safe_Release(m_tAttachDesc.pParentTransform);
	Safe_Release(m_tAttachDesc.pTargetBone);

	for (auto& pTex : m_pTexture)
		Safe_Release(pTex);
}

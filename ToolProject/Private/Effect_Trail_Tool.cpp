#include "Effect_Trail_Tool.h"
#include "VIBuffer_SwordTrail.h"
CEffect_Trail_Tool::CEffect_Trail_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect{ pDevice, pContext }
{
}

CEffect_Trail_Tool::CEffect_Trail_Tool(const CEffect_Trail_Tool& rhs)
	: CEffect(rhs)
	, m_tTrail_Desc{ rhs.m_tTrail_Desc }
{
}

HRESULT CEffect_Trail_Tool::Initialize_Prototype()
{
	m_eEffect_Type = TRAIL;
	return S_OK;
}

HRESULT CEffect_Trail_Tool::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pVIBuffer->Remakee_Buffers(m_tTrail_Desc);

	return S_OK;
}

void CEffect_Trail_Tool::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Trail_Tool::Tick(_float fTimeDelta)
{

}

void CEffect_Trail_Tool::LateTick(_float fTimeDelta)
{
	if (true == m_bEndCreate)
	{
		m_pVIBuffer->Set_DeleteStart(true);
		if (true == m_pVIBuffer->IsEmpty())
			m_isEnd = true;
	}

	if (true == m_isEnd && true == m_bEndCreate)
		return;



	Update_Color();
	Update_TexCoord(fTimeDelta);
	Update_Dissolve(fTimeDelta);

	__super::Attach_Update();

	m_pVIBuffer->Update(XMLoadFloat4x4(&m_AttachWorldMat));

	if (true == m_tCommon_Desc.bProportionalAlphaToSpeed && true == m_tCommon_Desc.bEnableColorOverLife)
	{
		_float3 vCur = {};
		memcpy(&vCur, m_AttachWorldMat.m[3], sizeof(_float3));

		_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPrevPos) - XMLoadFloat3(&vCur)));
		_float fSpeed = fDist / fTimeDelta;

		fSpeed = clamp(fSpeed, 0.f, m_tCommon_Desc.fProportionalAlphaMaxSpeed);

		if (true == m_tCommon_Desc.bEnableColorOverLife)
			m_vCurrentLerpColor.w = fSpeed / m_tCommon_Desc.fProportionalAlphaMaxSpeed;
		m_vPrevPos = vCur;
	}

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= m_tCommon_Desc.fTotalPlayTime)
	{
		m_fTimeAcc = 0.f;
		if (false == m_tCommon_Desc.bEffectLoop)
			m_bEndCreate = true;
			// m_isEnd = true;
		else
		{
			Reset_Effect();
		}
	}

	_uint iRenderGroup = CRenderer::RENDER_EFFECT_NONBLEND + m_tCommon_Desc.iRenderGroup;

	m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)iRenderGroup, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);

}

HRESULT CEffect_Trail_Tool::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_tCommon_Desc.iPassIdx)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CEffect_Trail_Tool::Save_Effect(const _char* pTag, nlohmann::ordered_json& JsonData)
{
	if (FAILED(__super::Save_Effect(pTag, JsonData)))
		return E_FAIL;

	JsonData[pTag]["Trail Description"]["vStartPos"] = { m_tTrail_Desc.vStartPos.x, m_tTrail_Desc.vStartPos.y, m_tTrail_Desc.vStartPos.x };
	JsonData[pTag]["Trail Description"]["vEndPos"] = { m_tTrail_Desc.vEndPos.x, m_tTrail_Desc.vEndPos.y, m_tTrail_Desc.vEndPos.z };
	JsonData[pTag]["Trail Description"]["iMaxVertexCount"] = m_tTrail_Desc.iMaxVertexCount;
	JsonData[pTag]["Trail Description"]["iCatmullRomCount"] = m_tTrail_Desc.iCatmullRomCount;
	JsonData[pTag]["Trail Description"]["iRefinement"] = m_tTrail_Desc.iRefinement;

	return S_OK;
}

HRESULT CEffect_Trail_Tool::Load_Effect(const _char* pTag, json& JsonData)
{
	__super::Load_Effect(pTag, JsonData);

	memcpy(&m_tTrail_Desc.vStartPos, JsonData[pTag]["Trail Description"]["vStartPos"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tTrail_Desc.vEndPos, JsonData[pTag]["Trail Description"]["vEndPos"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tTrail_Desc.iMaxVertexCount = JsonData[pTag]["Trail Description"]["iMaxVertexCount"];
	m_tTrail_Desc.iCatmullRomCount = JsonData[pTag]["Trail Description"]["iCatmullRomCount"];
	m_tTrail_Desc.iRefinement = JsonData[pTag]["Trail Description"]["iRefinement"];

	return S_OK;
}
HRESULT CEffect_Trail_Tool::Load_Effect_CINEMA(const _char* pTag, json& JsonData)
{
	if (FAILED(__super::Load_Effect_CINEMA(pTag, JsonData)))
		return E_FAIL;

	memcpy(&m_tTrail_Desc.vStartPos, JsonData[m_szEffectTag]["Trail Description"]["vStartPos"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tTrail_Desc.vEndPos, JsonData[m_szEffectTag]["Trail Description"]["vEndPos"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tTrail_Desc.iMaxVertexCount = JsonData[m_szEffectTag]["Trail Description"]["iMaxVertexCount"];
	m_tTrail_Desc.iCatmullRomCount = JsonData[m_szEffectTag]["Trail Description"]["iCatmullRomCount"];
	m_tTrail_Desc.iRefinement = JsonData[m_szEffectTag]["Trail Description"]["iRefinement"];
	return S_OK;
}
void CEffect_Trail_Tool::Reset_Effect(CINE_EFFECT_DESC* pDesc)
{
	__super::Reset_Effect(pDesc);
	Reset_Trail();

	m_pVIBuffer->Remakee_Buffers(m_tTrail_Desc);
}


void CEffect_Trail_Tool::Reset_Trail()
{
	__super::Reset_Setting();
	m_vBaseUVOffset = m_tCommon_UV_Desc.vStartBaseUVOffset;
	m_vAlphaMaskUVOffset = m_tCommon_UV_Desc.vStartAlphaMaskUVOffset;
	m_vNoiseUVOffset = m_tCommon_UV_Desc.vStartNoiseUVOffset;

	m_vStartColor = JoRandom::Random_Float4(m_tCommon_Desc.vStartColorMin, m_tCommon_Desc.vStartColorMax);
	_float3 vRot = JoRandom::Random_Float3(m_tCommon_Desc.vStartRotationMin, m_tCommon_Desc.vStartRotationMax);
	m_pTransform->Rotation_RollPitchYaw(To_Radian(vRot.z), To_Radian(vRot.x), To_Radian(vRot.y));
	//m_pVIBuffer->Reset_Points();
	m_bEndCreate = false;
}

void CEffect_Trail_Tool::Remake_Trail(const TRAIL_DESC& tDesc, const HR::EFFECT_COMMON_DESC& tCommonDesc)
{
	__super::Update_Effet_Desc(tCommonDesc);

	memcpy(&m_tTrail_Desc, &tDesc, sizeof(TRAIL_DESC));

	Reset_Trail();

	m_pVIBuffer->Remakee_Buffers(tDesc);
}

void CEffect_Trail_Tool::Update_Color()
{
	if (true == m_tCommon_Desc.bEnableColorOverLife)
	{
		if (0.f == m_tCommon_Desc.fTotalPlayTime)
			return;

		_float fRatio = m_fTimeAcc / m_tCommon_Desc.fTotalPlayTime;
		XMStoreFloat4(&m_vCurrentLerpColor, XMVectorLerp(XMLoadFloat4(&m_vStartColor),
			XMLoadFloat4(&m_tCommon_Desc.vColorOverLife), fRatio));
	}
}


HRESULT CEffect_Trail_Tool::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_EffectTrail"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Trail"), TEXT("Trail_Buffer"), (CComponent**)&m_pVIBuffer, &m_tTrail_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail_Tool::Bind_ShaderResources()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (true == m_tCommon_Desc.bUseDistortion)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_fDistortionIntensity", &m_tCommon_Desc.fDistortionPower, sizeof(_float))))
			return E_FAIL;
	}

	if (true == m_tCommon_Desc.bEnableColorOverLife)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vLerpColor", &m_vCurrentLerpColor, sizeof(_float4))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vStartColor", &m_vStartColor, sizeof(_float4))))
			return E_FAIL;
	}

	if (m_tCommon_Desc.iDissolveFlag & DISSOLVE_EDGE)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vDissolveDiffuse", &m_tCommon_Desc.vEdgeColor, sizeof(_float4))))
			return E_FAIL;
		_bool b = true;
		if (FAILED(m_pShader->Bind_RawValue("g_bDissolveEdge", &b, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDissolveEdgeWidth", &m_tCommon_Desc.fDissolveEdgeWidth, sizeof(_float))))
			return E_FAIL;
	}
	else if (0 == (m_tCommon_Desc.iDissolveFlag & DISSOLVE_EDGE) || nullptr == m_pTexture[TEX_DISSOLVE])
	{
		_bool b = false;
		if (FAILED(m_pShader->Bind_RawValue("g_bDissolveEdge", &b, sizeof(_bool))))
			return E_FAIL;
	}


	if (FAILED(m_pShader->Bind_RawValue("g_bDistortion", &m_tCommon_Desc.bUseDistortion, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_bColorLerp", &m_tCommon_Desc.bEnableColorOverLife, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vColor_Clip", &m_tCommon_Desc.vClipColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vAddColor", &m_tCommon_Desc.vAddColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vMulColor", &m_tCommon_Desc.vMultipleColor, sizeof(_float4))))
		return E_FAIL;



	// ********************** TEXTURE ************************

	if (FAILED(m_pShader->Bind_RawValue("g_TextureFlag", &m_iTextureFlag, sizeof(_int))))
		return E_FAIL;


	if (nullptr != m_pTexture[TEX_BASE])
	{
		if (FAILED(m_pTexture[TEX_BASE]->Set_SRV(m_pShader, "g_BaseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vBaseUVOffset", &m_vBaseUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iBaseSampler", &m_tCommon_UV_Desc.iBaseSampler, sizeof(_int))))
			return E_FAIL;
	}
	if (nullptr != m_pTexture[TEX_ALPHAMASK])
	{
		if (FAILED(m_pTexture[TEX_ALPHAMASK]->Set_SRV(m_pShader, "g_AlphaMask", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vAlphaMaskUVOffset", &m_vAlphaMaskUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iAlphaMaskSampler", &m_tCommon_UV_Desc.iAlphaMaskSampler, sizeof(_int))))
			return E_FAIL;
	}
	if (nullptr != m_pTexture[TEX_NOISE])
	{
		if (FAILED(m_pTexture[TEX_NOISE]->Set_SRV(m_pShader, "g_NoiseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vNoiseUVOffset", &m_vNoiseUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iNoiseSampler", &m_tCommon_UV_Desc.iNoiseSampler, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bNoiseMultiplyAlpha", &m_tCommon_Desc.bNoiseMultiplyAlpha, sizeof(_bool))))
			return E_FAIL;
	}
	if (nullptr != m_pTexture[TEX_DISSOLVE])
	{
		if (FAILED(m_pTexture[TEX_DISSOLVE]->Set_SRV(m_pShader, "g_DissolveTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vDissolveUVOffset", &m_vDissolveUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iDissolveSampler", &m_tCommon_UV_Desc.iDissolveSampler, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
			return E_FAIL;
	}


	return S_OK;
}

CEffect_Trail_Tool* CEffect_Trail_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Trail_Tool* pInstance = new CEffect_Trail_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Trail_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Trail_Tool::Clone(void* pArg)
{
	CEffect_Trail_Tool* pInstance = new CEffect_Trail_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Trail_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Trail_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pVIBuffer);
}

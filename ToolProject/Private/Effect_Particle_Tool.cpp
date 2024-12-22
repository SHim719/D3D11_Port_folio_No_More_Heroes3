#include "Effect_Particle_Tool.h"
#include "Transform.h"
#include "Camera.h"

CEffect_Paticle_Tool::CEffect_Paticle_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CEffect_Paticle_Tool::CEffect_Paticle_Tool(const CEffect_Paticle_Tool& rhs)
	:CEffect(rhs)
	, m_eParticle_Mode{ rhs.m_eParticle_Mode }
	, m_tParticle_Init_Desc{ rhs.m_tParticle_Init_Desc }
{
}

HRESULT CEffect_Paticle_Tool::Initialize_Prototype()
{
	m_eEffect_Type = PARTICLE;
	return S_OK;
}

HRESULT CEffect_Paticle_Tool::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Ready_ParticleDatas();

	First_Setting_Particle();
	m_isEnd = true;

	return S_OK;
}

void CEffect_Paticle_Tool::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Paticle_Tool::Tick(_float fTimeDelta)
{
	if (true == m_isEnd)
		return;

	m_bCPUCODE = false;

	if (m_tParticle_Init_Desc.bFollowTargetPos)
		Update_TargetPos();

	__super::Attach_Update();

	// ******* Compute Shader *********

	if (false == m_bCPUCODE)
	{
		Update_TickBuffer(fTimeDelta);

		Bind_Compute_Shader_Datas();

		m_pCS->Dispatch(m_iNumGroupX, m_iNumGroupY, 1);


		if (true == m_bChangeDir)
			m_bChangeDir = false;

		m_pContext->CopyResource(m_pCurrentVerticesStaging, m_pCurrentVerticesBuffer);

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		if (FAILED(m_pContext->Map(m_pCurrentVerticesStaging, 0, D3D11_MAP_READ, 0, &MappedResource)))
		{
			MSG_BOX(TEXT("FAILED To Map Staging Buffer"));
			return;
		}
		VTXPARTICLE* tTmp = reinterpret_cast<VTXPARTICLE*>(MappedResource.pData);
	
		memcpy(m_ParticleVertices_Current.data(), tTmp, sizeof(VTXPARTICLE) * m_tParticle_Init_Desc.iNumInstance);


		m_pContext->Unmap(m_pCurrentVerticesStaging, 0);
	}
	else
	{
		for (size_t i = 0; i < m_ParticleVertices_Current.size(); ++i)
		{
			if (false == ComputeStartDelay(i, fTimeDelta) ||
				0 >= m_ParticleVertices_Current[i].fLifeTime)
				continue;

			Update_Sprite(i, fTimeDelta);
			Update_Dissolve(i, fTimeDelta);
			Update_Position(i, fTimeDelta);
			Update_Rotation(i, fTimeDelta);
			Update_Color(i, fTimeDelta);
			Update_Size(i);
			Update_Speed(i, fTimeDelta);
			ComputeLifeTime(i, fTimeDelta);
		}
	}
	Update_TexCoord(fTimeDelta);


	if (nullptr != m_pVIBuffer)
		m_pVIBuffer->Update_Particle(m_ParticleVertices_Current);


	m_fTimeAcc += fTimeDelta;
	if (true == m_tParticle_Init_Desc.bStopParticle && false == m_EndStop)
	{

		if (m_fTimeAcc >= m_tParticle_Init_Desc.fStopTime)
		{
			if (m_tParticle_Init_Desc.fMaintainTime <= m_fTimeAcc - m_tParticle_Init_Desc.fStopTime)
			{
				m_bStoped = false;
				m_EndStop = true;

				if (/*STOP_DISTANCE == m_tParticle_Init_Desc.iStopType && */true == m_tParticle_Init_Desc.bChangeDirAfterStop)
				{
					for (size_t i = 0; i < m_ParticleVertices_Current.size(); ++i)
					{
						if (true == m_bCPUCODE)
						{
							XMStoreFloat3(&m_ParticleVertices_Current[i].vVelocity,
								XMLoadFloat3(&m_AfterStopDir[i]) * m_PaticleDesc_Current[i].fSpeed);

							m_PaticleDesc_Current[i].vSpeedDir = m_AfterStopDir[i];
						}
						else
						{
							m_bChangeDir = true;
						}
					}
				}
			}
			else
			{
				if (m_fTimeAcc >= m_tParticle_Init_Desc.fStopTime)
					m_bStoped = true;
			}
		}

	}

	if (m_fTimeAcc >= m_tCommon_Desc.fTotalPlayTime)
	{
		m_fTimeAcc = 0.f;
		if (false == m_tCommon_Desc.bEffectLoop)
			m_isEnd = true;
		else
		{
			Reset_Setting();
		}

	}
}

void CEffect_Paticle_Tool::LateTick(_float fTimeDelta)
{
	if (true == m_isEnd)
		return;

	_uint iRenderGroup = CRenderer::RENDER_EFFECT_NONBLEND + m_tCommon_Desc.iRenderGroup;

	m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)iRenderGroup, this);


}

HRESULT CEffect_Paticle_Tool::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin((_uint)m_tCommon_Desc.iPassIdx)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Paticle_Tool::Save_Effect(const _char* pTag, nlohmann::ordered_json& JsonData)
{
	if (FAILED(__super::Save_Effect(pTag, JsonData)))
		return E_FAIL;

	JsonData[pTag]["Particle Description"]["iNumInstance"] = m_tParticle_Init_Desc.iNumInstance;
	JsonData[pTag]["Particle Description"]["eParticleMode"] = m_tParticle_Init_Desc.eParticleMode;
	JsonData[pTag]["Particle Description"]["bParticleLoop"] = m_tParticle_Init_Desc.bParticleLoop;
	JsonData[pTag]["Particle Description"]["bisSquare"] = m_tParticle_Init_Desc.bisSquare;
	JsonData[pTag]["Particle Description"]["vStartSizeMin"] = { m_tParticle_Init_Desc.vStartSizeMin.x,m_tParticle_Init_Desc.vStartSizeMin.y };
	JsonData[pTag]["Particle Description"]["vStartSizeMax"] = { m_tParticle_Init_Desc.vStartSizeMax.x,m_tParticle_Init_Desc.vStartSizeMax.y };
	JsonData[pTag]["Particle Description"]["vSizeOverLife"] = { m_tParticle_Init_Desc.vSizeOverLife.x,m_tParticle_Init_Desc.vSizeOverLife.y };
	JsonData[pTag]["Particle Description"]["vRotAxis"] = { m_tParticle_Init_Desc.vRotAxis.x,m_tParticle_Init_Desc.vRotAxis.y,m_tParticle_Init_Desc.vRotAxis.z };
	JsonData[pTag]["Particle Description"]["fRotSpeed"] = m_tParticle_Init_Desc.fRotSpeed;
	JsonData[pTag]["Particle Description"]["bBillboardWithoutUp"] = m_tParticle_Init_Desc.bBillboardWithoutUp;
	JsonData[pTag]["Particle Description"]["fStretchFactor"] = m_tParticle_Init_Desc.fStretchFactor;
	JsonData[pTag]["Particle Description"]["vLightDir"] = { m_tParticle_Init_Desc.vLightDir.x,m_tParticle_Init_Desc.vLightDir.y,m_tParticle_Init_Desc.vLightDir.z };
	JsonData[pTag]["Particle Description"]["vLightAmbient"] = { m_tParticle_Init_Desc.vLightAmbient.x,m_tParticle_Init_Desc.vLightAmbient.y,m_tParticle_Init_Desc.vLightAmbient.z,m_tParticle_Init_Desc.vLightAmbient.w };
	JsonData[pTag]["Particle Description"]["bHasTargetPoint"] = m_tParticle_Init_Desc.bHasTargetPoint;
	JsonData[pTag]["Particle Description"]["bTargetWorldOffSet"] = m_tParticle_Init_Desc.bTargetWorldOffSet;
	JsonData[pTag]["Particle Description"]["vTargetPointOffset"] = { m_tParticle_Init_Desc.vTargetPointOffset.x,m_tParticle_Init_Desc.vTargetPointOffset.y,m_tParticle_Init_Desc.vTargetPointOffset.z };
	JsonData[pTag]["Particle Description"]["fMargin"] = m_tParticle_Init_Desc.fMargin;
	JsonData[pTag]["Particle Description"]["bTargetLifeZero"] = m_tParticle_Init_Desc.bTargetLifeZero;
	JsonData[pTag]["Particle Description"]["bFollowTargetPos"] = m_tParticle_Init_Desc.bFollowTargetPos;
	JsonData[pTag]["Particle Description"]["bXStretch"] = m_tParticle_Init_Desc.bXStretch;
	JsonData[pTag]["Particle Description"]["bYStretch"] = m_tParticle_Init_Desc.bYStretch;

	JsonData[pTag]["Particle Description"]["bStopParticle"] = m_tParticle_Init_Desc.bStopParticle;
	JsonData[pTag]["Particle Description"]["iStopType"] = m_tParticle_Init_Desc.iStopType;
	JsonData[pTag]["Particle Description"]["fStopTime"] = m_tParticle_Init_Desc.fStopTime;
	JsonData[pTag]["Particle Description"]["fStopDistance"] = m_tParticle_Init_Desc.fStopDistance;
	JsonData[pTag]["Particle Description"]["fMaintainTime"] = m_tParticle_Init_Desc.fMaintainTime;
	JsonData[pTag]["Particle Description"]["bChangeDirAfterStop"] = m_tParticle_Init_Desc.bChangeDirAfterStop;
	JsonData[pTag]["Particle Description"]["vAfterDirMin"] = { m_tParticle_Init_Desc.vAfterDirMin.x,m_tParticle_Init_Desc.vAfterDirMin.y,m_tParticle_Init_Desc.vAfterDirMin.z };
	JsonData[pTag]["Particle Description"]["vAfterDirMax"] = { m_tParticle_Init_Desc.vAfterDirMax.x,m_tParticle_Init_Desc.vAfterDirMax.y,m_tParticle_Init_Desc.vAfterDirMax.z };
	JsonData[pTag]["Particle Description"]["fSpriteChangeTolerance"] = m_tParticle_Init_Desc.fSpriteChangeTolerance;


	return S_OK;
}

HRESULT CEffect_Paticle_Tool::Load_Effect(const _char* pTag, json& JsonData)
{
	__super::Load_Effect(pTag, JsonData);

	m_tParticle_Init_Desc.iNumInstance = JsonData[m_szEffectTag]["Particle Description"]["iNumInstance"];
	m_tParticle_Init_Desc.eParticleMode = JsonData[m_szEffectTag]["Particle Description"]["eParticleMode"];
	m_tParticle_Init_Desc.bParticleLoop = JsonData[m_szEffectTag]["Particle Description"]["bParticleLoop"];
	m_tParticle_Init_Desc.bisSquare = JsonData[m_szEffectTag]["Particle Description"]["bisSquare"];
	memcpy(&m_tParticle_Init_Desc.vStartSizeMin, JsonData[m_szEffectTag]["Particle Description"]["vStartSizeMin"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tParticle_Init_Desc.vStartSizeMax, JsonData[m_szEffectTag]["Particle Description"]["vStartSizeMax"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tParticle_Init_Desc.vSizeOverLife, JsonData[m_szEffectTag]["Particle Description"]["vSizeOverLife"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tParticle_Init_Desc.vRotAxis, JsonData[m_szEffectTag]["Particle Description"]["vRotAxis"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tParticle_Init_Desc.fRotSpeed = JsonData[m_szEffectTag]["Particle Description"]["fRotSpeed"];
	m_tParticle_Init_Desc.bBillboardWithoutUp = JsonData[m_szEffectTag]["Particle Description"]["bBillboardWithoutUp"];
	m_tParticle_Init_Desc.fStretchFactor = JsonData[m_szEffectTag]["Particle Description"]["fStretchFactor"];
	memcpy(&m_tParticle_Init_Desc.vLightDir, JsonData[m_szEffectTag]["Particle Description"]["vLightDir"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tParticle_Init_Desc.vLightAmbient, JsonData[m_szEffectTag]["Particle Description"]["vLightAmbient"].get<array<_float, 4>>().data(), sizeof(_float4));
	m_tParticle_Init_Desc.bHasTargetPoint = JsonData[m_szEffectTag]["Particle Description"]["bHasTargetPoint"];
	m_tParticle_Init_Desc.bTargetWorldOffSet = JsonData[m_szEffectTag]["Particle Description"]["bTargetWorldOffSet"];
	memcpy(&m_tParticle_Init_Desc.vTargetPointOffset, JsonData[m_szEffectTag]["Particle Description"]["vTargetPointOffset"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tParticle_Init_Desc.fMargin = JsonData[m_szEffectTag]["Particle Description"]["fMargin"];
	m_tParticle_Init_Desc.bFollowTargetPos = JsonData[m_szEffectTag]["Particle Description"]["bFollowTargetPos"];
	m_tParticle_Init_Desc.bTargetLifeZero = JsonData[m_szEffectTag]["Particle Description"]["bTargetLifeZero"];
	m_tParticle_Init_Desc.bXStretch = JsonData[m_szEffectTag]["Particle Description"]["bXStretch"];
	m_tParticle_Init_Desc.bYStretch = JsonData[m_szEffectTag]["Particle Description"]["bYStretch"];

	m_tParticle_Init_Desc.bStopParticle = JsonData[m_szEffectTag]["Particle Description"]["bStopParticle"];
	m_tParticle_Init_Desc.iStopType = JsonData[m_szEffectTag]["Particle Description"]["iStopType"];
	m_tParticle_Init_Desc.fStopTime = JsonData[m_szEffectTag]["Particle Description"]["fStopTime"];
	m_tParticle_Init_Desc.fStopDistance = JsonData[m_szEffectTag]["Particle Description"]["fStopDistance"];
	m_tParticle_Init_Desc.fMaintainTime = JsonData[m_szEffectTag]["Particle Description"]["fMaintainTime"];
	m_tParticle_Init_Desc.bChangeDirAfterStop = JsonData[m_szEffectTag]["Particle Description"]["bChangeDirAfterStop"];
	memcpy(&m_tParticle_Init_Desc.vAfterDirMin, JsonData[m_szEffectTag]["Particle Description"]["vAfterDirMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tParticle_Init_Desc.vAfterDirMax, JsonData[m_szEffectTag]["Particle Description"]["vAfterDirMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tParticle_Init_Desc.fSpriteChangeTolerance = JsonData[m_szEffectTag]["Particle Description"]["fSpriteChangeTolerance"];

	return S_OK;
}

HRESULT CEffect_Paticle_Tool::Load_Effect_CINEMA(const _char* pTag, json& JsonData)
{

	if (FAILED(__super::Load_Effect_CINEMA(pTag, JsonData)))
		return E_FAIL;

	m_tParticle_Init_Desc.iNumInstance = JsonData[m_szEffectTag]["Particle Description"]["iNumInstance"];
	m_tParticle_Init_Desc.eParticleMode = JsonData[m_szEffectTag]["Particle Description"]["eParticleMode"];
	m_tParticle_Init_Desc.bParticleLoop = JsonData[m_szEffectTag]["Particle Description"]["bParticleLoop"];
	m_tParticle_Init_Desc.bisSquare = JsonData[m_szEffectTag]["Particle Description"]["bisSquare"];
	memcpy(&m_tParticle_Init_Desc.vStartSizeMin, JsonData[m_szEffectTag]["Particle Description"]["vStartSizeMin"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tParticle_Init_Desc.vStartSizeMax, JsonData[m_szEffectTag]["Particle Description"]["vStartSizeMax"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tParticle_Init_Desc.vSizeOverLife, JsonData[m_szEffectTag]["Particle Description"]["vSizeOverLife"].get<array<_float, 2>>().data(), sizeof(_float2));
	memcpy(&m_tParticle_Init_Desc.vRotAxis, JsonData[m_szEffectTag]["Particle Description"]["vRotAxis"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tParticle_Init_Desc.fRotSpeed = JsonData[m_szEffectTag]["Particle Description"]["fRotSpeed"];
	m_tParticle_Init_Desc.bBillboardWithoutUp = JsonData[m_szEffectTag]["Particle Description"]["bBillboardWithoutUp"];
	m_tParticle_Init_Desc.fStretchFactor = JsonData[m_szEffectTag]["Particle Description"]["fStretchFactor"];
	memcpy(&m_tParticle_Init_Desc.vLightDir, JsonData[m_szEffectTag]["Particle Description"]["vLightDir"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tParticle_Init_Desc.vLightAmbient, JsonData[m_szEffectTag]["Particle Description"]["vLightAmbient"].get<array<_float, 4>>().data(), sizeof(_float4));
	m_tParticle_Init_Desc.bHasTargetPoint = JsonData[m_szEffectTag]["Particle Description"]["bHasTargetPoint"];
	m_tParticle_Init_Desc.bTargetWorldOffSet = JsonData[m_szEffectTag]["Particle Description"]["bTargetWorldOffSet"];
	memcpy(&m_tParticle_Init_Desc.vTargetPointOffset, JsonData[m_szEffectTag]["Particle Description"]["vTargetPointOffset"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tParticle_Init_Desc.fMargin = JsonData[m_szEffectTag]["Particle Description"]["fMargin"];
	m_tParticle_Init_Desc.bFollowTargetPos = JsonData[m_szEffectTag]["Particle Description"]["bFollowTargetPos"];
	m_tParticle_Init_Desc.bTargetLifeZero = JsonData[m_szEffectTag]["Particle Description"]["bTargetLifeZero"];
	m_tParticle_Init_Desc.bXStretch = JsonData[m_szEffectTag]["Particle Description"]["bXStretch"];
	m_tParticle_Init_Desc.bYStretch = JsonData[m_szEffectTag]["Particle Description"]["bYStretch"];

	m_tParticle_Init_Desc.bStopParticle = JsonData[m_szEffectTag]["Particle Description"]["bStopParticle"];
	m_tParticle_Init_Desc.iStopType = JsonData[m_szEffectTag]["Particle Description"]["iStopType"];
	m_tParticle_Init_Desc.fStopTime = JsonData[m_szEffectTag]["Particle Description"]["fStopTime"];
	m_tParticle_Init_Desc.fStopDistance = JsonData[m_szEffectTag]["Particle Description"]["fStopDistance"];
	m_tParticle_Init_Desc.fMaintainTime = JsonData[m_szEffectTag]["Particle Description"]["fMaintainTime"];
	m_tParticle_Init_Desc.bChangeDirAfterStop = JsonData[m_szEffectTag]["Particle Description"]["bChangeDirAfterStop"];
	memcpy(&m_tParticle_Init_Desc.vAfterDirMin, JsonData[m_szEffectTag]["Particle Description"]["vAfterDirMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tParticle_Init_Desc.vAfterDirMax, JsonData[m_szEffectTag]["Particle Description"]["vAfterDirMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tParticle_Init_Desc.fSpriteChangeTolerance = JsonData[m_szEffectTag]["Particle Description"]["fSpriteChangeTolerance"];

	m_eParticle_Mode = (PARTICLE_MODE)m_tParticle_Init_Desc.eParticleMode;

	return S_OK;
}

void CEffect_Paticle_Tool::Reset_Effect(CINE_EFFECT_DESC* pDesc)
{
	Ready_ParticleDatas();

	First_Setting_Particle();

	false == m_bCPUCODE ? Create_Buffers() : NULL;

	__super::Reset_Effect(pDesc);

	Reset_Setting();
}
void CEffect_Paticle_Tool::Reset_Setting()
{
	__super::Reset_Setting();


	if (true == m_bCPUCODE)
	{

		for (size_t i = 0; i < m_PaticleDesc_Current.size(); ++i)
		{
			m_PaticleDesc_Current[i] = m_PaticleDesc_Original[i];
			m_DissolveTimeAcc[i] = 0.f;
		}

		if (0 == m_AfterStopDir.size())
		{
			true == m_tParticle_Init_Desc.bChangeDirAfterStop ? m_AfterStopDir.resize(m_tParticle_Init_Desc.iNumInstance) : NULL;
		}
	}
	else
		m_pContext->CopyResource(m_pCurrent_EachDesc_Buffer, m_pOriginal_EachDesc_Buffer);

	m_isEnd = false;

	m_vAlphaMaskUVOffset = m_tCommon_UV_Desc.vStartAlphaMaskUVOffset;
	m_vColorScaleUVOffset = m_tCommon_UV_Desc.vStartColorScaleUVOffset;
	m_vNoiseUVOffset = m_tCommon_UV_Desc.vStartNoiseUVOffset;
	m_vBaseUVOffset = m_tCommon_UV_Desc.vStartBaseUVOffset;
	m_vBaseNoiseUVOffset = m_tCommon_UV_Desc.vStartBaseNoiseUVOffset;
	m_vAlphaMaskNoiseUVOffset = m_tCommon_UV_Desc.vStartAlphaMaskNoiseUVOffset;
	m_vDissolveUVOffset = m_tCommon_UV_Desc.vStartDissolveUVOffset;

	m_bStoped = false;
	m_EndStop = false;
}
#pragma warning(default:6031)


void CEffect_Paticle_Tool::Update_Desc(const PARTICLE_INIT_DESC& tParticleDesc, const EFFECT_COMMON_DESC& tCommonDesc)
{
	memcpy(&m_tParticle_Init_Desc, &tParticleDesc, sizeof(PARTICLE_INIT_DESC));
	__super::Update_Effet_Desc(tCommonDesc);

	Ready_ParticleDatas();

	m_eParticle_Mode = (PARTICLE_MODE)m_tParticle_Init_Desc.eParticleMode;

	m_fTimeAcc = 0.f;

	Reset_Setting();
	First_Setting_Particle();
	false == m_bCPUCODE ? Create_Buffers() : NULL;
}

HRESULT CEffect_Paticle_Tool::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Particle"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Particle"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Compute"), TEXT("Compute_Shader"), (CComponent**)&m_pCS)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Paticle_Tool::Ready_ParticleDatas()
{
	if (0 == m_tParticle_Init_Desc.iNumInstance)
		return S_OK;

	if (FAILED(m_pVIBuffer->Init_InstanceBuffer(m_tParticle_Init_Desc.iNumInstance)))
		return E_FAIL;

	m_DissolveTimeAcc.clear();
	m_DissolveTimeAcc.resize(m_tParticle_Init_Desc.iNumInstance);

	m_ParticleVertices_Current.clear();
	m_ParticleVertices_Current.resize(m_tParticle_Init_Desc.iNumInstance);

	m_ParticleVertices_Original.clear();
	m_ParticleVertices_Original.resize(m_tParticle_Init_Desc.iNumInstance);

	m_PaticleDesc_Original.clear();
	m_PaticleDesc_Original.resize(m_tParticle_Init_Desc.iNumInstance);

	m_PaticleDesc_Current.clear();
	m_PaticleDesc_Current.resize(m_tParticle_Init_Desc.iNumInstance);

	m_MoveDistance.clear();
	m_AfterStopDir.clear();
	1 == m_tParticle_Init_Desc.iStopType ? m_MoveDistance.resize(m_tParticle_Init_Desc.iNumInstance) : NULL;
	true == m_tParticle_Init_Desc.bChangeDirAfterStop ? m_AfterStopDir.resize(m_tParticle_Init_Desc.iNumInstance) : NULL;

	if (m_tCommon_Desc.ChangeBaseSprite || m_tCommon_Desc.ChangeMaskSprite)
	{
		m_SpriteTimeAcc.clear();
		m_SpriteTimeAcc.resize(m_tParticle_Init_Desc.iNumInstance);

		m_SpriteChangeTimes.clear();
		m_SpriteChangeTimes.resize(m_tParticle_Init_Desc.iNumInstance);

	}
	return S_OK;
}

HRESULT CEffect_Paticle_Tool::Bind_ShaderResources()
{

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_AttachWorldMat)))
		return E_FAIL;


	_float4 vCamPos = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;


	if (true == m_tCommon_Desc.bUseDistortion)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_fDistortionIntensity", &m_tCommon_Desc.fDistortionPower, sizeof(_float))))
			return E_FAIL;
	}
	if (STRETCHED_BILLBOARD == m_eParticle_Mode)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_fStretchFactor", &m_tParticle_Init_Desc.fStretchFactor, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bXStretch", &m_tParticle_Init_Desc.bXStretch, sizeof(_bool))))
			return E_FAIL;
	}
	if (m_tCommon_UV_Desc.iRadialMappingFlag & BASERADIAL)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vBaseRadiusFlag", &m_tCommon_UV_Desc.vBaseNoiseRadiusFlag, sizeof(_float2))))
			return E_FAIL;
	}
	if (m_tCommon_UV_Desc.iRadialMappingFlag & ALPHARADIAL)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vAlphaRadiusFlag", &m_tCommon_UV_Desc.vAlphaNoiseRadiusFlag, sizeof(_float2))))
			return E_FAIL;
	}

	{
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
		_int iMode = (_int)m_eParticle_Mode;
		if (FAILED(m_pShader->Bind_RawValue("g_iParticleMode", &iMode, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bBillboardWithoutUp", &m_tParticle_Init_Desc.bBillboardWithoutUp, sizeof(_bool))))
			return E_FAIL;
		_int iType = m_eTF_Type + 10;
		if (FAILED(m_pShader->Bind_RawValue("g_iTransformType", &iType, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iRadialMappingFlag", &m_tCommon_UV_Desc.iRadialMappingFlag, sizeof(_int))))
			return E_FAIL;
	}



	if (FAILED(m_pShader->Bind_RawValue("g_TextureFlag", &m_iTextureFlag, sizeof(_int))))
		return E_FAIL;



	if (nullptr != m_pTexture[TEX_BASE])
	{
		if (FAILED(m_pTexture[TEX_BASE]->Set_SRV(m_pShader, "g_BaseTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bUseSpriteBase", &m_tCommon_Desc.bBaseSprite, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vBaseUVOffset", &m_vBaseUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iBaseSampler", &m_tCommon_UV_Desc.iBaseSampler, sizeof(_int))))
			return E_FAIL;
		if (true == m_tCommon_Desc.bBaseSprite)
		{
			_float fHeightUV = 1.f / m_tCommon_Desc.iBaseSpriteMaxRow;
			_float fWidthUV = 1.f / m_tCommon_Desc.iBaseSpriteMaxCol;
			if (FAILED(m_pShader->Bind_RawValue("g_iBaseCol", &m_tCommon_Desc.iBaseSpriteMaxCol, sizeof(int))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fColBaseUVInterval", &fHeightUV, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fRowBaseUVInterval", &fWidthUV, sizeof(_float))))
				return E_FAIL;

		}
	}
	if (nullptr != m_pTexture[TEX_NORM])
	{
		if (FAILED(m_pTexture[TEX_NORM]->Set_SRV(m_pShader, "g_NormalTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vLightAmbient", &m_tParticle_Init_Desc.vLightAmbient, sizeof(_vector))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vLightDir", &m_tParticle_Init_Desc.vLightDir, sizeof(_float3))))
			return E_FAIL;
	}
	if (nullptr != m_pTexture[TEX_ALPHAMASK])
	{
		if (FAILED(m_pTexture[TEX_ALPHAMASK]->Set_SRV(m_pShader, "g_AlphaMask", 0)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bUseSpriteMask", &m_tCommon_Desc.bMaskSprite, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vAlphaMaskUVOffset", &m_vAlphaMaskUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iAlphaMaskSampler", &m_tCommon_UV_Desc.iAlphaMaskSampler, sizeof(_int))))
			return E_FAIL;

		if (true == m_tCommon_Desc.bMaskSprite)
		{

			_float fWidthUV = 1.f / m_tCommon_Desc.iMaskSpriteMaxCol;
			_float fHeightUV = 1.f / m_tCommon_Desc.iMaskSpriteMaxRow;
			if (FAILED(m_pShader->Bind_RawValue("g_iMaskCol", &m_tCommon_Desc.iMaskSpriteMaxCol, sizeof(int))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fColMaskUVInterval", &fHeightUV, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fRowMaskUVInterval", &fWidthUV, sizeof(_float))))
				return E_FAIL;
		}

	}
	if (nullptr != m_pTexture[TEX_COLOR_SCALE_UV])
	{
		if (FAILED(m_pTexture[TEX_COLOR_SCALE_UV]->Set_SRV(m_pShader, "g_ColorScaleUV", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vColorScaleUVOffset", &m_vColorScaleUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iColorScaleSampler", &m_tCommon_UV_Desc.iColorScaleUVSampler, sizeof(_int))))
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
	if (nullptr != m_pTexture[TEX_COLORSCALE])
	{
		if (FAILED(m_pTexture[TEX_COLORSCALE]->Set_SRV(m_pShader, "g_ColorScaleTexture", 0)))
			return E_FAIL;
	}
	if (nullptr != m_pTexture[TEX_DISTORTION])
	{
		if (FAILED(m_pTexture[TEX_DISTORTION]->Set_SRV(m_pShader, "g_DistortionTexture", 0)))
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
	if (nullptr != m_pTexture[TEX_BASE_NOISE_UV])
	{
		if (FAILED(m_pTexture[TEX_BASE_NOISE_UV]->Set_SRV(m_pShader, "g_BaseNoiseUVTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vBaseNoiseUVOffset", &m_vBaseNoiseUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iBaseNoiseSampler", &m_tCommon_UV_Desc.iBaseNoiseSampler, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vBaseNoiseInfoFlag", &m_tCommon_UV_Desc.vBaseNoiseUVInfoFlag, sizeof(_float3))))
			return E_FAIL;

	}
	if (nullptr != m_pTexture[TEX_ALPHAMASK_NOISE_UV])
	{
		if (FAILED(m_pTexture[TEX_ALPHAMASK_NOISE_UV]->Set_SRV(m_pShader, "g_AlphaMaskNoiseUVTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vAlphaMaskNoiseUVOffset", &m_vAlphaMaskNoiseUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iAlphaMaskNoiseSampler", &m_tCommon_UV_Desc.iAlphaMaskNoiseSampler, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vAlphaNoiseInfoFlag", &m_tCommon_UV_Desc.vAlphaNoiseUVInfoFlag, sizeof(_float3))))
			return E_FAIL;
	}

	if (1 == m_iPassIdx) // SOFT
	{
		if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Depth", m_pShader, "g_DepthTexture")))
			return E_FAIL;
	}

	return S_OK;
}

_bool CEffect_Paticle_Tool::ComputeStartDelay(size_t iParticleIdx, _float fTimeDelta)
{
	if (0 <= m_PaticleDesc_Current[iParticleIdx].fStartDelay)
	{
		// Prevent Render.
		m_ParticleVertices_Current[iParticleIdx].fLifeTime = 0.f;
		m_PaticleDesc_Current[iParticleIdx].fStartDelay -= fTimeDelta;


		if (0.f > m_PaticleDesc_Current[iParticleIdx].fStartDelay)
		{
			XMStoreFloat4(&m_ParticleVertices_Original[iParticleIdx].vTranslation, XMVectorSetW(XMLoadFloat3(&JoRandom::Random_Float3(m_tCommon_Desc.vStartPosMin, m_tCommon_Desc.vStartPosMax)), 1.f));
			m_ParticleVertices_Current[iParticleIdx] = m_ParticleVertices_Original[iParticleIdx];
			if (FOLLOW == m_eTF_Type)
			{
				_matrix ParticleMat = XMLoadFloat4x4((_float4x4*)&m_ParticleVertices_Original[iParticleIdx]);
				_matrix BoneMat = XMLoadFloat4x4(&m_AttachWorldMat);
				CALC_TF->Set_WorldMatrix(ParticleMat * BoneMat);
				
				XMStoreFloat3(&m_ParticleVertices_Current[iParticleIdx].vVelocity, XMVector3TransformNormal(XMLoadFloat3(&m_ParticleVertices_Original[iParticleIdx].vVelocity), XMLoadFloat4x4(&m_AttachWorldMat)));
				XMStoreFloat3(&m_PaticleDesc_Current[iParticleIdx].vPowerDir, XMVector3TransformNormal(XMLoadFloat3(&m_PaticleDesc_Original[iParticleIdx].vPowerDir), XMLoadFloat4x4(&m_AttachWorldMat)));


				memcpy(&m_ParticleVertices_Current[iParticleIdx], &CALC_TF->Get_WorldFloat4x4(), sizeof(_float4x4));
			}
			return true;
		}
		return false;
	}

	return true;
}

void CEffect_Paticle_Tool::ComputeLifeTime(size_t iParticleIdx, _float fTimeDelta)
{
	m_ParticleVertices_Current[iParticleIdx].fLifeTime -= fTimeDelta;

	if (0.f >= m_ParticleVertices_Current[iParticleIdx].fLifeTime)
	{
		m_ParticleVertices_Current[iParticleIdx].fLifeTime = 0.f;

		if (true == m_tParticle_Init_Desc.bParticleLoop)
		{
			m_PaticleDesc_Current[iParticleIdx] = m_PaticleDesc_Original[iParticleIdx];
		}
	}
}

void CEffect_Paticle_Tool::Update_Position(size_t iParticleIdx, _float fTimeDelta)
{

	_float3 vCurPos = {};

	if (true == m_tParticle_Init_Desc.bStopParticle && false == m_EndStop)
	{
		if (true == m_bStoped && STOP_TIME == m_tParticle_Init_Desc.iStopType)
			return;

		if (STOP_DISTANCE == m_tParticle_Init_Desc.iStopType
			&& m_MoveDistance[iParticleIdx] >= m_tParticle_Init_Desc.fStopDistance)
			return;

		XMStoreFloat3(&vCurPos, XMLoadFloat4(&m_ParticleVertices_Current[iParticleIdx].vTranslation));
	}

	if (true == m_tParticle_Init_Desc.bHasTargetPoint)
	{
		_float4x4 TransformMatrix4x4;
		memcpy(&TransformMatrix4x4, &m_ParticleVertices_Current[iParticleIdx], sizeof(_float4x4));

		CALC_TF->Set_WorldMatrix(XMLoadFloat4x4(&TransformMatrix4x4));

		_vector vDir;

		vDir = XMVectorSetW(XMLoadFloat3(&m_vTargetPoint) - CALC_TF->Get_Position(), 0.f);

		if (true == m_tParticle_Init_Desc.bChangeDirAfterStop && true == m_EndStop)
			vDir = XMVectorSetW(XMLoadFloat3(&m_AfterStopDir[iParticleIdx]), 0.f);

		if (m_tParticle_Init_Desc.fMargin < XMVectorGetX(XMVector3Length(vDir))
			|| (true == m_tParticle_Init_Desc.bChangeDirAfterStop && true == m_EndStop))
		{
			CALC_TF->Set_Speed(m_PaticleDesc_Current[iParticleIdx].fSpeed);
			CALC_TF->Go_Dir(XMVector3Normalize(vDir), fTimeDelta, nullptr);
		}
		else
		{
			if (true == m_tParticle_Init_Desc.bTargetLifeZero)
				m_ParticleVertices_Current[iParticleIdx].fLifeTime = 0.f;
			else
			{
				if (false == m_tParticle_Init_Desc.bChangeDirAfterStop || false == m_EndStop)
					return;
			}

		}

		XMStoreFloat4(&m_ParticleVertices_Current[iParticleIdx].vTranslation, CALC_TF->Get_Position());

		if (STOP_DISTANCE == m_tParticle_Init_Desc.iStopType && false == m_EndStop)
		{
			m_MoveDistance[iParticleIdx] += XMVectorGetX(XMVector3Length(CALC_TF->Get_Position() - XMLoadFloat3(&vCurPos)));
		}
		return;
	}

	_vector vVelocity = XMLoadFloat3(&m_ParticleVertices_Current[iParticleIdx].vVelocity);

	_vector vPosition = XMLoadFloat4(&m_ParticleVertices_Current[iParticleIdx].vTranslation) + vVelocity * fTimeDelta;


	XMStoreFloat4(&m_ParticleVertices_Current[iParticleIdx].vTranslation, vPosition);

	if (STOP_DISTANCE == m_tParticle_Init_Desc.iStopType && false == m_EndStop)
	{
		m_MoveDistance[iParticleIdx] += XMVectorGetX(XMVector3Length(vPosition - XMLoadFloat3(&vCurPos)));
	}
}

void CEffect_Paticle_Tool::Update_Speed(size_t iParticleIdx, _float fTimeDelta)
{
	if (true == m_tParticle_Init_Desc.bStopParticle && false == m_EndStop)
	{
		if (true == m_bStoped && STOP_TIME == m_tParticle_Init_Desc.iStopType)
			return;

		if (STOP_DISTANCE == m_tParticle_Init_Desc.iStopType
			&& m_MoveDistance[iParticleIdx] >= m_tParticle_Init_Desc.fStopDistance)
			return;
	}

	if (true == m_tParticle_Init_Desc.bHasTargetPoint)
	{
		_float3 vVelocity = m_ParticleVertices_Current[iParticleIdx].vVelocity;
		_vector vDir;

		vDir = XMVectorSetW(XMLoadFloat3(&m_vTargetPoint) - CALC_TF->Get_Position(), 0.f);
		if (true == m_tParticle_Init_Desc.bChangeDirAfterStop && true == m_EndStop)
			vDir = XMVectorSetW(XMLoadFloat3(&m_AfterStopDir[iParticleIdx]), 0.f);

		XMStoreFloat3(&m_ParticleVertices_Current[iParticleIdx].vVelocity, XMVector3Normalize(vDir) * m_PaticleDesc_Current[iParticleIdx].fSpeed /**XMLoadFloat3(&vVelocity)*/);

		return;
	}


	_vector vSpeed = XMLoadFloat3(&m_ParticleVertices_Current[iParticleIdx].vVelocity);

	vSpeed += XMLoadFloat3(&m_PaticleDesc_Current[iParticleIdx].vPowerDir) * fTimeDelta;


	XMStoreFloat3(&m_ParticleVertices_Current[iParticleIdx].vVelocity, vSpeed);
}

void CEffect_Paticle_Tool::Update_Size(size_t iParticleIdx)
{
	if (false == m_tCommon_Desc.bLerpSize)
		return;

	_vector vCurRight = XMLoadFloat4(&m_ParticleVertices_Current[iParticleIdx].vRight);
	_vector vCurUp = XMLoadFloat4(&m_ParticleVertices_Current[iParticleIdx].vUp);

	_float fOriginalSizeX = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_ParticleVertices_Original[iParticleIdx].vRight)));
	_float fOriginalSizeY = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_ParticleVertices_Original[iParticleIdx].vUp)));

	_float fLifeLerpRatio = 1 - m_ParticleVertices_Current[iParticleIdx].fLifeTime
		/ m_ParticleVertices_Original[iParticleIdx].fLifeTime;

	_float fCurSizeX = JoMath::Lerp(fOriginalSizeX, m_tParticle_Init_Desc.vSizeOverLife.x, fLifeLerpRatio);
	_float fCurSizeY = JoMath::Lerp(fOriginalSizeY, m_tParticle_Init_Desc.vSizeOverLife.y, fLifeLerpRatio);

	vCurRight = XMVector3Normalize(vCurRight) * fCurSizeX;
	vCurUp = XMVector3Normalize(vCurUp) * fCurSizeY;

	XMStoreFloat4(&m_ParticleVertices_Current[iParticleIdx].vRight, vCurRight);
	XMStoreFloat4(&m_ParticleVertices_Current[iParticleIdx].vUp, vCurUp);
}

void CEffect_Paticle_Tool::Update_Rotation(size_t iParticleIdx, _float fTimeDelta)
{

	//_float3 vRot = m_tParticle_Init_Desc.vRotAxis;
	if (0.f == m_tParticle_Init_Desc.fRotSpeed)
		return;

	if (NONE != m_eParticle_Mode)
	{
		_float fCur = m_ParticleVertices_Current[iParticleIdx].fRotation;

		m_ParticleVertices_Current[iParticleIdx].fRotation += m_tParticle_Init_Desc.fRotSpeed * fTimeDelta;
	}
	else
	{
		_matrix TransformMatrix;
		memcpy(&TransformMatrix, &m_ParticleVertices_Current[iParticleIdx], sizeof(_matrix));

		CALC_TF->Set_WorldMatrix(TransformMatrix);
		_vector		vRight;
		_vector		vUp;
		_vector		vLook;
		_vector		vAxis;

		if (m_tCommon_Desc.bRotationLocal)
		{
			vRight = XMLoadFloat4(&m_ParticleVertices_Current[iParticleIdx].vRight);
			vUp = XMLoadFloat4(&m_ParticleVertices_Current[iParticleIdx].vUp);
			vLook = XMLoadFloat4(&m_ParticleVertices_Current[iParticleIdx].vLook);
			vAxis = XMVector3Normalize(vRight * m_tParticle_Init_Desc.vRotAxis.x + vUp * m_tParticle_Init_Desc.vRotAxis.y + vLook * m_tParticle_Init_Desc.vRotAxis.z);
		}
		else
		{
			vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
			vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

			vAxis = XMVector3Normalize(vRight * m_tParticle_Init_Desc.vRotAxis.x + vUp * m_tParticle_Init_Desc.vRotAxis.y + vLook * m_tParticle_Init_Desc.vRotAxis.z);
		}

		CALC_TF->Set_RotationSpeed(m_tParticle_Init_Desc.fRotSpeed);
		CALC_TF->Turn(vAxis, fTimeDelta);

		memcpy(&m_ParticleVertices_Current[iParticleIdx], &CALC_TF->Get_WorldMatrix(), sizeof(_float4x4));
	}

}

void CEffect_Paticle_Tool::Update_Color(size_t iParticleIdx, _float fTimeDelta)
{
	if (true == m_tCommon_Desc.bEnableColorOverLife)
	{
		_float fTimeLerpRatio = (1.f - m_ParticleVertices_Current[iParticleIdx].fLifeTime
			/ m_ParticleVertices_Original[iParticleIdx].fLifeTime);

		_vector vColor = XMVectorLerp(XMLoadFloat4(&m_ParticleVertices_Original[iParticleIdx].vColor),
			XMLoadFloat4(&m_tCommon_Desc.vColorOverLife), fTimeLerpRatio);

		XMStoreFloat4(&m_ParticleVertices_Current[iParticleIdx].vColor, vColor);
	}

}

void CEffect_Paticle_Tool::First_Setting_Particle()
{
	for (size_t i = 0; i < m_PaticleDesc_Original.size(); ++i)
	{
		VTXPARTICLE Particle;
		HR::PARTICLE_DESC_TEST Desc;

		_float2 vScale = JoRandom::Random_Float2(m_tParticle_Init_Desc.vStartSizeMin, m_tParticle_Init_Desc.vStartSizeMax);
		if (true == m_tParticle_Init_Desc.bisSquare) vScale.x = vScale.y;
		_float4 vColor = JoRandom::Random_Float4(m_tCommon_Desc.vStartColorMin, m_tCommon_Desc.vStartColorMax);
		_float fSpeed = JoRandom::Random_Float(m_tCommon_Desc.vSpeedMinMax.x, m_tCommon_Desc.vSpeedMinMax.y);
		_float3 vPos = JoRandom::Random_Float3(m_tCommon_Desc.vStartPosMin, m_tCommon_Desc.vStartPosMax);
		_float3 vDir = JoRandom::Random_Float3(m_tCommon_Desc.vSpeedDirMin, m_tCommon_Desc.vSpeedDirMax);
		_float fLife = JoRandom::Random_Float(m_tCommon_Desc.fLifeTimeMin, m_tCommon_Desc.fLifeTimeMax);
		_float3 vRot = JoRandom::Random_Float3(m_tCommon_Desc.vStartRotationMin, m_tCommon_Desc.vStartRotationMax);
		_float3 vPowerDir = JoRandom::Random_Float3(m_tCommon_Desc.vPowerDirMin, m_tCommon_Desc.vPowerDirMax);
		_float fDelay = JoRandom::Random_Float(m_tCommon_Desc.fStartDelayMin, m_tCommon_Desc.fStartDelayMax);
		m_vAlphaMaskUVOffset = m_tCommon_UV_Desc.vStartAlphaMaskUVOffset;
		m_vColorScaleUVOffset = m_tCommon_UV_Desc.vStartColorScaleUVOffset;
		m_vNoiseUVOffset = m_tCommon_UV_Desc.vStartNoiseUVOffset;
		m_vBaseUVOffset = m_tCommon_UV_Desc.vStartBaseUVOffset;
		m_vBaseNoiseUVOffset = m_tCommon_UV_Desc.vStartBaseNoiseUVOffset;
		m_vAlphaMaskNoiseUVOffset = m_tCommon_UV_Desc.vStartAlphaMaskNoiseUVOffset;
		m_vDissolveUVOffset = m_tCommon_UV_Desc.vStartDissolveUVOffset;

		if (true == m_bCPUCODE)
		{

			if (true == m_tCommon_Desc.ChangeBaseSprite)
				m_SpriteChangeTimes[i] = JoRandom::Random_Float(m_tCommon_Desc.fBaseSpriteChangeTime - m_tParticle_Init_Desc.fSpriteChangeTolerance
					, m_tCommon_Desc.fBaseSpriteChangeTime + m_tParticle_Init_Desc.fSpriteChangeTolerance);
			if (true == m_tCommon_Desc.ChangeMaskSprite)
				m_SpriteChangeTimes[i] = JoRandom::Random_Float(m_tCommon_Desc.fMaskSpriteChangeTime - m_tParticle_Init_Desc.fSpriteChangeTolerance
					, m_tCommon_Desc.fMaskSpriteChangeTime + m_tParticle_Init_Desc.fSpriteChangeTolerance);

			if (true == m_tParticle_Init_Desc.bChangeDirAfterStop)
			{
				m_AfterStopDir[i] = JoRandom::Random_Float3(m_tParticle_Init_Desc.vAfterDirMin, m_tParticle_Init_Desc.vAfterDirMax);
			}
		}
		else
		{
			if (true == m_tCommon_Desc.ChangeBaseSprite)
				Desc.fSpriteChangeTime = JoRandom::Random_Float(m_tCommon_Desc.fBaseSpriteChangeTime - m_tParticle_Init_Desc.fSpriteChangeTolerance
					, m_tCommon_Desc.fBaseSpriteChangeTime + m_tParticle_Init_Desc.fSpriteChangeTolerance);
			if (true == m_tCommon_Desc.ChangeMaskSprite)
				Desc.fSpriteChangeTime = JoRandom::Random_Float(m_tCommon_Desc.fMaskSpriteChangeTime - m_tParticle_Init_Desc.fSpriteChangeTolerance
					, m_tCommon_Desc.fMaskSpriteChangeTime + m_tParticle_Init_Desc.fSpriteChangeTolerance);

			if (true == m_tParticle_Init_Desc.bChangeDirAfterStop)
			{
				Desc.vAfterStop_Dir = JoRandom::Random_Float3(m_tParticle_Init_Desc.vAfterDirMin, m_tParticle_Init_Desc.vAfterDirMax);
			}
		}
		if (true == m_tCommon_Desc.bMaskSprite)
		{
			_int iRandIdx = {};
			if (0 != m_tCommon_Desc.iMaskSpriteMaxCol && m_tCommon_Desc.iMaskSpriteMaxRow)
				iRandIdx = rand() % (m_tCommon_Desc.iMaskSpriteMaxCol * m_tCommon_Desc.iMaskSpriteMaxRow);

			Particle.iSpriteIndex = iRandIdx;
		}
		if (true == m_tCommon_Desc.bBaseSprite)
		{
			_int iRandIdx = {};
			if (0 != m_tCommon_Desc.iBaseSpriteMaxCol && m_tCommon_Desc.iBaseSpriteMaxRow)
				iRandIdx = rand() % (m_tCommon_Desc.iBaseSpriteMaxCol * m_tCommon_Desc.iBaseSpriteMaxRow);

			Particle.iSpriteIndex = iRandIdx;
		}

		if (true == m_tCommon_Desc.ChangeBaseSprite || true == m_tCommon_Desc.ChangeMaskSprite)
		{
			Particle.iSpriteIndex = 0;
		}


		// Rotation
		_matrix TransformMat = XMMatrixRotationRollPitchYaw(To_Radian(vRot.x), To_Radian(vRot.y), To_Radian(vRot.z));

		Particle.fRotation = To_Radian(vRot.z);

		//Scale
		TransformMat.r[0] = XMVector3Normalize(TransformMat.r[0]) * vScale.x;
		TransformMat.r[1] = XMVector3Normalize(TransformMat.r[1]) * vScale.y;

		// Position
		TransformMat.r[3] = XMVectorSetW(XMLoadFloat3(&vPos), 1.f);
		memcpy(&Particle, &TransformMat, sizeof(_matrix));

		// Color
		Particle.vColor = vColor;


		// Speed
		XMStoreFloat3(&Particle.vVelocity, (XMLoadFloat3(&vDir) * fSpeed));
		Desc.fSpeed = fSpeed;
		Desc.vSpeedDir = vDir;

		//Power
		Desc.vPowerDir = vPowerDir;

		// Life
		Particle.fLifeTime = fLife;

		// Delay
		Desc.fStartDelay = fDelay;

		// Dissolve
		Particle.fDissolve = 0.f;

		// Tartget
		Update_TargetPos();

		m_ParticleVertices_Original[i] = Particle;
		m_ParticleVertices_Current[i] = Particle;

		m_PaticleDesc_Original[i] = Desc;
		m_PaticleDesc_Current[i] = Desc;

	}
}

void CEffect_Paticle_Tool::Create_Buffers()
{
	m_tCommon_Effect_Buffer.iTransformType = m_tCommon_Desc.iTransformType;
	m_tCommon_Effect_Buffer.iTextureFlag = m_iTextureFlag;
	m_tCommon_Effect_Buffer.bEffectLoop = m_tCommon_Desc.bEffectLoop;
	m_tCommon_Effect_Buffer.bLerpSize = m_tCommon_Desc.bLerpSize;
	m_tCommon_Effect_Buffer.fSpeedOverLifeTime = m_tCommon_Desc.fSpeedOverLifeTime;
	m_tCommon_Effect_Buffer.bEnableColorOverLife = m_tCommon_Desc.bEnableColorOverLife;
	m_tCommon_Effect_Buffer.vStartPosMin = m_tCommon_Desc.vStartPosMin;
	m_tCommon_Effect_Buffer.vStartPosMax = m_tCommon_Desc.vStartPosMax;
	m_tCommon_Effect_Buffer.vColorOverLife = m_tCommon_Desc.vColorOverLife;
	m_tCommon_Effect_Buffer.vRotationOverLife = m_tCommon_Desc.vRotationOverLife;
	m_tCommon_Effect_Buffer.bRotationLocal = m_tCommon_Desc.bRotationLocal;
	m_tCommon_Effect_Buffer.fTotalPlayTime = m_tCommon_Desc.fTotalPlayTime;
	m_tCommon_Effect_Buffer.bDissolveReverse = m_tCommon_Desc.bDissolveReverse;
	m_tCommon_Effect_Buffer.fDissolveSpeed = m_tCommon_Desc.fDissolveSpeed;
	m_tCommon_Effect_Buffer.fDissolveStartTime = m_tCommon_Desc.fDissolveStartTime;
	m_tCommon_Effect_Buffer.bBaseSprite = m_tCommon_Desc.bBaseSprite;
	m_tCommon_Effect_Buffer.iBaseSpriteMaxRow = m_tCommon_Desc.iBaseSpriteMaxRow;
	m_tCommon_Effect_Buffer.iBaseSpriteMaxCol = m_tCommon_Desc.iBaseSpriteMaxCol;
	m_tCommon_Effect_Buffer.ChangeBaseSprite = m_tCommon_Desc.ChangeBaseSprite;
	m_tCommon_Effect_Buffer.fBaseSpriteChangeTime = m_tCommon_Desc.fBaseSpriteChangeTime;
	m_tCommon_Effect_Buffer.bMaskSprite = m_tCommon_Desc.bMaskSprite;
	m_tCommon_Effect_Buffer.iMaskSpriteMaxRow = m_tCommon_Desc.iMaskSpriteMaxRow;
	m_tCommon_Effect_Buffer.iMaskSpriteMaxCol = m_tCommon_Desc.iMaskSpriteMaxCol;
	m_tCommon_Effect_Buffer.ChangeMaskSprite = m_tCommon_Desc.ChangeMaskSprite;
	m_tCommon_Effect_Buffer.fMaskSpriteChangeTime = m_tCommon_Desc.fMaskSpriteChangeTime;

	m_tParticle_Init_Desc_Test.iNumInstance = m_tParticle_Init_Desc.iNumInstance;
	m_tParticle_Init_Desc_Test.eParticleMode = m_tParticle_Init_Desc.eParticleMode;
	m_tParticle_Init_Desc_Test.bParticleLoop = m_tParticle_Init_Desc.bParticleLoop;
	m_tParticle_Init_Desc_Test.bisSquare = m_tParticle_Init_Desc.bisSquare;
	m_tParticle_Init_Desc_Test.vStartSizeMin = m_tParticle_Init_Desc.vStartSizeMin;
	m_tParticle_Init_Desc_Test.vStartSizeMax = m_tParticle_Init_Desc.vStartSizeMax;
	m_tParticle_Init_Desc_Test.vSizeOverLife = m_tParticle_Init_Desc.vSizeOverLife;
	m_tParticle_Init_Desc_Test.vPadding0 = m_tParticle_Init_Desc.vPadding0;
	m_tParticle_Init_Desc_Test.vRotAxis = m_tParticle_Init_Desc.vRotAxis;
	m_tParticle_Init_Desc_Test.fRotSpeed = m_tParticle_Init_Desc.fRotSpeed;
	m_tParticle_Init_Desc_Test.bBillboardWithoutUp = m_tParticle_Init_Desc.bBillboardWithoutUp;
	m_tParticle_Init_Desc_Test.fStretchFactor = m_tParticle_Init_Desc.fStretchFactor;
	m_tParticle_Init_Desc_Test.vPadding1 = m_tParticle_Init_Desc.vPadding1;
	m_tParticle_Init_Desc_Test.vLightDir = m_tParticle_Init_Desc.vLightDir;
	m_tParticle_Init_Desc_Test.fPadding2 = m_tParticle_Init_Desc.fPadding2;
	m_tParticle_Init_Desc_Test.vLightAmbient = m_tParticle_Init_Desc.vLightAmbient;
	m_tParticle_Init_Desc_Test.bHasTargetPoint = m_tParticle_Init_Desc.bHasTargetPoint;
	m_tParticle_Init_Desc_Test.bTargetWorldOffSet = m_tParticle_Init_Desc.bTargetWorldOffSet;
	m_tParticle_Init_Desc_Test.vPadding2 = m_tParticle_Init_Desc.vPadding2;
	m_tParticle_Init_Desc_Test.vTargetPointOffset = m_tParticle_Init_Desc.vTargetPointOffset;
	m_tParticle_Init_Desc_Test.fMargin = m_tParticle_Init_Desc.fMargin;
	m_tParticle_Init_Desc_Test.bFollowTargetPos = m_tParticle_Init_Desc.bFollowTargetPos;
	m_tParticle_Init_Desc_Test.bTargetLifeZero = m_tParticle_Init_Desc.bTargetLifeZero;
	m_tParticle_Init_Desc_Test.bXStretch = m_tParticle_Init_Desc.bXStretch;
	m_tParticle_Init_Desc_Test.bYStretch = m_tParticle_Init_Desc.bYStretch;
	m_tParticle_Init_Desc_Test.bStopParticle = m_tParticle_Init_Desc.bStopParticle;
	m_tParticle_Init_Desc_Test.iStopType = m_tParticle_Init_Desc.iStopType;
	m_tParticle_Init_Desc_Test.fStopTime = m_tParticle_Init_Desc.fStopTime;
	m_tParticle_Init_Desc_Test.fStopDistance = m_tParticle_Init_Desc.fStopDistance;
	m_tParticle_Init_Desc_Test.fMaintainTime = m_tParticle_Init_Desc.fMaintainTime;
	m_tParticle_Init_Desc_Test.bChangeDirAfterStop = m_tParticle_Init_Desc.bChangeDirAfterStop;
	m_tParticle_Init_Desc_Test.vPadding3 = m_tParticle_Init_Desc.vPadding3;
	m_tParticle_Init_Desc_Test.vAfterDirMin = m_tParticle_Init_Desc.vAfterDirMin;
	m_tParticle_Init_Desc_Test.fPadding4 = m_tParticle_Init_Desc.fPadding4;
	m_tParticle_Init_Desc_Test.vAfterDirMax = m_tParticle_Init_Desc.vAfterDirMax;
	m_tParticle_Init_Desc_Test.fPadding5 = m_tParticle_Init_Desc.fPadding5;
	m_tParticle_Init_Desc_Test.fSpriteChangeTolerance = m_tParticle_Init_Desc.fSpriteChangeTolerance;
	m_tParticle_Init_Desc_Test.vPadding6 = m_tParticle_Init_Desc.vPadding6;

	Safe_Release(m_pCommon_Particle_Data);
	Safe_Release(m_pCommon_Effect_Data);
	Safe_Release(m_pTick_Data);

	Safe_Release(m_pCurrentVerticesUAV);
	Safe_Release(m_pOriginalVerticesUAV);
	Safe_Release(m_pCurrent_EachDesc_UAV);
	Safe_Release(m_pOriginal_EachDesc_UAV);

	Safe_Release(m_pCurrentVerticesStaging);
	Safe_Release(m_pCurrentVerticesBuffer);
	Safe_Release(m_pOriginalVerticesBuffer);
	Safe_Release(m_pCurrent_EachDesc_Buffer);
	Safe_Release(m_pOriginal_EachDesc_Buffer);


	// 공통으로 사용되는 Particle Init
	{
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.ByteWidth = sizeof(PARTICLE_INIT_DESC_TEST);
		BufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = &m_tParticle_Init_Desc_Test;

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitData, &m_pCommon_Particle_Data)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}
	}

	// 이펙트 Common 중 CS에 전달되어야 하는 것들
	{
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.ByteWidth = sizeof(COMMON_EFFECT_BUFFER);
		BufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = &m_tCommon_Effect_Buffer;

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitData, &m_pCommon_Effect_Data)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}

	}

	//매 틱마다 전달 되어야 하는 Buffer
	{
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.ByteWidth = sizeof(Tick_Buffer);
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pTick_Data)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}
	}





	// ************* Vertices **************
	{
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(VTXPARTICLE) * m_tParticle_Init_Desc.iNumInstance;
		BufferDesc.StructureByteStride = sizeof(VTXPARTICLE);
		BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = m_ParticleVertices_Original.data();

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitData, &m_pCurrentVerticesBuffer)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitData, &m_pOriginalVerticesBuffer)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}


		D3D11_UNORDERED_ACCESS_VIEW_DESC UAV_Desc = {};
		UAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		UAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
		UAV_Desc.Buffer.NumElements = m_tParticle_Init_Desc.iNumInstance;
		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOriginalVerticesBuffer, &UAV_Desc, &m_pOriginalVerticesUAV)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}
		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pCurrentVerticesBuffer, &UAV_Desc, &m_pCurrentVerticesUAV)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}



	}

	// ************* Desc **************
	{

		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.ByteWidth = sizeof(HR::PARTICLE_DESC_TEST) * m_tParticle_Init_Desc.iNumInstance;
		BufferDesc.StructureByteStride = sizeof(HR::PARTICLE_DESC_TEST);
		BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = m_PaticleDesc_Original.data();

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitData, &m_pOriginal_EachDesc_Buffer)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitData, &m_pCurrent_EachDesc_Buffer)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}

		D3D11_UNORDERED_ACCESS_VIEW_DESC UAV_Desc = {};
		UAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		UAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
		UAV_Desc.Buffer.NumElements = m_tParticle_Init_Desc.iNumInstance;
		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOriginal_EachDesc_Buffer, &UAV_Desc, &m_pOriginal_EachDesc_UAV)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}
		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pCurrent_EachDesc_Buffer, &UAV_Desc, &m_pCurrent_EachDesc_UAV)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}

	}

	// ********** Current Vertices Staging****************
	{
		//m_pCurrentVerticesStaging
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.BindFlags = 0;
		BufferDesc.Usage = D3D11_USAGE_STAGING;
		BufferDesc.ByteWidth = sizeof(VTXPARTICLE) * m_tParticle_Init_Desc.iNumInstance;
		BufferDesc.StructureByteStride = sizeof(VTXPARTICLE);
		BufferDesc.MiscFlags = 0;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

		if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pCurrentVerticesStaging)))
		{
			MSG_BOX(TEXT("FAILED - CEffect_Paticle_Tool::Create_Buffers"));
			return;
		}
	}

	_int iTotalThreads = m_tParticle_Init_Desc.iNumInstance * 2;
	//m_iNumGroup = (m_tParticle_Init_Desc.iNumInstance + m_iThreadPerGroup - 1) / m_iThreadPerGroup;
	m_iNumGroupX = static_cast<int>(ceil(sqrt((float)iTotalThreads / m_iThreadPerGroup)));
	m_iNumGroupY = (iTotalThreads + (m_iNumGroupX * m_iThreadPerGroup) - 1) / (m_iNumGroupX * m_iThreadPerGroup);

	D3D11_FEATURE_DATA_D3D11_OPTIONS3 featureData;
	HRESULT hr = m_pDevice->CheckFeatureSupport(
		D3D11_FEATURE_D3D11_OPTIONS3,
		&featureData,
		sizeof(featureData)
	);

	D3D11_FEATURE_DATA_THREADING threadingFeature;
	hr = m_pDevice->CheckFeatureSupport(
		D3D11_FEATURE_THREADING,
		&threadingFeature,
		sizeof(threadingFeature)
	);

	// 스레드 그룹당 최대 스레드 수
	UINT maxThreadsPerGroup = D3D11_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP; // 1024

	if (maxThreadsPerGroup < (_uint)m_iThreadPerGroup)
	{
		MSG_BOX(TEXT("Thread Number OverFlow!"));
	}


	return;

}


void CEffect_Paticle_Tool::Update_Sprite(size_t iParticleIdx, _float fTimeDelta)
{

	if (true == m_tCommon_Desc.bBaseSprite && true == m_tCommon_Desc.ChangeBaseSprite)
	{
		m_SpriteTimeAcc[iParticleIdx] += fTimeDelta;
		if (m_SpriteTimeAcc[iParticleIdx] >= m_SpriteChangeTimes[iParticleIdx])
		{
			m_SpriteTimeAcc[iParticleIdx] = 0.f;
			++m_ParticleVertices_Current[iParticleIdx].iSpriteIndex;
			if (m_tCommon_Desc.iBaseSpriteMaxCol * m_tCommon_Desc.iBaseSpriteMaxRow
				<= m_ParticleVertices_Current[iParticleIdx].iSpriteIndex)
			{
				m_ParticleVertices_Current[iParticleIdx].iSpriteIndex = 0;
			}
		}
	}

	if (true == m_tCommon_Desc.bMaskSprite && true == m_tCommon_Desc.ChangeMaskSprite)
	{
		m_SpriteTimeAcc[iParticleIdx] += fTimeDelta;
		if (m_SpriteTimeAcc[iParticleIdx] >= m_SpriteChangeTimes[iParticleIdx])
		{
			m_SpriteTimeAcc[iParticleIdx] = 0.f;
			++m_ParticleVertices_Current[iParticleIdx].iSpriteIndex;
			if (m_tCommon_Desc.iMaskSpriteMaxCol * m_tCommon_Desc.iMaskSpriteMaxRow
				<= m_ParticleVertices_Current[iParticleIdx].iSpriteIndex)
			{
				m_ParticleVertices_Current[iParticleIdx].iSpriteIndex = 0;
			}
		}
	}
}

void CEffect_Paticle_Tool::Update_TargetPos()
{
	if (true == m_tParticle_Init_Desc.bHasTargetPoint)
	{
		CALC_TF->Set_WorldMatrix(XMMatrixIdentity());
		if (nullptr != m_tAttachDesc.pParentTransform)
		{
			if (strcmp(m_tAttachDesc.szTargetBoneName, ""))
			{
				CALC_TF->Set_WorldMatrix(m_tAttachDesc.pParentTransform->Get_WorldMatrix());
				CALC_TF->Attach_To_Bone(m_tAttachDesc.pTargetBone, m_tAttachDesc.pParentTransform, XMMatrixIdentity(), true);
			}
			else
			{
				CALC_TF->Set_WorldMatrix(m_tAttachDesc.pParentTransform->Get_WorldMatrix());
			}
		}

		if (true == m_tParticle_Init_Desc.bTargetWorldOffSet)
			CALC_TF->Add_Position(XMLoadFloat3(&m_tParticle_Init_Desc.vTargetPointOffset), false);
		else
			CALC_TF->Add_Position(XMLoadFloat3(&m_tParticle_Init_Desc.vTargetPointOffset), true);

		_matrix AttchInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_AttachWorldMat));
		XMStoreFloat3(&m_vTargetPoint, XMVector3TransformCoord(CALC_TF->Get_Position(), AttchInverse));
	}
}

void CEffect_Paticle_Tool::Update_Dissolve(size_t iParticleIdx, _float fTimeDelta)
{
	if (0 == (m_iTextureFlag & DISSOLVE))
		return;

	m_DissolveTimeAcc[iParticleIdx] += fTimeDelta;

	if (m_tCommon_Desc.fDissolveStartTime > m_DissolveTimeAcc[iParticleIdx])
		return;

	m_ParticleVertices_Current[iParticleIdx].fDissolve += m_tCommon_Desc.fDissolveSpeed * fTimeDelta;

}

void CEffect_Paticle_Tool::Update_TickBuffer(_float fTimeDelta)
{
	m_tTick_Buffer.bEndStop = m_EndStop;
	m_tTick_Buffer.bStoped = m_bStoped;
	m_tTick_Buffer.fTimeDelta = fTimeDelta;
	m_tTick_Buffer.vTargetPoint = m_vTargetPoint;
	m_tTick_Buffer.bChangeDir = m_bChangeDir;
	m_tTick_Buffer.iDispatchX = m_iNumGroupX;
	XMStoreFloat4x4(&m_tTick_Buffer.AttachMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_AttachWorldMat)));

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(m_pContext->Map(m_pTick_Data, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		MSG_BOX(TEXT("FAILED To Map Staging Buffer"));
		return;
	}

	Tick_Buffer* pData = reinterpret_cast<Tick_Buffer*>(MappedResource.pData);

	memcpy(pData, &m_tTick_Buffer, sizeof(Tick_Buffer));

	m_pContext->Unmap(m_pTick_Data, 0);
}

void CEffect_Paticle_Tool::Bind_Compute_Shader_Datas()
{
	m_pCS->Set_ConstantBuffer(0, 1, &m_pCommon_Particle_Data);
	m_pCS->Set_ConstantBuffer(1, 1, &m_pCommon_Effect_Data);
	m_pCS->Set_ConstantBuffer(2, 1, &m_pTick_Data);

	m_pCS->Set_UnorderedAccess(0, 1, &m_pOriginalVerticesUAV);
	m_pCS->Set_UnorderedAccess(1, 1, &m_pCurrentVerticesUAV);
	m_pCS->Set_UnorderedAccess(2, 1, &m_pOriginal_EachDesc_UAV);
	m_pCS->Set_UnorderedAccess(3, 1, &m_pCurrent_EachDesc_UAV);
}

HRESULT CEffect_Paticle_Tool::Copy_Resource()
{

	return S_OK;
}


CEffect_Paticle_Tool* CEffect_Paticle_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Paticle_Tool* pInstance = new CEffect_Paticle_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Paticle_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Paticle_Tool::Clone(void* pArg)
{
	CEffect_Paticle_Tool* pInstance = new CEffect_Paticle_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Paticle_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Paticle_Tool::Free()
{
	__super::Free();

	m_PaticleDesc_Original.clear();
	m_PaticleDesc_Current.clear();
	m_ParticleVertices_Original.clear();
	m_ParticleVertices_Current.clear();

	Safe_Release(m_pCommon_Particle_Data);
	Safe_Release(m_pCommon_Effect_Data);
	Safe_Release(m_pTick_Data);

	Safe_Release(m_pCurrentVerticesUAV);
	Safe_Release(m_pOriginalVerticesUAV);
	Safe_Release(m_pCurrent_EachDesc_UAV);
	Safe_Release(m_pOriginal_EachDesc_UAV);

	Safe_Release(m_pCurrentVerticesStaging);
	Safe_Release(m_pCurrentVerticesBuffer);
	Safe_Release(m_pOriginalVerticesBuffer);
	Safe_Release(m_pCurrent_EachDesc_Buffer);
	Safe_Release(m_pOriginal_EachDesc_Buffer);

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pCS);

}

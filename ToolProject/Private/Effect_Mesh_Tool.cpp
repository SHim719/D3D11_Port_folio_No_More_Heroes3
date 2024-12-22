#include "Effect_Mesh_Tool.h"

CEffect_Mesh_Tool::CEffect_Mesh_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect{ pDevice, pContext }
{
}

CEffect_Mesh_Tool::CEffect_Mesh_Tool(const CEffect_Mesh_Tool& rhs)
	: CEffect(rhs)
	, m_tMeshDesc{ rhs.m_tMeshDesc }
	, m_wszModelCompTag{ rhs.m_wszModelCompTag }
{
	if (lstrcmp(m_wszModelCompTag.c_str(), TEXT("")))
	{
		if (FAILED(__super::Add_Component((LEVEL)GET_CURLEVEL, m_wszModelCompTag, TEXT("Model"), reinterpret_cast<CComponent**>(&m_pModel))))
			MSG_BOX(TEXT("FAILED - CEffect_Mesh_Tool::Set_Model_Component"));
	}
}

HRESULT CEffect_Mesh_Tool::Initialize_Prototype()
{
	m_eEffect_Type = MESH;
	return S_OK;
}

HRESULT CEffect_Mesh_Tool::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Reset_Setting();

	return S_OK;
}

void CEffect_Mesh_Tool::PriorityTick(_float fTimeDelta)
{
}

void CEffect_Mesh_Tool::Tick(_float fTimeDelta)
{
	if (true == m_isEnd)
		return;

	if (false == Compute_StartDelay(fTimeDelta))
		return;

	m_bMaintain = Compute_MaintainTime(fTimeDelta);

	Update_Sprite(fTimeDelta);
	Update_Speed();
	Update_Position(fTimeDelta);
	Update_Rotation(fTimeDelta);
	Update_Scale();
	Update_Color();
	Update_TexCoord(fTimeDelta);
	Update_Dissolve(fTimeDelta);
	Update_FinalMatrix(fTimeDelta);


	if (true == m_bMaintain)
		return;

	m_fTimeAcc += fTimeDelta;
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

void CEffect_Mesh_Tool::LateTick(_float fTimeDelta)
{
	if (true == m_isEnd || m_fStarTimeAcc >= 0.f)
		return;
	_uint iRenderGroup = CRenderer::RENDER_EFFECT_NONBLEND + m_tCommon_Desc.iRenderGroup;
	m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)iRenderGroup, this);

	if (true == m_tCommon_Desc.bUseDistortion)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);

}

HRESULT CEffect_Mesh_Tool::Render()
{
	if (nullptr == m_pModel)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (nullptr != m_pModel)
	{
		if (FAILED(m_pModel->Bind_Buffers(0)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, 0, m_iPassIdx)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Mesh_Tool::Save_Effect(const _char* pTag, nlohmann::ordered_json& JsonData)
{
	if (FAILED(__super::Save_Effect(pTag, JsonData)))
		return E_FAIL;

	JsonData[pTag]["Mesh Description"]["bLerpRotation"] = m_tMeshDesc.bLerpRotation;
	JsonData[pTag]["Mesh Description"]["fTurnSpeed"] = m_tMeshDesc.fTurnSpeed;
	JsonData[pTag]["Mesh Description"]["vRotationAxis"] = { m_tMeshDesc.vRotationAxis.x, m_tMeshDesc.vRotationAxis.y, m_tMeshDesc.vRotationAxis.z, m_tMeshDesc.vRotationAxis.w };
	JsonData[pTag]["Mesh Description"]["vStartSizeMin"] = { m_tMeshDesc.vStartSizeMin.x, m_tMeshDesc.vStartSizeMin.y, m_tMeshDesc.vStartSizeMin.z };
	JsonData[pTag]["Mesh Description"]["vStartSizeMax"] = { m_tMeshDesc.vStartSizeMax.x, m_tMeshDesc.vStartSizeMax.y, m_tMeshDesc.vStartSizeMax.z };
	JsonData[pTag]["Mesh Description"]["vScaleOverLife"] = { m_tMeshDesc.vScaleOverLife.x, m_tMeshDesc.vScaleOverLife.y, m_tMeshDesc.vScaleOverLife.z };
	JsonData[pTag]["Mesh Description"]["vStartRotation"] = { m_tMeshDesc.vStartRotation.x, m_tMeshDesc.vStartRotation.y, m_tMeshDesc.vStartRotation.z };
	JsonData[pTag]["Mesh Description"]["vRotationOverLife"] = { m_tMeshDesc.vRotationOverLife.x, m_tMeshDesc.vRotationOverLife.y, m_tMeshDesc.vRotationOverLife.z };

	JsonData[pTag]["Mesh Description"]["bSizePhaseSecond"] = m_tMeshDesc.bSizePhaseSecond;
	JsonData[pTag]["Mesh Description"]["fSecondPhaseTime"] = m_tMeshDesc.fSecondPhaseTime;
	JsonData[pTag]["Mesh Description"]["vScaleOverLife_Secod"] = { m_tMeshDesc.vScaleOverLife_Secod.x, m_tMeshDesc.vScaleOverLife_Secod.y, m_tMeshDesc.vScaleOverLife_Secod.z };
	JsonData[pTag]["Mesh Description"]["bUseMaintainTime"] = m_tMeshDesc.bUseMaintainTime;
	JsonData[pTag]["Mesh Description"]["fMaintainStartTime"] = m_tMeshDesc.fMaintainStartTime;
	JsonData[pTag]["Mesh Description"]["fMaintainTime"] = m_tMeshDesc.fMaintainTime;
	JsonData[pTag]["Mesh Description"]["bDiscardRotation"] = m_tMeshDesc.bDiscardRotation;
	JsonData[pTag]["Mesh Description"]["Model Component Tag"] = Convert_WStrToStr(m_wszModelCompTag).c_str();

	return S_OK;
}

HRESULT CEffect_Mesh_Tool::Load_Effect(const _char* pTag, json& JsonData)
{
	__super::Load_Effect(pTag, JsonData);

	m_tMeshDesc.bLerpRotation = JsonData[pTag]["Mesh Description"]["bLerpRotation"];
	m_tMeshDesc.fTurnSpeed = JsonData[pTag]["Mesh Description"]["fTurnSpeed"];
	memcpy(&m_tMeshDesc.vRotationAxis, JsonData[pTag]["Mesh Description"]["vRotationAxis"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tMeshDesc.vStartSizeMin, JsonData[pTag]["Mesh Description"]["vStartSizeMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vStartSizeMax, JsonData[pTag]["Mesh Description"]["vStartSizeMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vScaleOverLife, JsonData[pTag]["Mesh Description"]["vScaleOverLife"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vStartRotation, JsonData[pTag]["Mesh Description"]["vStartRotation"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vRotationOverLife, JsonData[pTag]["Mesh Description"]["vRotationOverLife"].get<array<_float, 3>>().data(), sizeof(_float3));

	m_tMeshDesc.bSizePhaseSecond = JsonData[pTag]["Mesh Description"]["bSizePhaseSecond"];
	m_tMeshDesc.fSecondPhaseTime = JsonData[pTag]["Mesh Description"]["fSecondPhaseTime"];
	memcpy(&m_tMeshDesc.vScaleOverLife_Secod, JsonData[pTag]["Mesh Description"]["vScaleOverLife_Secod"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tMeshDesc.bUseMaintainTime = JsonData[pTag]["Mesh Description"]["bUseMaintainTime"];
	m_tMeshDesc.fMaintainStartTime = JsonData[pTag]["Mesh Description"]["fMaintainStartTime"];
	m_tMeshDesc.fMaintainTime = JsonData[pTag]["Mesh Description"]["fMaintainTime"];
	m_tMeshDesc.bDiscardRotation = JsonData[pTag]["Mesh Description"]["bDiscardRotation"];

	string strModelTag = JsonData[pTag]["Mesh Description"]["Model Component Tag"];
	m_wszModelCompTag = Convert_StrToWStr(strModelTag);
	Safe_Release(m_pModel);

	if (m_wszModelCompTag != TEXT(""))
	{
		if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, m_wszModelCompTag, TEXT("Model"), reinterpret_cast<CComponent**>(&m_pModel))))
			MSG_BOX(TEXT("FAILED - CEffect_Mesh_Tool::Set_Model_Component"));
	}

	return S_OK;
}
HRESULT CEffect_Mesh_Tool::Load_Effect_CINEMA(const _char* pTag, json& JsonData)
{
	if (FAILED(__super::Load_Effect_CINEMA(pTag,JsonData)))
		return E_FAIL;

	m_tMeshDesc.bLerpRotation = JsonData[m_szEffectTag]["Mesh Description"]["bLerpRotation"];
	m_tMeshDesc.fTurnSpeed = JsonData[m_szEffectTag]["Mesh Description"]["fTurnSpeed"];
	memcpy(&m_tMeshDesc.vRotationAxis, JsonData[m_szEffectTag]["Mesh Description"]["vRotationAxis"].get<array<_float, 4>>().data(), sizeof(_float4));
	memcpy(&m_tMeshDesc.vStartSizeMin, JsonData[m_szEffectTag]["Mesh Description"]["vStartSizeMin"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vStartSizeMax, JsonData[m_szEffectTag]["Mesh Description"]["vStartSizeMax"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vScaleOverLife, JsonData[m_szEffectTag]["Mesh Description"]["vScaleOverLife"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vStartRotation, JsonData[m_szEffectTag]["Mesh Description"]["vStartRotation"].get<array<_float, 3>>().data(), sizeof(_float3));
	memcpy(&m_tMeshDesc.vRotationOverLife, JsonData[m_szEffectTag]["Mesh Description"]["vRotationOverLife"].get<array<_float, 3>>().data(), sizeof(_float3));

	m_tMeshDesc.bSizePhaseSecond = JsonData[m_szEffectTag]["Mesh Description"]["bSizePhaseSecond"];
	m_tMeshDesc.fSecondPhaseTime = JsonData[m_szEffectTag]["Mesh Description"]["fSecondPhaseTime"];
	memcpy(&m_tMeshDesc.vScaleOverLife_Secod, JsonData[m_szEffectTag]["Mesh Description"]["vScaleOverLife_Secod"].get<array<_float, 3>>().data(), sizeof(_float3));
	m_tMeshDesc.bUseMaintainTime = JsonData[m_szEffectTag]["Mesh Description"]["bUseMaintainTime"];
	m_tMeshDesc.fMaintainStartTime = JsonData[m_szEffectTag]["Mesh Description"]["fMaintainStartTime"];
	m_tMeshDesc.fMaintainTime = JsonData[m_szEffectTag]["Mesh Description"]["fMaintainTime"];
	m_tMeshDesc.bDiscardRotation = JsonData[m_szEffectTag]["Mesh Description"]["bDiscardRotation"];

	string strModelTag = JsonData[m_szEffectTag]["Mesh Description"]["Model Component Tag"];
	m_wszModelCompTag = Convert_StrToWStr(strModelTag);

	Safe_Release(m_pModel);

	if (m_wszModelCompTag != TEXT(""))
	{
		if (FAILED(__super::Add_Component(GET_CURLEVEL, m_wszModelCompTag, TEXT("Model"), reinterpret_cast<CComponent**>(&m_pModel))))
			MSG_BOX(TEXT("FAILED - CEffect_Mesh_Tool::Set_Model_Component"));
	}

	return S_OK;
}
#pragma warning(default:6031)
#pragma warning(default:6029)

void CEffect_Mesh_Tool::Reset_Effect(CINE_EFFECT_DESC* pDesc)
{
	__super::Reset_Effect(pDesc);

	Reset_Setting();
}

void CEffect_Mesh_Tool::Set_Model_Component(const wstring& wstrModelTag, LEVEL eLevel)
{
	Safe_Release(m_pModel);
	Erase_Component(TEXT("Model"));

	if (FAILED(__super::Add_Component(eLevel, wstrModelTag, TEXT("Model"), reinterpret_cast<CComponent**>(&m_pModel))))
		MSG_BOX(TEXT("FAILED - CEffect_Mesh_Tool::Set_Model_Component"));


	m_wszModelCompTag = wstrModelTag;
}

void CEffect_Mesh_Tool::Remove_Model_Component()
{
	Safe_Release(m_pModel);
	m_wszModelCompTag = TEXT("");

	__super::Erase_Component(TEXT("Model"));
}

void CEffect_Mesh_Tool::Update_MeshEffect(const HR::MESH_DESC tMeshDesc, const HR::EFFECT_COMMON_DESC& tCommonDesc)
{
	__super::Update_Effet_Desc(tCommonDesc);

	memcpy(&m_tMeshDesc, &tMeshDesc, sizeof(HR::MESH_DESC));

	Reset_Setting();
}

void CEffect_Mesh_Tool::Reset_Setting()
{
	__super::Reset_Setting();
	Rendom_Value_Setting();

	//m_isEnd = false;
	//m_fStarTimeAcc = m_fInitStartDelay;


	m_pTransform->Set_WorldMatrix(XMMatrixIdentity());
	m_pTransform->Set_Position(XMVectorSetW(XMLoadFloat3(&m_vStartPos), 1.f));
	m_pTransform->Set_Scale(m_vStartSize);
	m_pTransform->Set_RotationSpeed(m_tMeshDesc.fTurnSpeed);
	_float3 vRot = m_tMeshDesc.vStartRotation;
	//_vector vQuat = XMVectorSet(To_Radian(vRot.z), To_Radian(vRot.x), To_Radian(vRot.y), 0.f);
	//m_pTransform->Rotation_Quaternion(vQuat);
	m_pTransform->Rotation_RollPitchYaw(To_Radian(vRot.z), To_Radian(vRot.x), To_Radian(vRot.y));
	m_pTransform->Set_Speed(m_fInitSpeed);
	m_fMaintainAcc = 0.f;
	m_bMaintain = false;
}

void CEffect_Mesh_Tool::Rendom_Value_Setting()
{
	m_fStarTimeAcc = JoRandom::Random_Float(m_tCommon_Desc.fStartDelayMin, m_tCommon_Desc.fStartDelayMax);
	m_vStartColor = JoRandom::Random_Float4(m_tCommon_Desc.vStartColorMin, m_tCommon_Desc.vStartColorMax);
	m_vStartPos = JoRandom::Random_Float3(m_tCommon_Desc.vStartPosMin, m_tCommon_Desc.vStartPosMax);
	m_fInitSpeed = JoRandom::Random_Float(m_tCommon_Desc.vSpeedMinMax.x, m_tCommon_Desc.vSpeedMinMax.y);
	m_vStartSpeedDir = JoRandom::Random_Float3(m_tCommon_Desc.vSpeedDirMin, m_tCommon_Desc.vSpeedDirMax);
	m_tMeshDesc.vStartRotation = JoRandom::Random_Float3(m_tCommon_Desc.vStartRotationMin, m_tCommon_Desc.vStartRotationMax);
	m_vStartSize = JoRandom::Random_Float3(m_tMeshDesc.vStartSizeMin, m_tMeshDesc.vStartSizeMax);
	if (true == m_tCommon_Desc.bMaskSprite)
	{
		_int iRandIdx = {};
		if (0 != m_tCommon_Desc.iMaskSpriteMaxCol && m_tCommon_Desc.iMaskSpriteMaxRow)
			iRandIdx = rand() % (m_tCommon_Desc.iMaskSpriteMaxCol * m_tCommon_Desc.iMaskSpriteMaxRow);

		m_iMaskSpriteIndex = iRandIdx;
	}
	if (true == m_tCommon_Desc.bBaseSprite)
	{
		_int iRandIdx = {};
		if (0 != m_tCommon_Desc.iBaseSpriteMaxCol && m_tCommon_Desc.iBaseSpriteMaxRow)
			iRandIdx = rand() % (m_tCommon_Desc.iBaseSpriteMaxCol * m_tCommon_Desc.iBaseSpriteMaxRow);

		m_iBaseSpriteIndex = iRandIdx;
	}


}

_bool CEffect_Mesh_Tool::Compute_StartDelay(_float fTimeDelta)
{
	if (0.f <= m_fStarTimeAcc)
	{

		m_fStarTimeAcc -= fTimeDelta;

		if (0.f > m_fStarTimeAcc)
		{
			__super::Reset_Setting();

			return true;
		}
		return false;
	}

	return true;
}

_bool CEffect_Mesh_Tool::Compute_MaintainTime(_float fTimeDelta)
{
	if (false == m_tMeshDesc.bUseMaintainTime)
		return false;
	if (m_fTimeAcc < m_tMeshDesc.fMaintainStartTime || m_fMaintainAcc >= m_tMeshDesc.fMaintainTime)
		return false;

	m_fMaintainAcc += fTimeDelta;

	return true;
}

void CEffect_Mesh_Tool::Update_Position(_float fTimeDelta)
{

	if (MOVE == m_eTF_Type)
	{

		if (nullptr != m_tAttachDesc.pParentTransform)
		{
			CTransform* pTransform = m_tAttachDesc.pParentTransform;
			_vector vDir = pTransform->Get_GroundLook();

			m_pTransform->Go_Dir(vDir, fTimeDelta, nullptr);

		}
	}
}

void CEffect_Mesh_Tool::Update_Speed()
{
	_float fSpeed = m_pTransform->Get_Speed();

	m_pTransform->Set_Speed(fSpeed);
}

void CEffect_Mesh_Tool::Update_Rotation(_float fTimeDelta)
{
	if (true == m_tMeshDesc.bLerpRotation)
	{
		if (0.f == m_tCommon_Desc.fTotalPlayTime)
			return;

		_float fRatio = m_fTimeAcc / m_tCommon_Desc.fTotalPlayTime;
		_vector vLerpedRotation = XMVectorLerp(XMLoadFloat3(&m_tMeshDesc.vStartRotation), XMLoadFloat3(&m_tMeshDesc.vRotationOverLife), fRatio);
		m_pTransform->Rotation_RollPitchYaw(To_Radian(vLerpedRotation.m128_f32[0]), To_Radian(vLerpedRotation.m128_f32[1]), To_Radian(vLerpedRotation.m128_f32[2]));
	}
	else
	{
		if (0.f == m_tMeshDesc.fTurnSpeed)
			return;

		_vector vAxis;
		if (m_tCommon_Desc.bRotationLocal)
			vAxis = XMVector3Normalize(
				m_pTransform->Get_Right() * m_tMeshDesc.vRotationAxis.x
				+ m_pTransform->Get_Up() * m_tMeshDesc.vRotationAxis.y
				+ m_pTransform->Get_Look() * m_tMeshDesc.vRotationAxis.z);
		else
			vAxis = XMVector3Normalize(
				XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_tMeshDesc.vRotationAxis.x
				+ XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_tMeshDesc.vRotationAxis.y
				+ (XMVectorSet(0.f, 0.f, 1.f, 0.f) * m_tMeshDesc.vRotationAxis.z));

		m_pTransform->Turn(vAxis, fTimeDelta);
	}
}

void CEffect_Mesh_Tool::Update_Scale()
{
	if (true == m_tCommon_Desc.bLerpSize)
	{
		if (0.f == m_tCommon_Desc.fTotalPlayTime)
			return;

		if (true == m_tMeshDesc.bSizePhaseSecond)
		{
			if (m_fTimeAcc <= m_tMeshDesc.fSecondPhaseTime)
			{
				_float fRatio = m_fTimeAcc / m_tMeshDesc.fSecondPhaseTime;
				_vector vLerpedScale = XMVectorLerp(XMLoadFloat3(&m_vStartSize), XMLoadFloat3(&m_tMeshDesc.vScaleOverLife), fRatio);

				_float3 vScale;
				XMStoreFloat3(&vScale, vLerpedScale);
				m_vSecondStartScale = vScale;
				m_pTransform->Set_Scale(vScale);
			}
			else
			{
				_float fRatio = (m_fTimeAcc - m_tMeshDesc.fSecondPhaseTime) / (m_tCommon_Desc.fTotalPlayTime - m_tMeshDesc.fSecondPhaseTime);
				_vector vLerpedScale = XMVectorLerp(XMLoadFloat3(&m_vSecondStartScale), XMLoadFloat3(&m_tMeshDesc.vScaleOverLife_Secod), fRatio);

				_float3 vScale;
				XMStoreFloat3(&vScale, vLerpedScale);

				m_pTransform->Set_Scale(vScale);
			}

		}
		else
		{
			_float fRatio = m_fTimeAcc / m_tCommon_Desc.fTotalPlayTime;
			_vector vLerpedScale = XMVectorLerp(XMLoadFloat3(&m_vStartSize), XMLoadFloat3(&m_tMeshDesc.vScaleOverLife), fRatio);

			_float3 vScale;
			XMStoreFloat3(&vScale, vLerpedScale);

			m_pTransform->Set_Scale(vScale);
		}


	}
}

void CEffect_Mesh_Tool::Update_Color()
{
	if (true == m_tCommon_Desc.bEnableColorOverLife)
	{
		if (0.f == m_tCommon_Desc.fTotalPlayTime)
			return;

		_float fRatio = m_fTimeAcc / m_tCommon_Desc.fTotalPlayTime;

		XMStoreFloat4(&m_vCurrentLerpColor, XMVectorLerp(XMLoadFloat4(&m_vStartColor), XMLoadFloat4(&m_tCommon_Desc.vColorOverLife), fRatio));
	}

}


void CEffect_Mesh_Tool::Update_FinalMatrix(_float fTimeDelta)
{
	__super::Attach_Update();

	_matrix EffectMat = m_pTransform->Get_WorldMatrix();
	_matrix ParentMat = XMLoadFloat4x4(&m_AttachWorldMat);
	if (true == m_tMeshDesc.bDiscardRotation)
	{
		ParentMat.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		ParentMat.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		ParentMat.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		XMStoreFloat4x4(&m_AttachWorldMat, ParentMat);
	}

	XMStoreFloat4x4(&m_FinalMatrix, EffectMat * ParentMat);


	if (true == m_tCommon_Desc.bProportionalAlphaToSpeed)
	{
		_float3 vCur;
		memcpy(&vCur, m_FinalMatrix.m[3], sizeof(_float3));
		_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPrevPos) - XMLoadFloat3(&vCur)));
		_float fSpeed = fDist / fTimeDelta;

		fSpeed = clamp(fSpeed, 0.f, m_tCommon_Desc.fProportionalAlphaMaxSpeed);

		if (true == m_tCommon_Desc.bEnableColorOverLife)
			m_vCurrentLerpColor.w = fSpeed / m_tCommon_Desc.fProportionalAlphaMaxSpeed;
		m_vPrevPos = vCur;
	}

}

HRESULT CEffect_Mesh_Tool::Ready_Components()
{
	// Transform.. 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	// Shader...
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_MeshEffect"), TEXT("Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh_Tool::Bind_ShaderResources()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_FinalMatrix)))
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
	if (m_tCommon_UV_Desc.iRadialMappingFlag & NOISERADIAL)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vNoiseRadiusFlag", &m_tCommon_UV_Desc.vNoise_RadiusFlag, sizeof(_float2))))
			return E_FAIL;
	}
	if (m_tCommon_UV_Desc.iRadialMappingFlag & DISTORTIONRADIAL)
	{
		if (FAILED(m_pShader->Bind_RawValue("g_vDistortionRadiusFlag", &m_tCommon_UV_Desc.vDistortion_RadiusFlag, sizeof(_float2))))
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


	if (FAILED(m_pShader->Bind_RawValue("g_bNoiseMultiplyAlpha", &m_tCommon_Desc.bNoiseMultiplyAlpha, sizeof(_bool))))
		return E_FAIL;
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
	if (FAILED(m_pShader->Bind_RawValue("g_iRadialMappingFlag", &m_tCommon_UV_Desc.iRadialMappingFlag, sizeof(_int))))
		return E_FAIL;

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
		if (FAILED(m_pShader->Bind_RawValue("g_bUseSpriteBase", &m_tCommon_Desc.bBaseSprite, sizeof(_bool))))
			return E_FAIL;

		if (true == m_tCommon_Desc.bBaseSprite)
		{
			_float fWidthUV = 1.f / m_tCommon_Desc.iBaseSpriteMaxCol;
			_float fHeightUV = 1.f / m_tCommon_Desc.iBaseSpriteMaxRow;
			if (FAILED(m_pShader->Bind_RawValue("g_iBaseCol", &m_tCommon_Desc.iBaseSpriteMaxCol, sizeof(int))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fColBaseUVInterval", &fWidthUV, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fRowBaseUVInterval", &fHeightUV, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_iBaseSpriteIndex", &m_iBaseSpriteIndex, sizeof(_int))))
				return E_FAIL;

		}
	}
	if (nullptr != m_pTexture[TEX_ALPHAMASK])
	{
		if (FAILED(m_pTexture[TEX_ALPHAMASK]->Set_SRV(m_pShader, "g_AlphaMask", 0)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vAlphaMaskUVOffset", &m_vAlphaMaskUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iAlphaMaskSampler", &m_tCommon_UV_Desc.iAlphaMaskSampler, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bUseSpriteMask", &m_tCommon_Desc.bMaskSprite, sizeof(_bool))))
			return E_FAIL;

		if (true == m_tCommon_Desc.bMaskSprite)
		{
			_float fWidthUV = 1.f / static_cast<_float>(m_tCommon_Desc.iMaskSpriteMaxCol);
			_float fHeightUV = 1.f / static_cast<_float>(m_tCommon_Desc.iMaskSpriteMaxRow);

			if (FAILED(m_pShader->Bind_RawValue("g_iMaskCol", &m_tCommon_Desc.iMaskSpriteMaxCol, sizeof(int))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fColMaskUVInterval", &fWidthUV, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_fRowMaskUVInterval", &fHeightUV, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_iMaskSpriteIndex", &m_iMaskSpriteIndex, sizeof(_int))))
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
		if (FAILED(m_pShader->Bind_RawValue("g_vDistortionUVOffset", &m_vDistortionUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iDistortionSampler", &m_tCommon_UV_Desc.iDistortionSampler, sizeof(_int))))
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
	if (nullptr != m_pTexture[TEX_UV_SLICE_CLIP])
	{
		if (FAILED(m_pTexture[TEX_UV_SLICE_CLIP]->Set_SRV(m_pShader, "g_UVSliceClipTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vUVSliceClipOffset", &m_vUVSliceClipUVOffset, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_iUVSliceClipSampler", &m_tCommon_UV_Desc.iUVSliceClipSampler, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_bUseGreaterThan_U", &m_tCommon_Desc.bUseGreaterThan_U, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bUseGreaterThan_V", &m_tCommon_Desc.bUseGreaterThan_V, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bUseLessthan_U", &m_tCommon_Desc.bUseLessThan_U, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_bUseLessthan_V", &m_tCommon_Desc.bUseLessThan_V, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_fGreaterCmpValue_U", &m_tCommon_Desc.fGreaterCmpVal_U, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fGreaterCmpValue_V", &m_tCommon_Desc.fGreaterCmpVal_V, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fLessCmpValue_U", &m_tCommon_Desc.fLessCmpVal_U, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fLessCmpValue_V", &m_tCommon_Desc.fLessCmpVal_V, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_fClipFactor", &m_tCommon_Desc.fClipFactor, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

CEffect_Mesh_Tool* CEffect_Mesh_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Mesh_Tool* pInstance = new CEffect_Mesh_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Mesh_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Mesh_Tool::Clone(void* pArg)
{
	CEffect_Mesh_Tool* pInstance = new CEffect_Mesh_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Mesh_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Mesh_Tool::Free()
{
	__super::Free();
	Safe_Release(m_pModel);


}

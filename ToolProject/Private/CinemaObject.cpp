#include "CinemaObject.h"

CCinemaObject::CCinemaObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCinemaObject::CCinemaObject(const CCinemaObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCinemaObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCinemaObject::Initialize(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
	return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

void CCinemaObject::Tick(_float fTimeDelta)
{
	if (true == m_bPlaying)
	{
		if (ScaleFrameIt != m_CinemaDesc.Scales.end())
		{
			TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(ScaleFrameIt);

			if (NextIter != m_CinemaDesc.Scales.end() && true == NextIter->second.second)
			{
				_float fRatio = (m_fPlayTimeAcc - ScaleFrameIt->first) / (NextIter->first - ScaleFrameIt->first);

				_vector vLerpedScale = XMVectorLerp(XMLoadFloat3(&ScaleFrameIt->second.first), XMLoadFloat3(&NextIter->second.first), fRatio);

				_float3 vScale;
				XMStoreFloat3(&vScale, vLerpedScale);
				m_pTransform->Set_Scale(vScale);
			}
			else
			{
				m_pTransform->Set_Scale(ScaleFrameIt->second.first);
			}
		}

		if (LookFrameIt != m_CinemaDesc.Looks.end())
		{
			TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(LookFrameIt);

			if (NextIter != m_CinemaDesc.Looks.end() && true == NextIter->second.second)
			{
				_float fRatio = (m_fPlayTimeAcc - LookFrameIt->first) / (NextIter->first - LookFrameIt->first);

				_vector vLerpedQuatByLook = JoMath::Slerp_TargetLook(XMLoadFloat3(&LookFrameIt->second.first), XMLoadFloat3(&NextIter->second.first), fRatio);

				m_pTransform->Rotation_Quaternion(vLerpedQuatByLook);
			}
			else
			{
				_vector vLook = XMLoadFloat3(&LookFrameIt->second.first);

				m_pTransform->LookTo(vLook);
			}
		}


		if (PositionFrameIt != m_CinemaDesc.Positions.end())
		{
			TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(PositionFrameIt);

			if (NextIter != m_CinemaDesc.Positions.end() && true == NextIter->second.second)
			{
				_float fRatio = (m_fPlayTimeAcc - PositionFrameIt->first) / (NextIter->first - PositionFrameIt->first);

				_vector vLerpedPosition = XMVectorLerp(XMLoadFloat3(&PositionFrameIt->second.first), XMLoadFloat3(&NextIter->second.first), fRatio);
				vLerpedPosition.m128_f32[3] = 1.f;

				m_pTransform->Set_Position(vLerpedPosition);
			}
			else
			{
				_vector vPosition = XMLoadFloat3(&PositionFrameIt->second.first);
				vPosition.m128_f32[3] = 1.f;

				m_pTransform->Set_Position(vPosition);
			}
		}
	}

}

void CCinemaObject::LateTick(_float fTimeDelta)
{
	if (m_pModel)
	{
		m_pModel->Play_Animation(fTimeDelta);
		m_pModel->Update_BoneMatrices();
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CCinemaObject::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture");

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

void CCinemaObject::Update_PlayTime(_float fPlayTime)
{
	for (ScaleFrameIt = m_CinemaDesc.Scales.begin(); ScaleFrameIt != m_CinemaDesc.Scales.end(); ++ScaleFrameIt)
	{
		_float fKeyPlayTime = ScaleFrameIt->first;
		if (fKeyPlayTime < fPlayTime)
		{
			if (ScaleFrameIt != m_CinemaDesc.Scales.begin())
				--ScaleFrameIt;
			break;
		}
	}

	if (m_CinemaDesc.Scales.end() == ScaleFrameIt && m_CinemaDesc.Scales.begin() != ScaleFrameIt)
		ScaleFrameIt = std::prev(ScaleFrameIt);


	for (LookFrameIt = m_CinemaDesc.Looks.begin(); LookFrameIt != m_CinemaDesc.Looks.end(); ++LookFrameIt)
	{
		_float fKeyPlayTime = LookFrameIt->first;
		if (fPlayTime < fKeyPlayTime)
		{
			if (LookFrameIt != m_CinemaDesc.Looks.begin())
				--LookFrameIt;
			break;
		}
	}

	if (m_CinemaDesc.Looks.end() == LookFrameIt && m_CinemaDesc.Looks.begin() != LookFrameIt)
		LookFrameIt = std::prev(LookFrameIt);


	for (PositionFrameIt = m_CinemaDesc.Positions.begin(); PositionFrameIt != m_CinemaDesc.Positions.end(); ++PositionFrameIt)
	{
		_float fKeyPlayTime = PositionFrameIt->first;
		if (fPlayTime < fKeyPlayTime)
		{
			if (PositionFrameIt != m_CinemaDesc.Positions.begin())
				--PositionFrameIt;
			break;
		}
	}

	if (m_CinemaDesc.Positions.end() == PositionFrameIt && m_CinemaDesc.Positions.begin() != PositionFrameIt)
		PositionFrameIt = std::prev(PositionFrameIt);


	for (EventFrameIt = m_CinemaDesc.Events.begin(); EventFrameIt != m_CinemaDesc.Events.end(); ++EventFrameIt)
	{
		_float fKeyPlayTime = EventFrameIt->first;
		if (fPlayTime > fKeyPlayTime)
		{
			size_t iBit = m_CinemaDesc.EventPlayed & ((size_t)1 << m_iEventIdx);

			if (m_fPlayTimeAcc >= EventFrameIt->first && !iBit)
			{
				m_CinemaDesc.EventPlayed |= ((size_t)1 << m_iEventIdx);

				if (nullptr != EventFrameIt->second.pFunc)
					EventFrameIt->second.pFunc();
			}
		}

		m_iEventIdx = (m_iEventIdx + 1) % m_CinemaDesc.Events.size();
	}


	m_fPlayTimeAcc = fPlayTime;
}

void CCinemaObject::Change_Model(CModel* pModel)
{
	Safe_Release(m_pModel);

	m_pModel = pModel;
	m_pModel->Change_Animation(0);
	m_pModel->Set_AnimPause();

	m_EffectSpawnDesc.pParentModel = m_pModel;
	m_EffectSpawnDesc.pParentTransform = m_pTransform;

}

CCinemaObject* CCinemaObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinemaObject* pInstance = new CCinemaObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolAnimObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinemaObject::Clone(void* pArg)
{
	CCinemaObject* pInstance = new CCinemaObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolAnimObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinemaObject::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

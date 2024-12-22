#include "..\Public\GameObject.h"
#include "GameInstance.h"




CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{rhs.m_pContext }
	, m_pGameInstance{ rhs.m_pGameInstance }
	, m_iTag {rhs.m_iTag}
	, m_bDestroyed { false }
	, m_fDissolveAmount { rhs.m_fDissolveAmount }
	, m_fDissolveSpeed { rhs.m_fDissolveSpeed }
	, m_iPassIdx{rhs.m_iPassIdx}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::OnActive()
{
}

void CGameObject::OnInActive()
{
}

HRESULT CGameObject::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;

	return S_OK;
}

void CGameObject::OnExit_Layer()
{
	m_bInLayer = false;
}

void CGameObject::PriorityTick(_float fTimeDelta)
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Render_Picking()
{
	return S_OK;
}

HRESULT CGameObject::Render_Cascade()
{
	return S_OK;
}

HRESULT CGameObject::Render_ShadowDepth()
{
	return S_OK;
}

_vector CGameObject::Get_Center() const
{
	return m_pTransform->Get_Position();
}

_vector CGameObject::Get_TopPos() const
{
	return m_pTransform->Get_Position();
}

_bool CGameObject::Bind_CinematicFunc(CINEMATIC_DESCS& CinematicDescs)
{
	return true;
}

void CGameObject::Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs)
{
	m_EventChecked = 0;

	m_CinematicDescs = CinematicDescs;

	m_ScaleFrameIt = m_CinematicDescs.Scales.begin();
	m_LookFrameIt = m_CinematicDescs.Looks.begin();
	m_PositionFrameIt = m_CinematicDescs.Positions.begin();
	m_EventFrameIt = m_CinematicDescs.Events.begin();

}

void CGameObject::End_Cinematic()
{
}

void CGameObject::Play_Cinematic(_float fPlayTime)
{
	if (m_ScaleFrameIt != m_CinematicDescs.Scales.end())
	{
		CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(m_ScaleFrameIt);

		if (NextIter != m_CinematicDescs.Scales.end() && true == NextIter->second.second)
		{
			_float fRatio = (fPlayTime - m_ScaleFrameIt->first) / (NextIter->first - m_ScaleFrameIt->first);

			_vector vLerpedScale = XMVectorLerp(XMLoadFloat3(&m_ScaleFrameIt->second.first), XMLoadFloat3(&NextIter->second.first), fRatio);

			_float3 vScale;
			XMStoreFloat3(&vScale, vLerpedScale);
			m_pTransform->Set_Scale(vScale);
		}
		else
		{
			m_pTransform->Set_Scale(m_ScaleFrameIt->second.first);
		}

		
		if ((NextIter != m_CinematicDescs.Scales.end() && NextIter->first <= fPlayTime) || NextIter == m_CinematicDescs.Scales.end())
		{
			m_ScaleFrameIt = NextIter;
		}
		
	}


	if (m_LookFrameIt != m_CinematicDescs.Looks.end())
	{
		CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(m_LookFrameIt);

		if (NextIter != m_CinematicDescs.Looks.end() && true == NextIter->second.second)
		{
			_float fRatio = (fPlayTime - m_LookFrameIt->first) / (NextIter->first - m_LookFrameIt->first);

			m_pGameInstance->Get_CalculateTF()->Set_WorldMatrix(XMMatrixIdentity());
			m_pGameInstance->Get_CalculateTF()->LookTo(XMLoadFloat3(&m_LookFrameIt->second.first));
			_vector StartQuat = XMQuaternionRotationMatrix(m_pGameInstance->Get_CalculateTF()->Get_WorldMatrix());

			m_pGameInstance->Get_CalculateTF()->LookTo(XMLoadFloat3(&NextIter->second.first));
			_vector EndQuat = XMQuaternionRotationMatrix(m_pGameInstance->Get_CalculateTF()->Get_WorldMatrix());
	
			_vector vLerpedQuatByLook = XMQuaternionSlerp(StartQuat, EndQuat, fRatio);
				
			m_pTransform->Rotation_Quaternion(vLerpedQuatByLook);
		}
		else
		{
			_vector vLook = XMLoadFloat3(&m_LookFrameIt->second.first);

			m_pTransform->LookTo(vLook);
		}


		if ((NextIter != m_CinematicDescs.Looks.end() && NextIter->first <= fPlayTime) || NextIter == m_CinematicDescs.Looks.end())
		{
			m_LookFrameIt = NextIter;
		}
	}


	if (m_PositionFrameIt != m_CinematicDescs.Positions.end())
	{
		CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(m_PositionFrameIt);

		if (NextIter != m_CinematicDescs.Positions.end() && true == NextIter->second.second)
		{
			_float fRatio = (fPlayTime - m_PositionFrameIt->first) / (NextIter->first - m_PositionFrameIt->first);

			_vector vLerpedPosition = XMVectorLerp(XMLoadFloat3(&m_PositionFrameIt->second.first), XMLoadFloat3(&NextIter->second.first), fRatio);
			vLerpedPosition.m128_f32[3] = 1.f;

			m_pTransform->Set_Position(vLerpedPosition);
		}
		else
		{
			_vector vPosition = XMLoadFloat3(&m_PositionFrameIt->second.first);
			vPosition.m128_f32[3] = 1.f;

			m_pTransform->Set_Position(vPosition);
		}

		if ((NextIter != m_CinematicDescs.Positions.end() && NextIter->first <= fPlayTime) || NextIter == m_CinematicDescs.Positions.end())
		{
			m_PositionFrameIt = NextIter;
		}
	}

	if (m_EventFrameIt != m_CinematicDescs.Events.end())
	{
		_float fKeyPlayTime = m_EventFrameIt->first;
		if (fPlayTime > fKeyPlayTime)
		{
			size_t iBit = m_EventChecked & ((size_t)1 << m_iCinematicEventIdx);

			while (m_EventFrameIt != m_CinematicDescs.Events.end() && !iBit && fPlayTime >= m_EventFrameIt->first)
			{
				m_EventChecked |= ((size_t)1 << m_iCinematicEventIdx);

				if (nullptr != m_EventFrameIt->second.pFunc)
					m_EventFrameIt->second.pFunc();

				m_iCinematicEventIdx = m_iCinematicEventIdx + 1;
				++m_EventFrameIt;

				iBit = m_EventChecked & ((size_t)1 << m_iCinematicEventIdx);
			}
		}
	}
}


HRESULT CGameObject::Bind_ShaderResources()
{
	return S_OK;
}

void CGameObject::Decide_PassIdx()
{

}

void CGameObject::Active_Dissolve()
{
	m_bDissolve = true;
	m_fDissolveAmount = m_fDissolveSpeed < 0.f ? 1.f : 0.f;

	Decide_PassIdx();
}

void CGameObject::Update_Dissolve(_float fTimeDelta)
{
	m_fDissolveAmount = clamp(m_fDissolveAmount + m_fDissolveSpeed * fTimeDelta, 0.f, 1.f);

	if (0.f == m_fDissolveAmount || 1.f == m_fDissolveAmount)
	{
		m_bDissolve = false;
		//Decide_PassIdx();
	}
}

HRESULT CGameObject::Bind_DissolveDescs()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CComponent* pComponent = m_pGameInstance->Clone_Component(iPrototoypeLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	if (nullptr != ppOut)
	{
		*ppOut = pComponent;
		Safe_AddRef(pComponent);
	}

	return S_OK;
}

HRESULT CGameObject::Erase_Component(const wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return E_FAIL;

	Safe_Release(iter->second);

	m_Components.erase(iter);
		
	return S_OK;
}

CComponent* CGameObject::Find_Component(const wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}


void CGameObject::Set_Active(_bool b)
{
	m_bActive = b;
	if (m_bActive)
		OnActive();
	else
		OnInActive();
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransform);
	Safe_Release(m_pDissolveTexture);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

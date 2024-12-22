#include "Cinema_Travis_Bike.h"
#include "Cinematic_Manager.h"

#include "Travis_Bike_Enums.h"

#include "Bone.h"
#include "Cinema_Travis_World.h"

#include "Effect_Manager.h"



CCinema_Travis_Bike::CCinema_Travis_Bike(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor(_pDevice, _pContext)
{
}

CCinema_Travis_Bike::CCinema_Travis_Bike(const CCinema_Travis_Bike& _rhs)
	: CCinemaActor(_rhs)
{
}


HRESULT CCinema_Travis_Bike::Initialize(void* _pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Model_Travis_Bike", TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	__super::Initialize(_pArg);

	if (FAILED(Ready_TireBones()))
		return E_FAIL;

	m_strCinemaActorTag = "Travis_Bike";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Travis_Bike::Tick(_float _fTimeDelta)
{
	if (m_bDisappear)
		return;

	__super::Tick(_fTimeDelta);

	if (false == m_bGetOff)
	{
		Rotate_Tires(_fTimeDelta);
		m_pTravis->Get_Transform()->Attach_To_Bone(m_pModel->Get_Bone("pirot_attach"), m_pTransform, XMMatrixRotationY(XM_PI));
		if (ANI_PL_Vehicle_IDL_Neutral_Victory == m_pModel->Get_CurrentAnimIndex() && m_pModel->Is_AnimComplete())
			Get_Off();
	}

}


HRESULT CCinema_Travis_Bike::Ready_TireBones()
{
	m_pBone_Tire_R = m_pModel->Get_Bone("rtire_r");
	if (nullptr == m_pBone_Tire_R)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_R);

	m_pBone_Tire_L = m_pModel->Get_Bone("rtire_l");
	if (nullptr == m_pBone_Tire_L)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_L);

	m_pBone_Tire_F = m_pModel->Get_Bone("fronttire");
	if (nullptr == m_pBone_Tire_F)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_F);

	return S_OK;
}

_bool CCinema_Travis_Bike::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CModel::Change_Animation, m_pModel, ANI_PL_Vehicle_IDL_Neutral_Victory, 0.1f, false);

	if (false == CINEMAMGR->Bind_ActorEvent("SonicJuice_Cinematic", m_strCinemaActorTag, 1.f, CineEvent))
		return false;

	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CCinema_Travis_Bike::Bike_Disappear, this);

	if (false == CINEMAMGR->Bind_ActorEvent("SonicJuice_Cinematic", m_strCinemaActorTag, 4.8f, CineEvent))
		return false;


	return true;
}

void CCinema_Travis_Bike::Rotate_Tires(_float fTimeDelta)
{
	m_fTireRotation += 360.f * fTimeDelta;
	if (m_fTireRotation >= 360.f)
		m_fTireRotation = 0.f;

	m_pBone_Tire_R->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation)));
	m_pBone_Tire_L->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation)));
	m_pBone_Tire_F->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation)));
}

void CCinema_Travis_Bike::Get_Off()
{
	m_bGetOff = true;
	ADD_EVENT(bind(&CCinema_Travis_World::Get_Off, m_pTravis));
	m_pTravis = nullptr;
}

void CCinema_Travis_Bike::Bike_Disappear()
{
	EFFECTMGR->Active_Effect(LEVEL_STATIC, "Bike_Disappear", Get_EffectDescPtr());
	m_bDisappear = true;
}

CCinema_Travis_Bike* CCinema_Travis_Bike::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Travis_Bike* pInstance = new CCinema_Travis_Bike(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Travis_Bike");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Travis_Bike::Clone(void* _pArg)
{
	CCinema_Travis_Bike* pInstance = new CCinema_Travis_Bike(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Travis_Bike"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Travis_Bike::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);

	Safe_Release(m_pTravis);

	Safe_Release(m_pBone_Tire_F);
	Safe_Release(m_pBone_Tire_L);
	Safe_Release(m_pBone_Tire_R);
}



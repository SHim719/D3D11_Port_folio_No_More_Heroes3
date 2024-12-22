#include "Cinema_Damon.h"

#include "Cinematic_Manager.h"
#include "SmashBros_Damon_Enums.h"

#include "Effect_Manager.h"

#pragma region Basic Function
CCinema_Damon::CCinema_Damon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor{ _pDevice, _pContext }
{
}

CCinema_Damon::CCinema_Damon(const CCinema_Damon& _rhs)
	: CCinemaActor(_rhs)
{
}
#pragma endregion

HRESULT CCinema_Damon::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_BATTLE_MOTEL_FRONT, TEXT("Prototype_Model_SmashBros_Damon"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "Damon";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Damon::Tick(_float _fTimeDelta)
{
	if (m_bDisappear == true)
		return;

	__super::Tick(_fTimeDelta);
}

_bool CCinema_Damon::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CModel::Change_Animation, m_pModel, (_uint)DAMON_ANIMLIST::ANI_CH_Demon_IDL_Appear_01, 0.1f, false);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 36.f, CineEvent))
		return false;

	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CCinema_Damon::Damon_Disappear, this);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 38.5f, CineEvent))
		return false;

	return true;
}

void CCinema_Damon::Damon_Disappear()
{
	EFFECTMGR->Active_Effect(LEVEL_STATIC, "Monster_WarpEffect", Get_EffectDescPtr());
	m_bDisappear = true;
}

void CCinema_Damon::Get_Off()
{
}

CCinema_Damon* CCinema_Damon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Damon* pInstance = new CCinema_Damon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Damon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Damon::Clone(void* _pArg)
{
	CCinema_Damon* pInstance = new CCinema_Damon(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Damon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Damon::Free()
{
	__super::Free();
}

#include "Cinema_Jeane.h"

#include "Cinematic_Manager.h"
#include "Jeane_Enums.h"

#include "Effect_Manager.h"

#pragma region Basic Function
CCinema_Jeane::CCinema_Jeane(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor{ _pDevice, _pContext }
{
}

CCinema_Jeane::CCinema_Jeane(const CCinema_Jeane& _rhs)
	: CCinemaActor(_rhs)
{
}
#pragma endregion

HRESULT CCinema_Jeane::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_BATTLE_MOTEL_FRONT, TEXT("Prototype_Model_NPC_Jeane"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "NPC_Jeane";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Jeane::Tick(_float _fTimeDelta)
{
	if (m_bDisappear == true)
		return;

	__super::Tick(_fTimeDelta);
}

HRESULT CCinema_Jeane::Render()
{
	__super::Render();

	return S_OK;
}

_bool CCinema_Jeane::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CCinema_Jeane::Jeane_Disappear, this);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 35.5f, CineEvent))
		return false;

	return true;
}

void CCinema_Jeane::Jeane_Disappear()
{
	EFFECTMGR->Active_Effect(LEVEL_STATIC, "Monster_WarpEffect", Get_EffectDescPtr());
	m_bDisappear = true;
}

void CCinema_Jeane::Get_Off()
{
}

CCinema_Jeane* CCinema_Jeane::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Jeane* pInstance = new CCinema_Jeane(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Jeane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Jeane::Clone(void* _pArg)
{
	CCinema_Jeane* pInstance = new CCinema_Jeane(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Jeane"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Jeane::Free()
{
	__super::Free();
}

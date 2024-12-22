#include "Cinema_Mbone.h"

#include "Cinematic_Manager.h"
#include "Mbone_Enums.h"

#include "Effect_Manager.h"

#pragma region Basic Function
CCinema_Mbone::CCinema_Mbone(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor{ _pDevice, _pContext }
{
}

CCinema_Mbone::CCinema_Mbone(const CCinema_Mbone& _rhs)
	: CCinemaActor(_rhs)
{
}
#pragma endregion

HRESULT CCinema_Mbone::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_BATTLE_MOTEL_FRONT, TEXT("Prototype_Model_Mbone"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "Mbone";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Mbone::Tick(_float _fTimeDelta)
{
	if (m_bDisappear == true)
		return;

	__super::Tick(_fTimeDelta);
}

_bool CCinema_Mbone::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CCinema_Mbone::Mbone_Disappear, this);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 33.5f, CineEvent))
		return false;

	return true;
}

void CCinema_Mbone::Mbone_Disappear()
{
	EFFECTMGR->Active_Effect(LEVEL_STATIC, "Monster_WarpEffect", Get_EffectDescPtr());
	m_bDisappear = true;
}

void CCinema_Mbone::Get_Off()
{
}

CCinema_Mbone* CCinema_Mbone::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Mbone* pInstance = new CCinema_Mbone(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Mbone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Mbone::Clone(void* _pArg)
{
	CCinema_Mbone* pInstance = new CCinema_Mbone(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Mbone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Mbone::Free()
{
	__super::Free();
}

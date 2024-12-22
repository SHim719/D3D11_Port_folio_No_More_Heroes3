#include "Cinema_Treatment.h"

#include "Cinematic_Manager.h"
#include "Treatment_Enums.h"

#include "Effect_Manager.h"

#pragma region Basic Function
CCinema_Treatment::CCinema_Treatment(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor{ _pDevice, _pContext }
{
}

CCinema_Treatment::CCinema_Treatment(const CCinema_Treatment& _rhs)
	: CCinemaActor(_rhs)
{
}
#pragma endregion

HRESULT CCinema_Treatment::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_BATTLE_MOTEL_FRONT, TEXT("Prototype_Model_Treatment"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "Treatment";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Treatment::Tick(_float _fTimeDelta)
{
	if (m_bDisappear == true)
		return;

	__super::Tick(_fTimeDelta);
}

_bool CCinema_Treatment::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CCinema_Treatment::Treatment_Disappear, this);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 34.5f, CineEvent))
		return false;

	return true;
}

void CCinema_Treatment::Treatment_Disappear()
{
	EFFECTMGR->Active_Effect(LEVEL_STATIC, "Monster_WarpEffect", Get_EffectDescPtr());
	m_bDisappear = true;
}

void CCinema_Treatment::Get_Off()
{
}

CCinema_Treatment* CCinema_Treatment::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Treatment* pInstance = new CCinema_Treatment(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Treatment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Treatment::Clone(void* _pArg)
{
	CCinema_Treatment* pInstance = new CCinema_Treatment(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Treatment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Treatment::Free()
{
	__super::Free();
}

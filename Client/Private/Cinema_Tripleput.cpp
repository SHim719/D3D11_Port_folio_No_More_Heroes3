#include "Cinema_Tripleput.h"

#include "Cinematic_Manager.h"
#include "Tripleput_Enums.h"

#include "Effect_Manager.h"

#pragma region Basic Function
CCinema_Tripleput::CCinema_Tripleput(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor{ _pDevice, _pContext }
{
}

CCinema_Tripleput::CCinema_Tripleput(const CCinema_Tripleput& _rhs)
	: CCinemaActor(_rhs)
{
}
#pragma endregion

HRESULT CCinema_Tripleput::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_BATTLE_MOTEL_FRONT, TEXT("Prototype_Model_Tripleput"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "Tripleput";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Tripleput::Tick(_float _fTimeDelta)
{
	if (m_bDisappear == true)
		return;

	__super::Tick(_fTimeDelta);
}

_bool CCinema_Tripleput::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CCinema_Tripleput::Tripleput_Disappear, this);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 34.f, CineEvent))
		return false;

	return true;
}

void CCinema_Tripleput::Tripleput_Disappear()
{
	EFFECTMGR->Active_Effect(LEVEL_STATIC, "Monster_WarpEffect", Get_EffectDescPtr());
	m_bDisappear = true;
}

void CCinema_Tripleput::Get_Off()
{
}

CCinema_Tripleput* CCinema_Tripleput::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Tripleput* pInstance = new CCinema_Tripleput(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Tripleput");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Tripleput::Clone(void* _pArg)
{
	CCinema_Tripleput* pInstance = new CCinema_Tripleput(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Tripleput"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Tripleput::Free()
{
	__super::Free();
}
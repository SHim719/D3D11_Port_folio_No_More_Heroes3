#include "Cinema_Fullface.h"

#include "Cinematic_Manager.h"
#include "Fullface_Enums.h"

#include "Effect_Manager.h"

#pragma region Basic Function
CCinema_Fullface::CCinema_Fullface(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor{ _pDevice, _pContext }
{
}

CCinema_Fullface::CCinema_Fullface(const CCinema_Fullface& _rhs)
	: CCinemaActor(_rhs)
{
}
#pragma endregion

HRESULT CCinema_Fullface::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_BATTLE_MOTEL_FRONT, TEXT("Prototype_Model_Fullface"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "Fullface";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Fullface::Tick(_float _fTimeDelta)
{
	if (m_bDisappear == true)
		return;

	__super::Tick(_fTimeDelta);
}

_bool CCinema_Fullface::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CCinema_Fullface::Fullface_Disappear, this);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 35.f, CineEvent))
		return false;

	return true;
}

void CCinema_Fullface::Fullface_Disappear()
{
	EFFECTMGR->Active_Effect(LEVEL_STATIC, "Monster_WarpEffect", Get_EffectDescPtr());
	m_bDisappear = true;
}

void CCinema_Fullface::Get_Off()
{
}

CCinema_Fullface* CCinema_Fullface::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Fullface* pInstance = new CCinema_Fullface(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Fullface");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Fullface::Clone(void* _pArg)
{
	CCinema_Fullface* pInstance = new CCinema_Fullface(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Fullface"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Fullface::Free()
{
	__super::Free();
}

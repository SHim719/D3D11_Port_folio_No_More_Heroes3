#include "Cinema_Travis_Motel.h"

#include "Cinematic_Manager.h"
#include "Travis_Enums.h"

#include "Effect_Manager.h"

#pragma region Basic Function
CCinema_Travis_Motel::CCinema_Travis_Motel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor{ _pDevice, _pContext }
{
}

CCinema_Travis_Motel::CCinema_Travis_Motel(const CCinema_Travis_Motel& _rhs)
	: CCinemaActor(_rhs)
{
}
#pragma endregion


HRESULT CCinema_Travis_Motel::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Travis"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "Travis_Motel";
	CINEMAMGR->Add_CinemaObject(this);

	if (false == Bind_CinemaEtcFunc())
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Travis_Motel::Tick(_float _fTimeDelta)
{
	if (m_bDisappear == true)
		return;

	__super::Tick(_fTimeDelta);

	if (m_pModel->Is_AnimComplete() == true) {
		if (m_pModel->Get_CurrentAnimIndex() == (_uint)TRAVIS_ANIMLIST::ANI_PL_Travis_MOV_Walk) {
			m_pModel->Change_Animation((_uint)TRAVIS_ANIMLIST::ANI_PL_Travis_MOV_Walk_Out, 0.1f, false);
		}
		else if (m_pModel->Get_CurrentAnimIndex() == (_uint)TRAVIS_ANIMLIST::ANI_PL_Travis_MOV_Walk_Out) {
			m_pModel->Change_Animation((_uint)TRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Neutral);
		}
	}
	else if (m_pModel->Is_AnimComplete() == false)
		m_pTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

_bool CCinema_Travis_Motel::Bind_CinemaEtcFunc()
{
	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CModel::Change_Animation, m_pModel, (_uint)TRAVIS_ANIMLIST::ANI_PL_Travis_MOV_Walk, 0.1f, false);

	if (false == CINEMAMGR->Bind_ActorEvent("Motel_Front", m_strCinemaActorTag, 8.2f, CineEvent))
		return false;

	return true;
}

void CCinema_Travis_Motel::Get_Off()
{
}

CCinema_Travis_Motel* CCinema_Travis_Motel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Travis_Motel* pInstance = new CCinema_Travis_Motel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Travis_Motel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Travis_Motel::Clone(void* _pArg)
{
	CCinema_Travis_Motel* pInstance = new CCinema_Travis_Motel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Travis_Motel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Travis_Motel::Free()
{
	__super::Free();
}

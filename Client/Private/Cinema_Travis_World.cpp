#include "Cinema_Travis_World.h"
#include "Cinematic_Manager.h"
#include "Travis_World_Enums.h"



CCinema_Travis_World::CCinema_Travis_World(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCinemaActor(_pDevice, _pContext)
{
}

CCinema_Travis_World::CCinema_Travis_World(const CCinema_Travis_World& _rhs)
	: CCinemaActor(_rhs)
{
}


HRESULT CCinema_Travis_World::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Travis_World"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_strCinemaActorTag = "Travis_World";
	CINEMAMGR->Add_CinemaObject(this);


	m_pModel->Set_AnimPlay();

	return S_OK;
}

void CCinema_Travis_World::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	if ((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral_R_Out == m_pModel->Get_CurrentAnimIndex())
	{
		if (m_pModel->Is_AnimComplete())
		{
			m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_NonBattle_Neutral);
			ADD_EVENT(bind(&CTransform::Turn_Quaternion, m_pTransform, m_pModel->Get_NowRootQuat(), XMMatrixIdentity()));
		}
		else
		{
			m_pTransform->Move_Root(m_pModel->Get_DeltaRootPos());
		}
	}
}

void CCinema_Travis_World::Get_Off()
{
	m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral_R_Out, false);
}


CCinema_Travis_World* CCinema_Travis_World::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinema_Travis_World* pInstance = new CCinema_Travis_World(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCinema_Travis_World");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Travis_World::Clone(void* _pArg)
{
	CCinema_Travis_World* pInstance = new CCinema_Travis_World(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Travis_World"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Travis_World::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

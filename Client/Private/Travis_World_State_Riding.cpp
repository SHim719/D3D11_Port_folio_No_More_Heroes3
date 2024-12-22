#include "Travis_World_State_Riding.h"

#include "GameInstance.h"

#include "Travis_World.h"
#include "Travis_Bike.h"

CTravis_World_State_Riding::CTravis_World_State_Riding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTravis_World_State_Base{ _pDevice,_pContext }
{
}

void CTravis_World_State_Riding::OnState_Start(void* _pArg)
{
	m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral);
	m_eCurDir = NEUTRAL;
}

void CTravis_World_State_Riding::Update(_float _fTimeDelta)
{
	CHECK_CONTROL_LOCK;

	if (KEY_PUSHING(eKeyCode::A) && LEFT != m_eCurDir)
	{
		//m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral_AKIRA, 0.1f, false);
		m_eCurDir = LEFT;
	}

	if (KEY_PUSHING(eKeyCode::D) && RIGHT != m_eCurDir)
	{
		//m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral_AKIRA_R, 0.1f, false);
		m_eCurDir = RIGHT;
	}

	if (KEY_NONE(eKeyCode::A) && KEY_NONE(eKeyCode::D) && (LEFT == m_eCurDir || RIGHT == m_eCurDir))
	{
		//m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral);
		m_eCurDir = NEUTRAL;
	}

	if (KEY_DOWN(eKeyCode::Space))
	{

	}
}


CState_Base* CTravis_World_State_Riding::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CState_Base* pInstance = new CTravis_World_State_Riding(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg))) {
		MSG_BOX(L"Failed to Create : CTravis_World_State_Riding");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_World_State_Riding::Free()
{
	__super::Free();
}

#include "Travis_World_State_Riding_Out.h"

#include "GameInstance.h"

#include "Travis_World.h"
#include "Travis_Bike.h"
#include "UI_Manager.h"
#include "UI_World_City.h"

CTravis_World_State_Riding_Out::CTravis_World_State_Riding_Out(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTravis_World_State_Base{ _pDevice,_pContext }
{
}

void CTravis_World_State_Riding_Out::OnState_Start(void* _pArg)
{
	m_pOwner->Set_AdjustNaviY(false);

	m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral_R_Out);
}

void CTravis_World_State_Riding_Out::OnState_End()
{
	m_pOwner->Set_AdjustNaviY(true);
}

void CTravis_World_State_Riding_Out::Update(_float _fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void CTravis_World_State_Riding_Out::Late_Update(_float _fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		ADD_EVENT(bind(&CTravis_World::GetOff_Bike_End, m_pOwner));
	
}

CState_Base* CTravis_World_State_Riding_Out::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CState_Base* pInstance = new CTravis_World_State_Riding_Out(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg))) {
		MSG_BOX(L"Failed to Create : CTravis_World_State_Riding_Out");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_World_State_Riding_Out::Free()
{
	__super::Free();

}

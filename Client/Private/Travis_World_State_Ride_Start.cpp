#include "Travis_World_State_Ride_Start.h"

#include "GameInstance.h"

#include "Travis_World.h"
#include "Travis_Bike.h"
#include "UI_Manager.h"
#include "UI_World_City.h"

CTravis_World_State_Ride_Start::CTravis_World_State_Ride_Start(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTravis_World_State_Base{ _pDevice,_pContext }
{
}

void CTravis_World_State_Ride_Start::OnState_Start(void* _pArg)
{
	CTravis_Bike* pBike = (CTravis_Bike*)_pArg;
	CTransform* pBikeTransform = pBike->Get_Transform();

	m_pTravis_Bike = pBike;
	Safe_AddRef(m_pTravis_Bike);

	m_pOwner->Set_AdjustNaviY(false);

	_vector vRidePosition = pBikeTransform->Get_Position() + pBikeTransform->Get_GroundRight() * 1.5f - pBikeTransform->Get_GroundLook() * 0.4f;
	m_pOwnerTransform->Set_Position(vRidePosition);
	m_pOwnerTransform->LookTo(-pBikeTransform->Get_GroundRight());

	static_cast<CCollider*>(m_pOwner->Find_Component(L"HitBox"))->Set_Active(false);
	pBike->Set_Active_Colliders_By_RidingState(true);

	m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral_R_In);

	static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->Set_RideUI();
	static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->Set_BikeUI(false);
}

void CTravis_World_State_Ride_Start::Update(_float _fTimeDelta)
{
	m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos());
}

void CTravis_World_State_Ride_Start::Late_Update(_float _fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		ADD_EVENT(bind(&CCharacter::Change_State, m_pOwner, (_uint)WORLDTRAVIS_STATES::STATE_RIDING, nullptr));
		ADD_EVENT(bind(&CTransform::Turn_Quaternion, m_pOwnerTransform, m_pModel->Get_NowRootQuat(), XMMatrixIdentity()));
		m_pTravis_Bike->Change_State((_uint)Travis_Bike_States::State_Ride);
		Safe_Release(m_pTravis_Bike);
	}
		
}


CState_Base* CTravis_World_State_Ride_Start::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CState_Base* pInstance = new CTravis_World_State_Ride_Start(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg))) {
		MSG_BOX(L"Failed to Create : CTravis_World_State_Ride_Start");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_World_State_Ride_Start::Free()
{
	__super::Free();

	Safe_Release(m_pTravis_Bike);
}

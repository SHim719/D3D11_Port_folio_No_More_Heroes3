#include "Travis_Bike_State_NonRide.h"

CTravis_Bike_State_NonRide::CTravis_Bike_State_NonRide(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTravis_Bike_State_Base(_pDevice, _pContext)
{
}

HRESULT CTravis_Bike_State_NonRide::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CTravis_Bike_State_NonRide::OnState_Start(void* _pArg)
{
	m_pModel->Change_Animation(ANI_PL_Vehicle_IDL_Neutral);
}

void CTravis_Bike_State_NonRide::Update(_float _fTimeDelta)
{
}

void CTravis_Bike_State_NonRide::Late_Update(_float _fTimeDelta)
{
}

void CTravis_Bike_State_NonRide::OnState_End()
{
}

CTravis_Bike_State_NonRide* CTravis_Bike_State_NonRide::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	CTravis_Bike_State_NonRide* pInstance = new CTravis_Bike_State_NonRide(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX(L"Failed to Create : CTravis_Bike_State_NonRide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_Bike_State_NonRide::Free()
{
	__super::Free();
}

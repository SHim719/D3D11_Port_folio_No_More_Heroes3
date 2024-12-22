#include "Travis_Bike_State_Base.h"

CTravis_Bike_State_Base::CTravis_Bike_State_Base(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CState_Base(_pDevice, _pContext)
{
}

HRESULT CTravis_Bike_State_Base::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_pOwner = static_cast<CTravis_Bike*>(_pArg);

	return S_OK;
}

void CTravis_Bike_State_Base::OnState_Start(void* _pArg)
{
}

void CTravis_Bike_State_Base::Update(_float _fTimeDelta)
{
}

void CTravis_Bike_State_Base::Late_Update(_float _fTimeDelta)
{
}

void CTravis_Bike_State_Base::OnState_End()
{
}

void CTravis_Bike_State_Base::OnHit(const ATTACKDESC& _tagAtkDesc)
{
}

void CTravis_Bike_State_Base::Key_Input(_float _fTimeDelta)
{
}

void CTravis_Bike_State_Base::Free()
{
	__super::Free();
}

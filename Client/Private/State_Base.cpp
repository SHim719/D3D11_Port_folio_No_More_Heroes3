#include "State_Base.h"

#include "GameObject.h"

CState_Base::CState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CState_Base::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CGameObject* pOwner = (CGameObject*)pArg;

	m_pModel = static_cast<CModel*>(pOwner->Find_Component(L"Model"));
	if (nullptr == m_pModel)
		return E_FAIL;

	m_pOwnerTransform = pOwner->Get_Transform();
	if (nullptr == m_pOwnerTransform)
		return E_FAIL;

	m_pNavigation = static_cast<CNavigation*>(pOwner->Find_Component(L"Navigation"));

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixRotationX(-XM_PIDIV2));

	return S_OK;
}

void CState_Base::OnState_Start(void* pArg)
{

}

void CState_Base::Priority_Update(_float fTimeDelta)
{
}

void CState_Base::Update(_float fTimeDelta)
{
}

void CState_Base::Late_Update(_float fTimeDelta)
{
}

void CState_Base::OnState_End()
{
}

void CState_Base::OnHit(const ATTACKDESC& AttackDesc)
{
}

void CState_Base::Init_AttackDesc()
{
}

void CState_Base::Reset_AttackDesc()
{
}

void CState_Base::Setup_RootRotation()
{
	_vector vQuat = m_pModel->Get_NowRootQuat();

	ADD_EVENT(bind(&CTransform::Turn_Quaternion, m_pOwnerTransform, vQuat, XMLoadFloat4x4(&m_OffsetMatrix))); // 마지막 회전상태를 가짐

}


void CState_Base::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "ToolTriggerObj.h"

CToolTriggerObj::CToolTriggerObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolTriggerObj::CToolTriggerObj(const CToolTriggerObj& rhs)
	: CGameObject(rhs)
{

}

HRESULT CToolTriggerObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolTriggerObj::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (nullptr != pArg)
		m_TriggerDesc = *(TRIGGER_DESC*)pArg;

	CCollider::COLLIDERDESC pDesc = {};
	pDesc.vCenter = { 0.f, 0.f, 0.f };
	pDesc.pOwner = this;
	pDesc.vSize = { 1.f, 1.f, 1.f };
	pDesc.vRotation = { 0.f, 0.f, 0.f };
	pDesc.bActive = true;
	pDesc.eType = (CCollider::ColliderType)m_TriggerDesc.eType;
	pDesc.strCollisionLayer = "EventTrigger";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	switch (pDesc.eType)
	{
	case CCollider::AABB:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_AABB"), TEXT("Trigger"), (CComponent**)&m_pCollider, &pDesc)))
			return E_FAIL;
		break;

	case CCollider::OBB:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_OBB"), TEXT("Trigger"), (CComponent**)&m_pCollider, &pDesc)))
			return E_FAIL;
		break;

	case CCollider::SPHERE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Trigger"), (CComponent**)&m_pCollider, &pDesc)))
			return E_FAIL;
		break;
	}

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_TriggerDesc.matTrigger));

	return S_OK;
}

void CToolTriggerObj::Tick(_float fTimeDelta)
{
}

void CToolTriggerObj::LateTick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());

	if (m_bDebug)
		m_pGameInstance->Add_RenderComponent(m_pCollider);
}

HRESULT CToolTriggerObj::Render()
{
	return S_OK;
}

void CToolTriggerObj::Save_TriggerData(HANDLE hFile, _ulong& dwByte)
{
	const _float4x4 WorldMatrix = m_pTransform->Get_WorldFloat4x4();
	WriteFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

	_uint iType = (_uint)m_TriggerDesc.eType;
	DWORD iLen = (DWORD)m_TriggerDesc.strName.length();
	WriteFile(hFile, &iType, sizeof _uint, &dwByte, nullptr);
	WriteFile(hFile, &iLen, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, m_TriggerDesc.strName.c_str(), iLen * sizeof _tchar, &dwByte, nullptr);
}

CToolTriggerObj* CToolTriggerObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolTriggerObj* pInstance = new CToolTriggerObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolTriggerObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolTriggerObj::Clone(void* pArg)
{
	CToolTriggerObj* pInstance = new CToolTriggerObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolTriggerObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolTriggerObj::Free()
{
	__super::Free();

	Safe_Release(m_pOwner);
	Safe_Release(m_pCollider);
}

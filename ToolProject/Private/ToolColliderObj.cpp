#include "ToolColliderObj.h"

#include "Bone.h"


CToolColliderObj::CToolColliderObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolColliderObj::CToolColliderObj(const CToolColliderObj& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolColliderObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolColliderObj::Initialize(void* pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	CCollider::COLLIDERDESC* pDesc = (CCollider::COLLIDERDESC*)pArg;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	switch (((CCollider::COLLIDERDESC*)pArg)->eType)
	{
	case CCollider::AABB:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_AABB"), TEXT("Collider"), (CComponent**)&m_pCollider, pArg)))
			return E_FAIL;
		break;

	case CCollider::OBB:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_OBB"), TEXT("Collider"), (CComponent**)&m_pCollider, pArg)))
			return E_FAIL;
		break;

	case CCollider::SPHERE:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, pArg)))
			return E_FAIL;
		break;
	}

	m_pOwner = ((CCollider::COLLIDERDESC*)pArg)->pOwner;

	m_pOwnerModel = static_cast<CModel*>(m_pOwner->Find_Component(L"Model"));

	return S_OK;
}

void CToolColliderObj::Tick(_float fTimeDelta)
{
}

void CToolColliderObj::LateTick(_float fTimeDelta)
{
	if (m_pAttachBone)
	{
		_matrix SocketMatrix = m_pAttachBone->Get_CombinedTransformation();
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		m_pTransform->Set_WorldMatrix(SocketMatrix * m_pOwner->Get_Transform()->Get_WorldMatrix());
	}
	else
	{
		m_pTransform->Set_WorldMatrix(m_pOwner->Get_Transform()->Get_WorldMatrix());
	}

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());

	m_pGameInstance->Add_RenderComponent(m_pCollider);
}

HRESULT CToolColliderObj::Render()
{

	return S_OK;
}


void CToolColliderObj::Set_AttachBone(const _char* szBoneName)
{
	if (m_pOwner)
		m_pAttachBone = m_pOwnerModel->Get_Bone(szBoneName);

}

CToolColliderObj* CToolColliderObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolColliderObj* pInstance = new CToolColliderObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolColliderObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolColliderObj::Clone(void* pArg)
{
	CToolColliderObj* pInstance = new CToolColliderObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolColliderObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolColliderObj::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
}

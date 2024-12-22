#include "OBB.h"
#include "AABB.h"
#include "Sphere.h"

COBB::COBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext)
{

}

COBB::COBB(const COBB& rhs)
	: CCollider(rhs)
{

}

HRESULT COBB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eColliderType = CCollider::OBB;

	return S_OK;
}

HRESULT COBB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	m_pOriginal_OBB = new BoundingOrientedBox(pColliderDesc->vCenter, _float3(pColliderDesc->vSize.x * 0.5f,
		pColliderDesc->vSize.y * 0.5f,
		pColliderDesc->vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	_matrix		RotationMatrix = XMMatrixRotationX(pColliderDesc->vRotation.x) *
		XMMatrixRotationY(pColliderDesc->vRotation.y) *
		XMMatrixRotationZ(pColliderDesc->vRotation.z);

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);

	return S_OK;
}

void COBB::Update(_fmatrix TransformMatrix)
{
	if (false == m_bActive)
		return;

	m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);
}

_bool COBB::Intersects(CCollider* pTargetCollider)
{
	if (false == m_bActive)
		return false;

	ColliderType		eType = pTargetCollider->Get_ColliderType();

	m_bIsColl = false;

	switch (eType)
	{
	case CCollider::AABB:
		m_bIsColl = m_pOBB->Intersects(*((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::OBB:
		m_bIsColl = m_pOBB->Intersects(*((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::SPHERE:
		m_bIsColl = m_pOBB->Intersects(*((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	return m_bIsColl;
}

_bool COBB::Intersects(_fvector _vRayOrigin, _fvector _vRayDir, _float& _fDist)
{
	if (false == m_bActive)
		return false;

	m_bIsColl = false;

	m_bIsColl = m_pOBB->Intersects(_vRayOrigin, _vRayDir, _fDist);

	return m_bIsColl;
}

void COBB::Remake_Collider(COLLIDERDESC* pColliderDesc)
{
	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);


	m_pOriginal_OBB = new BoundingOrientedBox(pColliderDesc->vCenter, _float3(pColliderDesc->vSize.x * 0.5f,
		pColliderDesc->vSize.y * 0.5f,
		pColliderDesc->vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	_matrix		RotationMatrix = XMMatrixRotationX(pColliderDesc->vRotation.x) *
		XMMatrixRotationY(pColliderDesc->vRotation.y) *
		XMMatrixRotationZ(pColliderDesc->vRotation.z);

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);
}


HRESULT COBB::Render()
{
#ifdef _DEBUG
	if (false == m_bActive)
		return E_FAIL;

	__super::Render();

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();
#endif 

	return S_OK;
}


COBB* COBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COBB* pInstance = new COBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}



CComponent* COBB::Clone(void* pArg)
{
	COBB* pInstance = new COBB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void COBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);

}

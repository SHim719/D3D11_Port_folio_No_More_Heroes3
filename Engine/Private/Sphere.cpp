#include "OBB.h"
#include "AABB.h"
#include "Sphere.h"

CSphere::CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext)
{

}

CSphere::CSphere(const CSphere& rhs)
	: CCollider(rhs)
{

}

HRESULT CSphere::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eColliderType = CCollider::SPHERE;

	return S_OK;
}

HRESULT CSphere::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	m_pOriginal_Sphere = new BoundingSphere(pColliderDesc->vCenter, pColliderDesc->vSize.x * 0.5f);

	m_pSphere = new BoundingSphere(*m_pOriginal_Sphere);

	return S_OK;
}

void CSphere::Update(_fmatrix TransformMatrix)
{
	if (false == m_bActive)
		return;

	m_pOriginal_Sphere->Transform(*m_pSphere, TransformMatrix);
}

_bool CSphere::Intersects(CCollider* pTargetCollider)
{
	if (false == m_bActive)
		return false;

	ColliderType		eType = pTargetCollider->Get_ColliderType();

	m_bIsColl = false;

	switch (eType)
	{
	case CCollider::AABB:
		m_bIsColl = m_pSphere->Intersects(*((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::OBB:
		m_bIsColl = m_pSphere->Intersects(*((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::SPHERE:
		m_bIsColl = m_pSphere->Intersects(*((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	return m_bIsColl;
}

_bool CSphere::Intersects(_fvector _vRayOrigin, _fvector _vRayDir, _float& _fDist)
{
	if (false == m_bActive)
		return false;

	m_bIsColl = false;

	m_bIsColl = m_pSphere->Intersects(_vRayOrigin, _vRayDir, _fDist);

	return m_bIsColl;
}

void CSphere::Remake_Collider(COLLIDERDESC* pColliderDesc)
{
	Safe_Delete(m_pOriginal_Sphere);
	Safe_Delete(m_pSphere);

	m_pOriginal_Sphere = new BoundingSphere(pColliderDesc->vCenter, pColliderDesc->vSize.x * 0.5f);
	  
	m_pSphere = new BoundingSphere(*m_pOriginal_Sphere);
}

#ifdef _DEBUG
HRESULT CSphere::Render()
{
	if (false == m_bActive)
		return E_FAIL;

	__super::Render();

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

#endif


CSphere* CSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphere* pInstance = new CSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}



CComponent* CSphere::Clone(void* pArg)
{
	CSphere* pInstance = new CSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CSphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_Sphere);
	Safe_Delete(m_pSphere);

}

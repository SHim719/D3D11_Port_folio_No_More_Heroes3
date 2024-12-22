#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"

CAABB::CAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCollider(pDevice, pContext)
{
}

CAABB::CAABB(const CAABB& rhs)
	: CCollider(rhs)
	, m_pAABB(rhs.m_pAABB)
{
}

HRESULT CAABB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_eColliderType = CCollider::AABB;
	return S_OK;
}

HRESULT CAABB::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERDESC* pColliderDesc = (COLLIDERDESC*)pArg;

	m_pOriginal_AABB = new BoundingBox(pColliderDesc->vCenter, _float3(pColliderDesc->vSize.x * 0.5f,
		pColliderDesc->vSize.y * 0.5f,
		pColliderDesc->vSize.z * 0.5f));

	m_pAABB = new BoundingBox(*m_pOriginal_AABB);

	return S_OK;
}

void CAABB::Update(_fmatrix TransformMatrix)
{
	if (false == m_bActive)
		return;

	m_pOriginal_AABB->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
}

_bool CAABB::Intersects(CCollider* pTargetCollider)
{
	if (false == m_bActive)
		return false;

	CCollider::ColliderType	eType = pTargetCollider->Get_ColliderType();

	switch (eType)
	{
	case CCollider::AABB:
		m_bIsColl = m_pAABB->Intersects(*((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::OBB:
		m_bIsColl = m_pAABB->Intersects(*((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::SPHERE:
		m_bIsColl = m_pAABB->Intersects(*((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}
	
	return m_bIsColl;
}

_bool CAABB::Intersects(_fvector _vRayOrigin, _fvector _vRayDir, _float& _fDist)
{
	if (false == m_bActive)
		return false;

	m_bIsColl = false;

	m_bIsColl = m_pAABB->Intersects(_vRayOrigin, _vRayDir, _fDist);

	return m_bIsColl;
}

void CAABB::Remake_Collider(COLLIDERDESC* pColliderDesc)
{
	Safe_Delete(m_pOriginal_AABB);
	Safe_Delete(m_pAABB);

	m_pOriginal_AABB = new BoundingBox(pColliderDesc->vCenter, _float3(pColliderDesc->vSize.x * 0.5f,
		pColliderDesc->vSize.y * 0.5f,
		pColliderDesc->vSize.z * 0.5f));

	m_pAABB = new BoundingBox(*m_pOriginal_AABB);
}

_vector CAABB::Get_Size() const
{
	return XMVectorSet(m_pOriginal_AABB->Extents.x * 2.f, m_pOriginal_AABB->Extents.y * 2.f, m_pOriginal_AABB->Extents.z * 2.f, 1.f);
}					   

void CAABB::Set_Size(_fvector vSize)
{
	m_pOriginal_AABB->Extents.x = vSize.m128_f32[0] * 0.5f;
	m_pOriginal_AABB->Extents.y = vSize.m128_f32[1] * 0.5f;
	m_pOriginal_AABB->Extents.z = vSize.m128_f32[2] * 0.5f;
}

void CAABB::Set_Center(_fvector vCenter)
{
	XMStoreFloat3(&m_pOriginal_AABB->Center, vCenter);
}

#ifdef _DEBUG
HRESULT CAABB::Render()
{
	if (false == m_bActive)
		return E_FAIL;

	__super::Render();

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}
#endif

CAABB* CAABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAABB* pInstance = new CAABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CAABB::Clone(void* pArg)
{
	CAABB* pInstance = new CAABB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_AABB);
	Safe_Delete(m_pAABB);
}

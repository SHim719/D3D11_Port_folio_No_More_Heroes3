#include "Collision_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Update()
{
	/* Player && Monster */
	Execute_Collision("Travis_Weapon", "Monster_HitBox", CollisionType::TRIGGER);
	Execute_Collision("Travis_BodyAttack", "Monster_HitBox", CollisionType::TRIGGER);
	Execute_Collision("Monster_Weapon", "Travis_HitBox", CollisionType::TRIGGER);
	Execute_Collision("Travis_Projectile", "Monster_Targetable", CollisionType::TRIGGER);
	Execute_Collision("Travis_Projectile", "Monster_HitBox", CollisionType::TRIGGER);
	Execute_Collision("Travis_HitBox", "Monster_Targetable", CollisionType::TRIGGER);
	Execute_Collision("Travis_HitBox", "EventTrigger", CollisionType::TRIGGER);
	Execute_Collision("Travis_Bike", "EventTrigger", CollisionType::TRIGGER);
	Execute_Collision("Monster_Weapon", "Ground", CollisionType::TRIGGER);

	Execute_Collision("Travis_HitBox", "Monster_HitBox", CollisionType::COLLISION);
	Execute_Collision("Monster_HitBox", "Monster_HitBox", CollisionType::COLLISION);
	Execute_Collision("NPC_CarHitBox", "Travis_Bike", CollisionType::COLLISION);
	Execute_Collision("Travis_HitBox", "Travis_Bike", CollisionType::TRIGGER);
	Execute_Collision("Travis_HitBox", "NPC_HitBox", CollisionType::TRIGGER);

	// SmashBros & Mario
	Execute_Collision_Platformer("Physical_Character", "Physical_Terrain", CollisionType::COLLISION);
	Execute_Collision_Platformer("Physical_Character", "Physical_Wall", CollisionType::COLLISION);
	Execute_Collision_Platformer("Physical_Character", "Physical_Block", CollisionType::COLLISION);

	Execute_Collision("Travis_HitBox", "Physical_Terrain", CollisionType::COLLISION);
	Execute_Collision("Travis_HitBox", "Physical_Wall", CollisionType::COLLISION);
	Execute_Collision("Travis_HitBox", "Physical_Block", CollisionType::COLLISION);
	Execute_Collision("Travis_HitBox", "Physical_Collectible", CollisionType::TRIGGER);

	Execute_Collision_Platformer("Travis_Hat", "Physical_Block", CollisionType::COLLISION);

	Execute_Collision("Physical_Character", "Mario_Monster_HitBox", CollisionType::TRIGGER);
	Execute_Collision("Travis_Weapon", "Mario_Monster_HitBox", CollisionType::TRIGGER);
	Execute_Collision("Travis_HitBox", "Mario_Monster_HitBox", CollisionType::TRIGGER);
}

void CCollision_Manager::Add_ColliderToLayer(const string& strLayer, CCollider* pCollider)
{
	auto it = m_CollisionLayer.find(strLayer);

	if (m_CollisionLayer.end() == it)
	{
		list<CCollider*> Layer;
		Layer.push_back(pCollider);

		m_CollisionLayer.emplace(strLayer, Layer);
	}
	else
		(it->second).push_back(pCollider);

	Safe_AddRef(pCollider);
}

void CCollision_Manager::Clear()
{
	m_CollisionInfo.clear();

	for (auto Pair : m_CollisionLayer)
	{
		for (auto pColl : Pair.second)
		{
			Safe_Release(pColl);
		}
	}

	m_CollisionLayer.clear();
}


void CCollision_Manager::Execute_Collision(const string& strDstLayer, const string& strSrcLayer, CollisionType eType)
{
	auto DstLayerIt = m_CollisionLayer.find(strDstLayer);
	if (m_CollisionLayer.end() == DstLayerIt)
		return;
	auto SrcLayerIt = m_CollisionLayer.find(strSrcLayer);
	if (m_CollisionLayer.end() == SrcLayerIt)
		return;

	list<CCollider*>& DstColliders = DstLayerIt->second;
	list<CCollider*>& SrcColliders = SrcLayerIt->second;

	for (auto DstIt = DstColliders.begin(); DstIt != DstColliders.end(); )
	{
		CCollider* pDstCollider = *DstIt;
		if (nullptr == pDstCollider)
			continue;

		CGameObject* pDstObj = pDstCollider->Get_Owner();
		if (pDstObj->Is_Destroyed())
		{
			DstIt = DstColliders.erase(DstIt);
			Safe_Release(pDstCollider);
			continue;
		}
			
		for (auto SrcIt = SrcColliders.begin(); SrcIt != SrcColliders.end(); )
		{
			if (*SrcIt == *DstIt)
			{
				++SrcIt;
				continue;
			}
				

			CCollider* pSrcCollider = *SrcIt;
			if (nullptr == pSrcCollider)
				continue;

			CGameObject* pSrcObj = pSrcCollider->Get_Owner();

			CollisionID id;
			id.left = pDstCollider->Get_CollisionID();
			id.right = pSrcCollider->Get_CollisionID();

			auto it = m_CollisionInfo.find(id.id);
			if (m_CollisionInfo.end() == it)
				m_CollisionInfo.insert({ id.id, false });

			it = m_CollisionInfo.find(id.id);

			if (false == pDstCollider->Is_Active() || false == pSrcCollider->Is_Active() ||
				false == pDstObj->Is_Active() || false == pSrcObj->Is_Active() ||
				pDstObj->Is_OutOfLayer() || pSrcObj->Is_OutOfLayer())
			{
				if (it->second)
				{
					pDstObj->OnCollisionExit(pSrcObj, pSrcCollider);
					pSrcObj->OnCollisionExit(pDstObj, pDstCollider);
			
					pDstCollider->Set_IsColl(false);
					pSrcCollider->Set_IsColl(false);
			
					it->second = false;
				}

				if (pSrcObj->Is_Destroyed())
				{
					SrcIt = SrcColliders.erase(SrcIt);
					Safe_Release(pSrcCollider);
				}
				else
				{
					++SrcIt;
				}
				continue;
			}


			if (pSrcCollider->Intersects(pDstCollider))
			{
				if (false == it->second)
				{
					if (COLLISION == eType)
						Push_Object(pDstCollider, pSrcCollider, pDstObj->Get_Transform());
					pDstObj->OnCollisionEnter(pSrcObj, pSrcCollider);
					pSrcObj->OnCollisionEnter(pDstObj, pDstCollider);
					pDstCollider->Set_DstCollider(pSrcCollider);
					pSrcCollider->Set_DstCollider(pDstCollider);
					
					it->second = true;
				}

				else
				{
					if (COLLISION == eType)
						Push_Object(pDstCollider, pSrcCollider, pDstObj->Get_Transform());
					pDstObj->OnCollisionStay(pSrcObj, pSrcCollider);
					pSrcObj->OnCollisionStay(pDstObj, pDstCollider);

				}
			}
			else if (it->second)
			{
				pDstObj->OnCollisionExit(pSrcObj, pSrcCollider);
				pSrcObj->OnCollisionExit(pDstObj, pDstCollider);

				it->second = false;
			}

			++SrcIt;
		}

		++DstIt;
	}
	
}

void CCollision_Manager::Execute_Collision_Platformer(const string& strDstLayer, const string& strSrcLayer, CollisionType eType)
{
	auto DstLayerIt = m_CollisionLayer.find(strDstLayer);
	if (m_CollisionLayer.end() == DstLayerIt)
		return;
	auto SrcLayerIt = m_CollisionLayer.find(strSrcLayer);
	if (m_CollisionLayer.end() == SrcLayerIt)
		return;

	list<CCollider*>& DstColliders = DstLayerIt->second;
	list<CCollider*>& SrcColliders = SrcLayerIt->second;

	for (auto DstIt = DstColliders.begin(); DstIt != DstColliders.end(); )
	{
		CCollider* pDstCollider = *DstIt;
		if (nullptr == pDstCollider)
			continue;

		CGameObject* pDstObj = pDstCollider->Get_Owner();
		if (pDstObj->Is_Destroyed())
		{
			DstIt = DstColliders.erase(DstIt);
			Safe_Release(pDstCollider);
			continue;
		}

		for (auto SrcIt = SrcColliders.begin(); SrcIt != SrcColliders.end(); )
		{
			if (*SrcIt == *DstIt)
			{
				++SrcIt;
				continue;
			}


			CCollider* pSrcCollider = *SrcIt;
			if (nullptr == pSrcCollider)
				continue;

			CGameObject* pSrcObj = pSrcCollider->Get_Owner();

			CollisionID id;
			id.left = pDstCollider->Get_CollisionID();
			id.right = pSrcCollider->Get_CollisionID();

			auto it = m_CollisionInfo.find(id.id);
			if (m_CollisionInfo.end() == it)
				m_CollisionInfo.insert({ id.id, false });

			it = m_CollisionInfo.find(id.id);

			if (false == pDstCollider->Is_Active() || false == pSrcCollider->Is_Active() ||
				false == pDstObj->Is_Active() || false == pSrcObj->Is_Active() ||
				pDstObj->Is_OutOfLayer() || pSrcObj->Is_OutOfLayer())
			{
				if (it->second)
				{
					pDstObj->OnCollisionExit(pSrcObj, pSrcCollider);
					pSrcObj->OnCollisionExit(pDstObj, pDstCollider);

					pDstCollider->Set_IsColl(false);
					pSrcCollider->Set_IsColl(false);

					it->second = false;
				}

				if (pSrcObj->Is_Destroyed())
				{
					SrcIt = SrcColliders.erase(SrcIt);
					Safe_Release(pSrcCollider);
				}
				else
				{
					++SrcIt;
				}
				continue;
			}


			if (pSrcCollider->Intersects(pDstCollider))
			{
				if (false == it->second)
				{
					// ù �浹
					if (COLLISION == eType)
						Push_Object_Platformer(pDstCollider, pSrcCollider, pDstObj->Get_Transform());
					pDstObj->OnCollisionEnter(pSrcObj, pSrcCollider);
					pSrcObj->OnCollisionEnter(pDstObj, pDstCollider);

					it->second = true;
				}

				else
				{
					// �浹 ��
					if (COLLISION == eType)
						Push_Object_Platformer(pDstCollider, pSrcCollider, pDstObj->Get_Transform());
					pDstObj->OnCollisionStay(pSrcObj, pSrcCollider);
					pSrcObj->OnCollisionStay(pDstObj, pDstCollider);

				}
			}
			else if (it->second)
			{
				// �浹�� �� �������� 
				pDstObj->OnCollisionExit(pSrcObj, pSrcCollider);
				pSrcObj->OnCollisionExit(pDstObj, pDstCollider);

				it->second = false;
			}

			++SrcIt;
		}

		++DstIt;
	}

}


void CCollision_Manager::Push_Object(CCollider* pDstCollider, CCollider* pSrcCollider, CTransform* pDstTransform)
{
	CCollider::ColliderType eDstCollType = pDstCollider->Get_ColliderType();
	CCollider::ColliderType eSrcCollType = pSrcCollider->Get_ColliderType();

	if (CCollider::SPHERE == eDstCollType)
	{
		if (CCollider::SPHERE == eSrcCollType)
		{
			BoundingSphere* pDstSphere = static_cast<CSphere*>(pDstCollider)->Get_Collider();
			BoundingSphere* pSrcSphere = static_cast<CSphere*>(pSrcCollider)->Get_Collider();

			_vector vDstCenter = XMLoadFloat3(&(pDstSphere->Center));
			_vector vSrcCenter = XMLoadFloat3(&(pSrcSphere->Center));

			_float vDstRadius = pDstSphere->Radius;
			_float vSrcRadius = pSrcSphere->Radius;

			_vector vPushDir = vDstCenter - vSrcCenter;
			
			_float fDist = vDstRadius + vSrcRadius - XMVectorGetX(XMVector3Length(vPushDir)) - 0.1f;

			if (fDist > 0.f) {
				vPushDir = XMVector3Normalize(XMVectorSetY(vPushDir, 0.f));

				pDstTransform->Add_Position(vPushDir, fDist, static_cast<CNavigation*>(pDstCollider->Get_Owner()->Find_Component(L"Navigation")));
			}
		}
		else if (CCollider::AABB == eSrcCollType)
		{
			BoundingSphere* pDstSphere = static_cast<CSphere*>(pDstCollider)->Get_Collider();
			BoundingBox* pSrcAABB = static_cast<CAABB*>(pSrcCollider)->Get_Collider();

			_vector vSrcAABBMin = XMLoadFloat3(&pSrcAABB->Center) - XMLoadFloat3(&pSrcAABB->Extents);
			_vector vSrcAABBMax = XMLoadFloat3(&pSrcAABB->Center) + XMLoadFloat3(&pSrcAABB->Extents);

			_vector vClosetPoint = XMVectorSet(
				max(vSrcAABBMin.m128_f32[0], min(pDstSphere->Center.x, vSrcAABBMax.m128_f32[0]))
				, max(vSrcAABBMin.m128_f32[1], min(pDstSphere->Center.y, vSrcAABBMax.m128_f32[1]))
				, max(vSrcAABBMin.m128_f32[2], min(pDstSphere->Center.z, vSrcAABBMax.m128_f32[2])), 1.f);

			_vector vPushDir = XMVectorSet(pDstSphere->Center.x - vClosetPoint.m128_f32[0]
				, pDstSphere->Center.y - vClosetPoint.m128_f32[1]
				, pDstSphere->Center.z - vClosetPoint.m128_f32[2], 0.f);

			_float fDist = XMVector3Length(vPushDir).m128_f32[0];

			vPushDir = XMVector3Normalize(vPushDir);

			pDstTransform->Add_Position(vPushDir, pDstSphere->Radius - fDist, static_cast<CNavigation*>(pDstCollider->Get_Owner()->Find_Component(L"Navigation")));
		}
	}
}

void CCollision_Manager::Push_Object_Platformer(CCollider* pDstCollider, CCollider* pSrcCollider, CTransform* pDstTransform)
{
	CCollider::ColliderType eDstCollType = pDstCollider->Get_ColliderType();
	CCollider::ColliderType eSrcCollType = pSrcCollider->Get_ColliderType();

	if (CCollider::SPHERE == eDstCollType)
	{
		if (CCollider::SPHERE == eSrcCollType)
		{
			BoundingSphere* pDstSphere = static_cast<CSphere*>(pDstCollider)->Get_Collider();
			BoundingSphere* pSrcSphere = static_cast<CSphere*>(pSrcCollider)->Get_Collider();

			_vector vDstCenter = XMLoadFloat3(&(pDstSphere->Center));
			_vector vSrcCenter = XMLoadFloat3(&(pSrcSphere->Center));

			_float vDstRadius = pDstSphere->Radius;
			_float vSrcRadius = pSrcSphere->Radius;

			_vector vPushDir = vDstCenter - vSrcCenter;

			_float fDist = vDstRadius + vSrcRadius - XMVectorGetX(XMVector3Length(vPushDir)) - 0.1f;

			if (fDist > 0.f) {
				vPushDir = XMVector3Normalize(XMVectorSetY(vPushDir, 0.f));

				pDstTransform->Add_Position(vPushDir, fDist, static_cast<CNavigation*>(pDstCollider->Get_Owner()->Find_Component(L"Navigation")));
			}
		}
		else if (CCollider::AABB == eSrcCollType)
		{
			BoundingSphere* pDstSphere = static_cast<CSphere*>(pDstCollider)->Get_Collider();
			BoundingBox* pSrcAABB = static_cast<CAABB*>(pSrcCollider)->Get_Collider();

			_vector vSrcAABBMin = XMLoadFloat3(&pSrcAABB->Center) - XMLoadFloat3(&pSrcAABB->Extents);
			_vector vSrcAABBMax = XMLoadFloat3(&pSrcAABB->Center) + XMLoadFloat3(&pSrcAABB->Extents);

			_vector vClosetPoint = {};

				vClosetPoint = XMVectorSet(
					max(vSrcAABBMin.m128_f32[0], min(pDstSphere->Center.x, vSrcAABBMax.m128_f32[0]))
					, max(vSrcAABBMin.m128_f32[1], min(pDstSphere->Center.y, vSrcAABBMax.m128_f32[1]))
					, max(vSrcAABBMin.m128_f32[2], min(pDstSphere->Center.z, vSrcAABBMax.m128_f32[2])), 1.f);
		
			_vector vPushDir = {};

			vPushDir = XMVectorSet(
				0.f
				, pDstSphere->Center.y - vClosetPoint.m128_f32[1]
				, 0.f, 0.f);

			if (fabsf(pDstSphere->Center.x - vClosetPoint.m128_f32[0]) > fabsf(pDstSphere->Center.y - vClosetPoint.m128_f32[1])) {
				vPushDir = XMVectorSet(
					pDstSphere->Center.x - vClosetPoint.m128_f32[0]
					, 0.f
					, 0.f, 0.f);
			}
			else {
				vPushDir = XMVectorSet(
					0.f
					, pDstSphere->Center.y - vClosetPoint.m128_f32[1]
					, 0.f, 0.f);
			}

			pDstCollider->Set_PushDist(vPushDir);

			_float fDist = XMVector3Length(vPushDir).m128_f32[0];

			vPushDir = XMVector3Normalize(vPushDir);

			pDstTransform->Add_Position(vPushDir, pDstSphere->Radius - fDist, static_cast<CNavigation*>(pDstCollider->Get_Owner()->Find_Component(L"Navigation")));
		}
	}
}

void CCollision_Manager::Execute_RayCollision(CGameObject* _pSrcObject, const string& _strRayTag, _fvector _vRayOrigin, _fvector _vRayDir, const string& _strDstLayer)
{
	auto DstLayerIt = m_CollisionLayer.find(_strDstLayer);
	if (m_CollisionLayer.end() == DstLayerIt)
		return;

	list<CCollider*>& DstColliders = DstLayerIt->second;

	for (auto DstIt = DstColliders.begin(); DstIt != DstColliders.end(); ) {
		CCollider* pDstCollider = *DstIt;
		if (nullptr == pDstCollider)
			continue;

		CGameObject* pDstObj = pDstCollider->Get_Owner();
		if (pDstObj->Is_Destroyed())
		{
			DstIt = DstColliders.erase(DstIt);
			Safe_Release(pDstCollider);
			continue;
		}

		_float fDist = { 0.f };

		if (pDstCollider->Intersects(_vRayOrigin, _vRayDir, fDist)) {
			_pSrcObject->RayArrived(_strRayTag, _vRayOrigin, _vRayDir, fDist, pDstObj);
			pDstObj->OnRayCollision(_strRayTag,_vRayOrigin,_vRayDir,fDist,_pSrcObject);
		}

		++DstIt;
	}
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollision_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	for (auto Pair : m_CollisionLayer)
	{
		for (auto pColl : Pair.second)
		{
			Safe_Release(pColl);
		}
	}
}

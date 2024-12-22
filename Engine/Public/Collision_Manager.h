#pragma once

#include "Base.h"
#include "Collider.h"


BEGIN(Engine)
class CCollision_Manager : public CBase
{
private:
	union CollisionID
	{
		struct
		{
			UINT32 left;
			UINT32 right;
		};

		UINT64 id;
	};

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

	HRESULT Initialize();

public:
	void Update();

private:
	CGameInstance* m_pGameInstance = nullptr;

	unordered_map<UINT64, bool>					m_CollisionInfo;
	unordered_map<string, list<CCollider*>>		m_CollisionLayer; 

public:
	void Add_ColliderToLayer(const string& strLayer, CCollider* pCollider);
	void Clear();

private:
	void Execute_Collision(const string& strDstLayer, const string& strSrcLayer, CollisionType eType);
	void Execute_Collision_Platformer(const string& strDstLayer, const string& strSrcLayer, CollisionType eType);
	void Push_Object(CCollider* pDstCollider, CCollider* pSrcCollider, class CTransform* pDstTransform);
	void Push_Object_Platformer(CCollider* pDstCollider, CCollider* pSrcCollider, class CTransform* pDstTransform);

public: // BH - Add Ray Checking
	void Execute_RayCollision(CGameObject* _pSrcObject, const string& _strRayTag, _fvector _vRayOrigin,_fvector _vRayDir, const string& _strDstLayer);

public:
	static CCollision_Manager* Create();
	void Free() override;

};

END


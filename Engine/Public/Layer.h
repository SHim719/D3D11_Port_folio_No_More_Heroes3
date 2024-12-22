#pragma once

#include "Base.h"


BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);

	void Manage_LifeCycle();
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	class CComponent* Find_Component(const wstring& strComponentTag, _uint iIndex);
	class CGameObject* Find_GameObject(_uint iIndex);

private:
	list<class CGameObject*>			m_GameObjects;

public:
	list<class CGameObject*>&	Get_GameObjects() { return m_GameObjects; }

public:
	static CLayer* Create();
	virtual void Free() override;
};

END
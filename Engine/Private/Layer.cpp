#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;
	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

void CLayer::Manage_LifeCycle()
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if (nullptr != *it)
		{
			if ((*it)->Is_OutOfLayer())
			{
			(*it)->OnExit_Layer();

				Safe_Release(*it);
				it = m_GameObjects.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void CLayer::PriorityTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->Is_Active())
			pGameObject->PriorityTick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->Is_Active())
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::LateTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject && pGameObject->Is_Active())
			pGameObject->LateTick(fTimeDelta);
	}
}

CComponent* CLayer::Find_Component(const wstring& strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter)->Find_Component(strComponentTag);
}

CGameObject* CLayer::Find_GameObject(_uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_GameObjects.end())
		return nullptr;

	return (*iter);
}


CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
	
}

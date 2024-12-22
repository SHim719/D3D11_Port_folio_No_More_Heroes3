#pragma once

#include "Base.h"

/* 1. 현재 할당된 레벨을 보관하낟.*/
/* 1_1. 레벨 교체 시, 기존 레벨을 파괴하는 역활. 기존 레벨용으로 할당됐던 메모리도 해소하낟.  */
/* 2. 활성화된 레벨의 Tick Render를 무한히 호췰한다. */


BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	void Change_Level(class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iCurrentLevelID = { 0 };
public:
	_uint Get_CurrentLevelID();

	void Set_CurrentLevelID(_uint iLevelID) {
		m_iCurrentLevelID = iLevelID;
	}

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	void Set_TimeDivide(float fTimeDivide) 
	{ m_fTimeDivide = fTimeDivide; }

	float Get_TimeDelta() {	
		return m_fTimeDelta / m_fTimeDivide;	}

public:
	HRESULT			Initialize();
	void			Update();

private:
	LARGE_INTEGER			m_CurTime;
	LARGE_INTEGER			m_OldTime;
	LARGE_INTEGER			m_OriginTime;
	LARGE_INTEGER			m_CpuTick;

	float					m_fTimeDelta;
	float					m_fTimeDivide = 1;
public:
	static CTimer* Create();
	virtual void Free() override;
};


END
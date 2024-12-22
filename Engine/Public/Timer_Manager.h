#pragma once

#include "Base.h"

/* 여러개의 타이머를 태그(Key)로 구분하여 저장한다. */
/* 특정 타이머를 계산하고 리턴해준다. */

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float	Get_TimeDelta(const wstring& strTimerTag);

public:
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);

private:
	map<const wstring, class CTimer*>			m_Timers;

private:
	class CTimer* Find_Timer(const wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free() override;
};

END
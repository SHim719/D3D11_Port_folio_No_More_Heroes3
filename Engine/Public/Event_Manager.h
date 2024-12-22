#pragma once

#include "Base.h"
#include "Collider.h"


BEGIN(Engine)
class CEvent_Manager : public CBase
{
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	void Process_Events() {
		for (auto& EventFunc : m_Events)
			EventFunc();
		m_Events.clear();
 	}


public:
	void Add_Event(function<void()> EventFunc) {
		m_Events.push_back(move(EventFunc));
	 }

private:
	list<function<void()>> m_Events;

public:
	static CEvent_Manager* Create();
	void Free() override;

};

END

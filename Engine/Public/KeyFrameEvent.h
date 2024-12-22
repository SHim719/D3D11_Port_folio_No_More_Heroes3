#pragma once

#include "Base.h"

BEGIN(Engine)

class CKeyFrameEvent final : public CBase
{
public:
	enum KEYFRAMETYPE { NORMAL, PLAY_SOUND, KEYFRAMETYPE_END};

	typedef struct KeyFrameDescs
	{
		KEYFRAMETYPE	eKeyFrameType;
		string			strEventName;
		_float			fVolume = 1.f;
	}KEYFRAME_DESC;

private:
	CKeyFrameEvent();
	CKeyFrameEvent(const CKeyFrameEvent& rhs);
	virtual ~CKeyFrameEvent() = default;

	HRESULT Initialize(void* pArg);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	KEYFRAMETYPE		m_eKeyFrameType = { NORMAL };
	string				m_strEventName = "";
	function<void()>	m_CallBackFunc = { nullptr };

	_float				m_fVolume = { 1.f }; // FOR_SOUND

public:
	void Bind_Func(function<void()> CallBackFunc) { 
		m_CallBackFunc = std::move(CallBackFunc); 
	}
	
	void Execute();
	

	const string& Get_EventName() const { return m_strEventName; }
														
public:
	static CKeyFrameEvent* Create(void* pArg);
	CKeyFrameEvent* Clone();
	void Free() override;
};

END
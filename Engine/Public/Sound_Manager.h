#pragma once

#include "Base.h"


BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);

public:
	HRESULT Create_Sound(const string& strPath, const string& strSoundTag);
	HRESULT Play(const string& strSoundTag, _bool bLoop, _float fVolume = 1.f);
	HRESULT Play_WithPosition(const string& strSoundTag, _bool bLoop, _float fPositionSec, _float fVolume = 1.f);
	HRESULT Stop(const string& strSoundTag);
	HRESULT SetVolume(const string& strSoundTag, const _float& fVolume);
	HRESULT SetPosition(const string& strSoundTag, _float fPosition);
	HRESULT Set_Sound_FadeOut(const string& strSoundTag, _float fTime);

	_bool Is_Playing(const string& strSoundTag);

	class CSound* Get_Sound(const string& strSoundTag);
private:
	class CFMOD_Core* m_pFmodCore = { nullptr };

	unordered_map<string, class CSound*>		m_Sounds;
	list<class CSound*>							m_FadeOut_Sounds;

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END
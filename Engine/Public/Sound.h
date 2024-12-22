#pragma once

#include "Base.h"
#include "FMOD_Core.h"

BEGIN(Engine)
class ENGINE_DLL CSound : public CBase
{
public:
    CSound();
    ~CSound() = default;

    void Play(const bool& _bLoop, _float fVolume);
    void Stop();
    _bool Fade_Out(_float fTimeDelta);
    _bool Is_Playing();
    void SetVolume(const float& _fVolume);
    void SetLoop(const bool& _bLoop) { m_bLoop = _bLoop; }
    void SetPosition(_float _fPositionSec);

    void Set_FadeOutTime(_float fFadeOutTime) {
        m_fFadeOutTime = m_fFadeOutTimeAcc = fFadeOutTime;
    }
private:
    HRESULT Initialize(CFMOD_Core* pFMOD_Core, const string& strPath);

private:
    CFMOD_Core*             m_pFMOD_Core = { nullptr };

    class FMOD::Sound*      m_pSound = { nullptr };
    class FMOD::Channel*    m_pChannel = { nullptr };
    _bool                   m_bLoop = { false };

private:
    _float m_fFadeOutTime = { 0.f };
    _float m_fFadeOutTimeAcc = { 0.f };

public:
    static CSound* Create(CFMOD_Core* pFMOD_Core, const string& strPath);
    virtual void Free() override;

};
END


#include "Sound.h"

CSound::CSound()
{
}

void CSound::Play(const bool& _bLoop, _float fVolume)
{
	if (_bLoop)
		m_pSound->setMode(FMOD_LOOP_NORMAL);
	else
		m_pSound->setMode(FMOD_LOOP_OFF);

	m_pFMOD_Core->SoundPlay(m_pSound, &m_pChannel);
	m_pChannel->setVolume(fVolume);
}

void CSound::Stop()
{
	m_pChannel->stop();
}

_bool CSound::Fade_Out(_float fTimeDelta)
{
	m_fFadeOutTimeAcc -= fTimeDelta;
	if (m_fFadeOutTimeAcc < 0.f)
		m_fFadeOutTimeAcc = 0.f;

	SetVolume(m_fFadeOutTimeAcc / m_fFadeOutTime);

	return m_fFadeOutTimeAcc == 0.f;
}

_bool CSound::Is_Playing()
{
	_bool isPlaying;
	m_pChannel->isPlaying(&isPlaying);
	return isPlaying;
}

void CSound::SetVolume(const float& _fVolume)
{
	m_pChannel->setVolume(_fVolume);
}

void CSound::SetPosition(_float _fPositionSec)
{
	if (nullptr == m_pSound) return;
	if (nullptr != m_pChannel)
	{
		_uint iLenMS;
		m_pSound->getLength(&iLenMS, FMOD_TIMEUNIT_MS);

		_uint iPositionMS = _uint(_fPositionSec * 1000.0f);

		if (iPositionMS > iLenMS)
			iPositionMS = iLenMS;
		

		m_pChannel->setPosition(iPositionMS, FMOD_TIMEUNIT_MS);
	}
}

HRESULT CSound::Initialize(CFMOD_Core* pFMOD_Core, const string& strPath)
{
	m_pFMOD_Core = pFMOD_Core;
	Safe_AddRef(m_pFMOD_Core);

	if (false == m_pFMOD_Core->CreateSound(strPath, &m_pSound))
		return E_FAIL;

	return S_OK;
}

CSound* CSound::Create(CFMOD_Core* pFMOD_Core, const string& strPath)
{
	CSound* pInstance = new CSound;
	
	if (FAILED(pInstance->Initialize(pFMOD_Core, strPath)))
	{
		MSG_BOX(TEXT("Failed to Created : CSound"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSound::Free()
{
	m_pSound->release();
	m_pSound = nullptr;

	Safe_Release(m_pFMOD_Core);
}

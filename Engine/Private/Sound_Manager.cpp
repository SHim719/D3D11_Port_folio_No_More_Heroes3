#include "Sound_Manager.h"

#include "Sound.h"
#include "FMOD_Core.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	m_pFmodCore = CFMOD_Core::Create();

	return S_OK;
}

void CSound_Manager::Tick(_float fTimeDelta)
{
	for (auto it = m_FadeOut_Sounds.begin(); it != m_FadeOut_Sounds.end();)
	{
		CSound* pSound = *it;
		if (pSound->Fade_Out(fTimeDelta))
		{
			pSound->SetVolume(1.f);
			pSound->Stop();
			it = m_FadeOut_Sounds.erase(it);
		}
		else
		{
			++it;
		}
	}
}


HRESULT CSound_Manager::Create_Sound(const string& strPath, const string& strSoundTag)
{
	CSound* pSound = CSound::Create(m_pFmodCore, strPath);

	if (nullptr == pSound)
	{
		assert(false);
		return E_FAIL;
	}

	auto iter = m_Sounds.find(strSoundTag);

	if (iter == m_Sounds.end())
	{
		m_Sounds.insert({ strSoundTag, pSound });
	}
	else
	{
		Safe_Release(pSound);
	}

	return S_OK;
}

HRESULT CSound_Manager::Play(const string& strSoundTag, _bool bLoop, _float fVolume)
{
	auto it = m_Sounds.find(strSoundTag);
	
	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->Play(bLoop, fVolume);

	return S_OK;
}

HRESULT CSound_Manager::Play_WithPosition(const string& strSoundTag, _bool bLoop, _float fPositionSec, _float fVolume)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->Play(bLoop, fVolume);
	it->second->SetPosition(fPositionSec);

	return S_OK;
}

HRESULT CSound_Manager::Stop(const string& strSoundTag)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->Stop();

	return S_OK;
}

HRESULT CSound_Manager::SetVolume(const string& strSoundTag, const _float& fVolume)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->SetVolume(fVolume);

	return S_OK;
}

HRESULT CSound_Manager::SetPosition(const string& strSoundTag, _float fPosition)
{
	auto it = m_Sounds.find(strSoundTag);
	
	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->SetPosition(fPosition);
	
	return S_OK;
}

_bool CSound_Manager::Is_Playing(const string& strSoundTag)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return false;

	return it->second->Is_Playing();
}

CSound* CSound_Manager::Get_Sound(const string& strSoundTag)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return nullptr;

	return it->second;
}

HRESULT CSound_Manager::Set_Sound_FadeOut(const string& strSoundTag, _float fTime)
{
	auto it = m_Sounds.find(strSoundTag);

	if (m_Sounds.end() == it)
		return E_FAIL;

	it->second->Set_FadeOutTime(fTime);
	m_FadeOut_Sounds.emplace_back(it->second);

	return S_OK;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& pair : m_Sounds)
	{
		Safe_Release(pair.second);
	}

	Safe_Release(m_pFmodCore);
}

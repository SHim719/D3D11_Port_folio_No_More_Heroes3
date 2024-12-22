#include "FMOD_Core.h"


CFMOD_Core::CFMOD_Core()
{
}

HRESULT CFMOD_Core::Initialize()
{
	void* extraDriverData = nullptr;

	FMOD::Studio::System::create(&m_System);

	m_System->getCoreSystem(&m_CoreSystem);
	m_CoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_MAX, 0);

	m_System->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_CLIP_OUTPUT, extraDriverData);

	return S_OK;
}

bool CFMOD_Core::CreateSound(const std::string& path, FMOD::Sound** sound)
{
	if (FMOD_OK != m_CoreSystem->createSound(path.c_str(), FMOD_3D, 0, sound))
		return false;

	return true;
}

void CFMOD_Core::SoundPlay(FMOD::Sound* sound, FMOD::Channel** channel)
{
	m_CoreSystem->playSound(sound, 0, false, channel);
}

void CFMOD_Core::Set3DListenerAttributes(const _float3& vPosition)
{
	FMOD_VECTOR fmodPos{ 0.0f, 0.0f, 0.3f };
	FMOD_VECTOR fmodVel{ 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR fmodForward{ 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR fmodUp{ 0.0f, 1.0f, 0.0f };

	m_CoreSystem->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
}

CFMOD_Core* CFMOD_Core::Create()
{
	CFMOD_Core* pInstance = new CFMOD_Core();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CFMOD_Core"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFMOD_Core::Free()
{
	m_System->release();
}

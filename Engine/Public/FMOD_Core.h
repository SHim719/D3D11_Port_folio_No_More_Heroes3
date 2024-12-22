#pragma once

#include "Base.h"

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_common.h"
#include "fmod_codec.h"


BEGIN(Engine)

class CFMOD_Core final : public CBase
{
private:
	CFMOD_Core();
	virtual ~CFMOD_Core() = default;

public:
	HRESULT Initialize();
	bool CreateSound(const string& path, FMOD::Sound** sound);
	void SoundPlay(FMOD::Sound* sound, FMOD::Channel** channel);
	void Set3DListenerAttributes(const _float3& vPosition);

private:
	FMOD::Studio::System* m_System = nullptr;
	FMOD::System* m_CoreSystem = nullptr;

public:
	static CFMOD_Core* Create();
	virtual void Free() override;
};

END


#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}
	_uint Get_NumKeyFrames() const {
		return m_iNumKeyFrames;
	}

	_float Get_FrameGap() const {
		return m_KeyFrames[1].fTime - m_KeyFrames[0].fTime;
	}

	_float Get_LastFrameTime() const {
		return m_KeyFrames.back().fTime;
	}

public:
	HRESULT Initialize(ifstream& fin);

	_uint Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CBone* pBone);
	_uint Blend_Transformation(_float fBlendRatio, _float fPlayTime, _uint iCurrentKeyFrame, CBone* pBone);

	_matrix Get_CurTransformation(_float fPlayTime, _uint iCurrentKeyFrame);

	void Set_CurrentKeyFrame(_float fPlayTime);
private:
	char							m_szName[MAX_PATH] = "";

	_uint							m_iNumKeyFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;
public:
	static CChannel* Create(ifstream& fin);
	virtual void Free() override;
};

END
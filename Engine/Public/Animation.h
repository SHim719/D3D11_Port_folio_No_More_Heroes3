#pragma once

#include "Base.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	typedef unordered_multimap<_int, class CKeyFrameEvent*>	ANIMEVENTS;

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(ifstream& fin);
	HRESULT Initialize(const CModel::KEYFRAMEEVENTS& Events, const ANIMEVENTS& AnimEvents);
	_bool Play_Animation(_float fTimeDelta, vector<class CBone*>& Bones, _bool bLoop, _bool bPlay);
	_bool Play_Animation_Blend(_float fTimeDelta, vector<class CBone*>& Bones, _bool bPlay);

	void Reset();
private:
	string											m_strAnimName = "";

	_uint											m_iNumChannels = 0;
	vector<class CChannel*>							m_Channels;

	_float											m_fDuration = 0.f;
	_float											m_fTickPerSecond = 0.f;
	_float											m_fPlayTime = 0.f;

	_float											m_fBlendingTime = 0.f;
	_float											m_fBlendingAcc = 0.f;

	_float											m_fPlayRate = 1.f;
	_int											m_iPrevKeyFrame = 0;

	_float4											m_vInitialRootPos = {};
private:
	vector<_uint>									m_ChannelKeyFrames;
	vector<_uint>									m_BoneIndices;
	unordered_multimap<_int, class CKeyFrameEvent*>	m_KeyFrameEvents;

	_bool*											m_bCheckKeyFrames = { nullptr };
public:
	const string& Get_AnimName() { 
		return m_strAnimName; }
	void Set_BlendingTime(_float fTime) {
		m_fBlendingTime = fTime;
	}

	_uint Get_NowKeyFrame() const { 
		return m_ChannelKeyFrames[0]; }
	_uint Get_NumKeyFrames() const;
	void Set_CurrentKeyFrames(_uint iKeyFrame);

	_vector Get_InitRootPos() {
		return XMLoadFloat4(&m_vInitialRootPos);
	}

	_matrix Get_RootTransformation();
	void Add_KeyFrameEvent(_int iKeyFrame, CKeyFrameEvent* pEvent);

private:
	void Check_KeyFrameEvent();

public:
	static CAnimation* Create(ifstream& fin);
	CAnimation* Clone(const CModel::KEYFRAMEEVENTS& Events);
	virtual void Free() override;
};

END
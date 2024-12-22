#pragma once


#include "Tool_Defines.h"
#include "GameObject.h"
#include "Effect.h"

BEGIN(Tool)

class CCinemaObject final : public CGameObject
{
private:
	CCinemaObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinemaObject(const CCinemaObject& rhs);
	virtual ~CCinemaObject() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_int						m_iModelIdx = { -1 };
	TOOL_CINEMATIC_DESCS		m_CinemaDesc;

	_bool				m_bPlaying = { false };
	_float				m_fPlayTimeAcc = { 0.f };

	TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER ScaleFrameIt;
	TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER PositionFrameIt;
	TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER LookFrameIt;
	TOOL_CINEMATIC_DESCS::CINE_EVENTS_ITER EventFrameIt;

	_int						m_iEventIdx = { 0 };

	CEffect::CINE_EFFECT_DESC   m_EffectSpawnDesc = {};

public:
	TOOL_CINEMATIC_DESCS&	Get_CinemaDesc(){
		return m_CinemaDesc;
	}

	void Set_CinemaDesc(const TOOL_CINEMATIC_DESCS& CinemaDesc) {
		m_CinemaDesc = CinemaDesc;
	}

	void Play_Cinematic() {
		m_bPlaying = true;
		m_CinemaDesc.EventPlayed = 0;
	}

	void End_Cinematic() {
		m_bPlaying = false;
	}

	void Update_PlayTime(_float fPlayTime);

private:
	CShader*	m_pShader = nullptr;
	CModel*		m_pModel = nullptr;

public:
	CModel* Get_Model() const {
		return m_pModel;
	}

	void Change_Model(CModel* pModel);

	_int Get_ModelIdx() const {
		return m_iModelIdx;
	}

	void Set_ModelIdx(_int iIdx) {
		m_iModelIdx = iIdx;
	}

	CEffect::CINE_EFFECT_DESC* Get_CineEffectDesc() {
		return &m_EffectSpawnDesc;
	}


public:
	static CCinemaObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END


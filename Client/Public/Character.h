#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameEffect.h"

BEGIN(Client)

class CCharacter abstract : public CGameObject
{
protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

	virtual HRESULT Initialize(void* _pArg)				override;
	virtual HRESULT Render_ShadowDepth()				override;
	virtual HRESULT Render_Cascade()					override;

	virtual void Bind_KeyFrames();
	virtual void Bind_KeyFrameEffects();
	virtual void Bind_KeyFrameSounds();

	void Decide_PassIdx()			override;
	HRESULT Bind_DissolveDescs()	override;

protected: 
	vector<class CState_Base*>	m_States;
	_uint						m_iState = { 0 };
	_uint						m_iPrevState = { 0 };

public:
	void Change_State(_uint eState, void* pArg = nullptr);
	_uint Get_PrevState() const {
		return m_iPrevState;
	}
	_uint Get_NowState() const {
		return m_iState;
	}

public:
	virtual void Hit(const ATTACKDESC& AttackDesc);
	virtual _int Take_Damage(const ATTACKDESC& AttackDesc);
	virtual void OnDeath();

protected:
	vector<class CWeapon*>		m_Weapons;

protected:
	void Tick_Weapons(_float fTimeDelta);
	void LateTick_Weapons(_float fTimeDelta);

public:
	void Update_Colliders();

protected:
	_bool				m_bAlphaBlend = { false };
	_bool				m_bOnField = { true };
	_bool				m_bAdjustNaviY = { true };
	

public:
	void Set_OnField(_bool _bVal) { m_bOnField = _bVal; }
	_bool Get_OnField() const { return m_bOnField; }

	void Set_AdjustNaviY(_bool bAdjust) {
		m_bAdjustNaviY = bAdjust;
	}
protected:
	CGameEffect::EFFECT_DESC	m_Effect_Desc = {};

public:
	CGameEffect::EFFECT_DESC* Get_EffectDescPtr() {
		return &m_Effect_Desc;
	}

protected:
	RIMLIGHTDESC		m_tRimLightDesc = {};
	HRESULT				Bind_RimLightDescs();

protected:
	CShader*		m_pShader = { nullptr };
	CModel*			m_pModel = { nullptr };
	CCollider*		m_pCollider = { nullptr };
	CCollider*		m_pHitBoxCollider = { nullptr };

public:
	void Free() override;
};

END
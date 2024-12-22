#pragma once

#include "GameObject.h"
#include "GameEffect.h"

BEGIN(Client)

class CDragon_Breath final : public CGameObject
{
private:
	CDragon_Breath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDragon_Breath(const CDragon_Breath& rhs);
	virtual ~CDragon_Breath() = default;

public:
	HRESULT Initialize(void* pArg)			override;
	void	PriorityTick(_float fTimeDelta)	override;
	void	Tick(_float fTimeDelta)			override;
	void	LateTick(_float fTimeDelta)		override;

	HRESULT OnEnter_Layer(void* pArg)		override;
	void	OnExit_Layer()					override;

private:
	void	OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)		override;
	void	End_Effect();


private:
	_int						m_iEffectIdx = { -1 };
	_float						m_fMoveDistance = { 0.f };

	CGameObject*				m_pTarget = { nullptr };
	CGameEffect::EFFECT_DESC	m_Effect_Desc = {};

	string						m_strEffectTag = { "" };

private:
	static _float		s_fMaxDist;
	static _float		s_fBreathAttack;
	static _float		s_fBreathSpeed;
	static _uint		s_iBreathStrength;

public:
	static void Add_Attack(_float fAttack) {
		s_fBreathAttack += fAttack;
	}

	static void Add_ShotSpeed(_float fSpeed) {
		s_fBreathSpeed += fSpeed;
	}

	static void Add_Strength() {
		s_iBreathStrength = (s_iBreathStrength + 1) % 3;
	}

private:
	CCollider* m_pCollider = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CDragon_Breath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
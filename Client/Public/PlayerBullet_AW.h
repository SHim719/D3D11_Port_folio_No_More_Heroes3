#pragma once

#include "Weapon.h"

BEGIN(Client)

class CPlayerBullet_AW final : public CGameObject
{
private:
	CPlayerBullet_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerBullet_AW(const CPlayerBullet_AW& rhs);
	virtual ~CPlayerBullet_AW() = default;

public:
	HRESULT Initialize(void* pArg)			override;
	void	PriorityTick(_float fTimeDelta)	override;
	void	Tick(_float fTimeDelta)			override;
	void	LateTick(_float fTimeDelta)		override;

	HRESULT OnEnter_Layer(void* pArg)		override;
	void	OnExit_Layer()					override;

private:
	void	OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)		override;

private:
	_int		m_iEffectIdx = { 0 };
	_float		m_fMoveDistance = { 0.f };
	string		m_strEffectTag = { "" };
	
private:
	static _float			s_fMaxDist;
	static _float			s_fBulletAttack;
	static _float			s_fBulletSpeed;
	static size_t			s_iStrength;
	static vector<string>	s_strEffectTag;

public:
	static void Add_Attack(_float fAttack) {
		s_fBulletAttack += fAttack;
	}

	static void Add_ShotSpeed(_float fSpeed) {
		s_fBulletSpeed += fSpeed;
	}

	static void Add_BulletStrength() {
		s_iStrength = min(s_iStrength + 1, 3);
	}

	static size_t Get_BulletStrength() {
		return s_iStrength;
	}

private:
	CCollider*	m_pCollider = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CPlayerBullet_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
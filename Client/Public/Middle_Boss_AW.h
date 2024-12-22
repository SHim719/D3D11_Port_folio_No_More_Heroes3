#pragma once

#include "Enemy_AW.h"
#include "GameEffect.h"
BEGIN(Client)

class CMiddle_Boss_AW final : public CEnemy_AW
{
private:
	CMiddle_Boss_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiddle_Boss_AW(const CMiddle_Boss_AW& rhs);
	virtual ~CMiddle_Boss_AW() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* pArg)				override;
	void	Tick(_float fTimeDelta)				override;

	void	Hit(const ATTACKDESC& AttackDesc)	override;

	HRESULT OnEnter_Layer(void* pArg)			override;
	void	OnExit_Layer()						override;

	void	Bind_KeyFrames()					override;
	void	OnCollisionEnter(CGameObject* pOther, CCollider* pCollider) override;
private:
	void Fire();

private:
	vector<class CEnemy_Bullet_AW*>		m_Bullets;
	size_t								m_iBulletIdx = { 0 };
	_float4								m_vShotOffset = { 0.f, 1.2f, 0.f, 0.f };
	CGameEffect::EFFECT_DESC			m_Effect_Desc = {};
private:
	HRESULT Ready_Components();
	HRESULT Ready_Bullets();

public:
	static CMiddle_Boss_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
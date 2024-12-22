#pragma once

#include "Enemy_AW.h"

#include "Last_Boss_Includes.h"
#include "GameEffect.h"

BEGIN(Client)

class CLast_Boss_AW final : public CEnemy_AW
{
private:
	CLast_Boss_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLast_Boss_AW(const CLast_Boss_AW& rhs);
	virtual ~CLast_Boss_AW() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* pArg)				override;
	void	Tick(_float fTimeDelta)				override;
	void	Hit(const ATTACKDESC& AttackDesc)	override;

	HRESULT OnEnter_Layer(void* pArg)			override;
	void	OnExit_Layer()						override;

	void	Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs) override;
	void	End_Cinematic();

public:
	void	Bind_KeyFrames()					override;

private:
	void	OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)	override;

private: /* Frame Function */
	void Fire();
	void Spark();

private:
	LAST_BOSS_STATE m_eCurState = { LAST_BOSS_STATE::STATE_END };

	_vector			m_PrevPos = {};
	_float			m_fShootTime = { 0.f };
	_int			m_iPrevRandom = { 0 };
	_bool 			m_bSpark = { false };
	_bool 			m_bPunch = { false };
	_bool 			m_bDown = { false };
	_bool			m_bAppearing = { true };

	_bool			m_bCinematic = { false };

	/* Bullet */
	vector<class CEnemy_Bullet_AW*>		m_Bullets;
	size_t								m_iBulletIdx = { 0 };
	_float4								m_vShotOffset = { 0.f, 1.2f, 0.f, 0.f };
	CGameEffect::EFFECT_DESC			m_Effect_Desc = {};

private:
	CRigidbody*		m_pRigidbody = { nullptr };

private:
	void Fly_High();


private:
	HRESULT Ready_Components();
	HRESULT Ready_Bullets();

private: /* Patten Function */
	void Patten_Root(_float _fTimeDelta);
	void Patten_Spark(_float _fTimeDelta);


	/* EFfect */
	void Landing_Effect();
	//void Appearing();

public:
	static CLast_Boss_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
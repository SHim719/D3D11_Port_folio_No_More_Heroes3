#pragma once

#include "Character.h"

BEGIN(Client)

class CJeane_AW final : public CCharacter
{
private:
	enum JEANE_STATE { JEANE_MOVE, JEANE_RUSH, JEANE_ATTACK };

private:
	CJeane_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJeane_AW(const CJeane_AW& rhs);
	virtual ~CJeane_AW() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	PriorityTick(_float fTimeDelta)	override;
	void	Tick(_float fTimeDelta)			override;
	void	LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

	HRESULT OnEnter_Layer(void* pArg)		override;
	void	OnExit_Layer()					override;

private:
	JEANE_STATE			m_eState = { JEANE_MOVE };
	class CEnemy_AW*	m_pAttackTarget = { nullptr };

	_float				m_fRushDist = { 22.f };
	_float				m_fRushMargin = { 1.5f };
	_float4				m_vOffset = {};

	_float				m_fIntervalAcc = { 0.f };
	_bool				m_bEnd_SpawnEffect = { false };
	_float				m_fSpawnEffectTimeAcc = { 0.f };
private:
	static _float		s_fAttackInterval;
	static _float		s_fAttack;
	static _int			s_iSpawnSequence;

public:
	static void Adjust_Interval(_float fValue) {
		s_fAttackInterval -= fValue;
	}

	static void Add_Attack(_float fValue) {
		s_fAttack += fValue;
	}


private:
	void Move(_float fTimeDelta);
	void Rush(_float fTimeDelta);
	void Attack(_float fTimeDelta);

	void Change_State(JEANE_STATE eState);

	_bool Check_Release_Target(CEnemy_AW* pEnemy);
private:
	void OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)	override;

private:
	HRESULT Ready_Components();

public:
	static CJeane_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
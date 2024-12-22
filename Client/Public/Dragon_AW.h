#pragma once

#include "Character.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CDragon_AW final : public CCharacter
{
private:
	CDragon_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDragon_AW(const CDragon_AW& rhs);
	virtual ~CDragon_AW() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	PriorityTick(_float fTimeDelta)	override;
	void	Tick(_float fTimeDelta)			override;
	void	LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

private:
	_float4							m_vOffset = {};
	_float							m_fIntervalAcc = { 0.f };

	CBone*							m_pShotPosBone = { nullptr };

	vector<class CDragon_Breath*>	m_Breathes;
	size_t							m_iBreathIdx = { 0 };

	_bool				m_bEnd_SpawnEffect = { false };
	_float				m_fSpawnEffectTimeAcc = { 0.f };

private:
	void Check_Fire(_float fTimeDelta);
	void Spawn_Breath();


private:
	static _float		s_fAttackInterval;
	static _float		s_fAttack;
	static _float		s_fShotDist;
	static _int			s_iSpawnSequence;

public:
	static void Adjust_Interval(_float fValue) {
		s_fAttackInterval -= fValue;
	}

	static void Add_Attack(_float fValue) {
		s_fAttack += fValue;
	}

private:
	HRESULT Ready_Components();
	HRESULT Ready_Breath();

public:
	static CDragon_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
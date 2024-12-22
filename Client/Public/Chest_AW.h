#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameEffect.h"
BEGIN(Client)

class CChest_AW final : public CGameObject
{
private:
	CChest_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChest_AW(const CChest_AW& rhs);
	virtual ~CChest_AW() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	PriorityTick(_float fTimeDelta)	override;
	void	Tick(_float fTimeDelta)			override;
	void	LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

	HRESULT OnEnter_Layer(void* pArg)		override;
	void	OnExit_Layer()					override;

	void OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)	override;

private:
	void Set_RandomReward();
	void Update_ChestParts();

private:
	class CChest_Up*			m_pChestUp = { nullptr };
	class CChest_Down*			m_pChestDown = { nullptr };
	class CChest_Ability_Font*	m_pChestFont = { nullptr };

	CHEST_REWARD_TYPE			m_eRewardType = {};
	_float						m_fAbilityVal = { 0.f };

private:
	CGameEffect::EFFECT_DESC	m_Effect_Desc = {};
	_int						m_iEffect_Index = { -1 };
private:
	CCollider*					m_pCollider = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CChest_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
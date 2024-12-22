#pragma once

#include "Character.h"

BEGIN(Client)

class CSign_AW final : public CGameObject
{
private:
	CSign_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSign_AW(const CSign_AW& rhs);
	virtual ~CSign_AW() = default;

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
	void OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)	override;

	void Select_RandAbility();

private:
	_int			m_iIdx = { 0 };

	wstring			m_wstrText = { L"" };

	ABILITY_TYPE	m_eAbilityType = {};
	_float			m_fAbilityVal = { 0.f };

	_bool			m_bFlying = { false };
private:
	CCollider*		m_pCollider = { nullptr };
	CVIBuffer*		m_pVIBuffer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CRigidbody*		m_pRigidbody = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CSign_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
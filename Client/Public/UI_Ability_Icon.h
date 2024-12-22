#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Ability_Icon final : public CUI
{
private:
	CUI_Ability_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Ability_Icon(const CUI_Ability_Icon& rhs);
	virtual ~CUI_Ability_Icon() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void Start_Rolling();
	void End_Rolling(size_t iIdx);

	void Release_Texture();
private:
	size_t		m_iNowIconIdx = { 0 };
	_float		m_fRatio = { 1.f };
	_float		m_fSpeed = { 10.f };

	_bool		m_bRolling = { false };

private:
	CVIBuffer*	m_pVIBuffer = { nullptr };

private:
	HRESULT Ready_Component();

public:
	static CUI_Ability_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END
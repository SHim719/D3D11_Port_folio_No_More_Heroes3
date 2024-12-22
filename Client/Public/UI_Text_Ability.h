#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Text_Ability final : public CUI
{
private:
	enum TEXT_STATE { SCALING, CHANGE_ALPHA };

private:
	CUI_Text_Ability(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Text_Ability(const CUI_Text_Ability& rhs);
	virtual ~CUI_Text_Ability() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

public:
	HRESULT OnEnter_Layer(void* pArg)	override;

private:
	TEXT_STATE	m_eState = { SCALING };

	wstring		m_wstrAbilityText = { L"" };

	_float		m_fTimeAcc = { 0.f };
	_float		m_fLifeTime = { 0.5f };

	_float2		m_vTextPos = {};
	_float		m_fNowScale = { 0.f };
	_float		m_fTargetScale = { 0.5f };

	_float		m_fStartY = { 0.f };
	_float		m_fTargetY = { 0.f };

public:
	static CUI_Text_Ability* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END
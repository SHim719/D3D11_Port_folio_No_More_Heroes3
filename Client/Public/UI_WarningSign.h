#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_WarningSign final : public CUI
{
private:
	CUI_WarningSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_WarningSign(const CUI_WarningSign& rhs);
	virtual ~CUI_WarningSign() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_float		m_fFlickerAcc = { 0.f };
	_float		m_fFlickerTime = { 0.2f };
	_float		m_fLifeTime = { 3.f };

	_bool		m_bRender = { true };

private:
	CVIBuffer*	m_pVIBuffer = { nullptr };

private:
	HRESULT		Ready_Component();


public:
	static CUI_WarningSign* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END
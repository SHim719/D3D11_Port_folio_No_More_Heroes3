#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Travis_Icon_AW final : public CUI
{
private:
	CUI_Travis_Icon_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Travis_Icon_AW(const CUI_Travis_Icon_AW& rhs);
	virtual ~CUI_Travis_Icon_AW() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_float		m_fStartY = { 600.f };
	_float		m_fFinalY = { 10.f };

private:
	CVIBuffer*	m_pVIBuffer = { nullptr };

private:
	HRESULT Ready_Component();

public:
	static CUI_Travis_Icon_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END
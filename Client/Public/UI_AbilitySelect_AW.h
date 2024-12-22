#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_AbilitySelect_AW final : public CUI
{
private:
	CUI_AbilitySelect_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_AbilitySelect_AW(const CUI_AbilitySelect_AW& rhs);
	virtual ~CUI_AbilitySelect_AW() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CVIBuffer* m_pVIBuffer = { nullptr };


private:
	HRESULT Ready_Component();

public:
	static CUI_AbilitySelect_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END
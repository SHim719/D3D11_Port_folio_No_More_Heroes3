#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LevelProgress_AW final : public CUI
{
private:
	CUI_LevelProgress_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LevelProgress_AW(const CUI_LevelProgress_AW& rhs);
	virtual ~CUI_LevelProgress_AW() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	class CUI_Progress_Empty_AW*	m_pUI_Progress_Empty = { nullptr };
	class CUI_Progress_Full_AW*		m_pUI_Progress_Full = { nullptr };
	class CUI_Travis_Icon_AW*		m_pUI_Travis_Icon = { nullptr };


public:
	static CUI_LevelProgress_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END
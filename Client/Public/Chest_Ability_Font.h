#pragma once

#include "Character.h"

BEGIN(Client)

class CChest_Ability_Font final : public CGameObject
{
private:
	CChest_Ability_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChest_Ability_Font(const CChest_Ability_Font& rhs);
	virtual ~CChest_Ability_Font() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

	void Set_RenderText(const wstring& wstrText) {
		m_wstrText = wstrText;
	}

private:
	wstring		m_wstrText = { L"" };

private:
	CVIBuffer*	m_pVIBuffer = { nullptr };
	CShader*	m_pShader = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CChest_Ability_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
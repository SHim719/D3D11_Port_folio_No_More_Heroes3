#pragma once

#include "Character.h"

BEGIN(Client)

class CChest_Down final : public CGameObject
{
private:
	CChest_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChest_Down(const CChest_Down& rhs);
	virtual ~CChest_Down() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	Tick(_float fTimeDelta)			override;
	HRESULT Render()						override;

private:
	CModel* m_pModel = { nullptr };
	CShader* m_pShader = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CChest_Down* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
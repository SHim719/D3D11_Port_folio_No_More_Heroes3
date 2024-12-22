#pragma once

#include "GameObject.h"

BEGIN(Client)

class CHP_Number final : public CGameObject
{
private:
	CHP_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHP_Number(const CHP_Number& rhs);
	virtual ~CHP_Number() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	LateTick(_float fTimeDelta)		override;
	HRESULT Render();

	void Update_Position(_fvector vPosition);
	void Update_RenderHp(_int iCurHp) {
		m_wstrCurHp = to_wstring(iCurHp);
	}

private:
	wstring		m_wstrCurHp = L"";

	_float4		m_vOffset = {};
	//_float4		m_vColor = {};

public:
	void Set_Offset(_fvector vOffset) {
		XMStoreFloat4(&m_vOffset, vOffset);
	}

private:
	CVIBuffer*	m_pVIBuffer = { nullptr };
	CShader*	m_pShader = { nullptr };


private:
	HRESULT Ready_Components();

public:
	static CHP_Number* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
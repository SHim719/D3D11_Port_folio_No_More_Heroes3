#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CSkyBox final : public CGameObject
{
private:
	CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void PriorityTick(_float fTimeDelta)	override;
	void Tick(_float fTimeDelta)			override;
	void LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

private:
	CShader*	m_pShader = { nullptr };
	CTexture*	m_pTexture = { nullptr };
	CVIBuffer*	m_pVIBuffer = { nullptr };

private:
	HRESULT Ready_Components(const wstring& wstrTextureTag);

public:
	static CSkyBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free() override;
};

END
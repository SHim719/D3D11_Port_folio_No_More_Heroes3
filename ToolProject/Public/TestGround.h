#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"


BEGIN(Tool)

class CTestGround final : public CGameObject
{
private:
	CTestGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestGround(const CTestGround& rhs);
	virtual ~CTestGround() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CShader*	m_pShader = nullptr;
	CVIBuffer*	m_pVIBuffer = nullptr;
	CTexture*	m_pTexture = nullptr;

private:
	HRESULT Ready_Components();


public:
	static CTestGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
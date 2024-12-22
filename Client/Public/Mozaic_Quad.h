#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMozaic_Quad final : public CGameObject
{
protected:
	CMozaic_Quad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMozaic_Quad(const CMozaic_Quad& rhs);
	virtual ~CMozaic_Quad() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	PriorityTick(_float fTimeDelta)	override;
	void	Tick(_float fTimeDelta)			override;
	void	LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

private:
	_float4x4 m_ViewMatrix, m_ProjMatrix;

private:
	CShader*	m_pShader = { nullptr };
	CVIBuffer*	m_pVIBuffer = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CMozaic_Quad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;

};

END
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CTravis_Status final : public CGameObject
{
private:
	CTravis_Status(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTravis_Status(const CTravis_Status& _rhs);
	virtual ~CTravis_Status() = default;

public:
	HRESULT Initialize(void* _pArg)				override;
	void	PriorityTick(_float _fTimeDelta)	override;
	void	Tick(_float _fTimeDelta)			override;
	HRESULT Render()							override;

private:
	HRESULT Ready_Components();

private:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	_float4x4	m_ViewMatrix = {};
	_float4x4	m_ProjMatrix = {};
	_float4		m_vCamPosition = { 0.f, 1.f, -2.f, 1.f };

public:
	static CTravis_Status* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END


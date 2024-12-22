#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Tool)

class CWater final : public CGameObject
{
protected:
	CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWater(const CWater& rhs);
	virtual ~CWater() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	PriorityTick(_float fTimeDelta)	override;
	void	Tick(_float fTimeDelta)			override;
	void	LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

	HRESULT Bind_ShaderResources()			override;

private:
	_float			m_fFlowTime = { 0.f };
	WATER_DESCS		m_tWater_Descs = {};

private:
	CShader*	m_pShader = { nullptr };
	CVIBuffer*	m_pVIBuffer = { nullptr };

private:
	void	Set_WaterDesc(WATER_DESCS& tWaterDesc)
	{
		m_tWater_Descs = tWaterDesc;
	}

	HRESULT Ready_Components();

public:
	static CWater* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;

	friend class CMap_Tool;
	friend class CCinema_Tool;
};

END
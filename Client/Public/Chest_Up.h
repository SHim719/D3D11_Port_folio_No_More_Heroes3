#pragma once

#include "Character.h"

BEGIN(Client)

class CChest_Up final : public CGameObject
{
private:
	enum CHEST_STATE { IDLE, OPENING};

private:
	CChest_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChest_Up(const CChest_Up& rhs);
	virtual ~CChest_Up() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void	Tick(_float fTimeDelta)			override;
	HRESULT Render()						override;

	void Init_Spawn();

	void Set_State_Opening() {
		m_eState = OPENING;
	}

private:
	_float			m_fAngle = { 0.f };
	_float			m_fRotationSpeed = { 60.f };
	CHEST_STATE		m_eState = { IDLE };



private:
	CModel*			m_pModel = { nullptr };
	CShader*		m_pShader = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CChest_Up* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
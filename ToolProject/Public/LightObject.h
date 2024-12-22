#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"
#include "Light.h"

BEGIN(Tool)

class CLightObject final : public CGameObject
{
private:
	CLightObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightObject(const CLightObject& rhs);
	virtual ~CLightObject() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void LateTick(_float fTimeDelta)	override;

private:
	CLight* m_pLight = { nullptr };

public:
	void Set_LightPosition(_fvector vPosition) {
		m_pLight->Set_LightPosition(vPosition);
	}
	void Set_LightRange(_float fRange) {
		m_pLight->Set_LightRange(fRange);
	}

	void Set_LightDesc(const LIGHT_DESC& LightDesc) {
		m_pLight->Set_LightDesc(LightDesc);
	}
	const LIGHT_DESC& Get_LightDesc() const {
		return *(m_pLight->Get_LightDesc());
	}

public:
	static CLightObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END


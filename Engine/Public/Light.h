#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer* pVIBuffer, _bool bPBR);

private:
	HRESULT Bind_Directional(CShader* pShader, _bool bPBR, OUT _uint& iPassIdx);
	HRESULT Bind_Point(CShader* pShader, _bool bPBR, OUT _uint& iPassIdx);
	HRESULT Bind_Spot(CShader* pShader, _bool bPBR, OUT _uint& iPassIdx);

private:
	LIGHT_DESC			m_LightDesc = {};

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

	void Set_LightDesc(const LIGHT_DESC& LightDesc) {
		m_LightDesc = LightDesc;
	}

	void Set_LightPosition(_fvector vPosition) {
		XMStoreFloat4(&m_LightDesc.vPosition, vPosition);
	}

	void Set_LightRange(_float fRange) {
		m_LightDesc.fRange = fRange;
	}

	void Set_LightDiffuse(_fvector vDiffuse) {
		XMStoreFloat4(&m_LightDesc.vDiffuse, vDiffuse);
	}


public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;

public:
	HRESULT Initialize();
	HRESULT Add_Light(class CLight* pLight);
	HRESULT Render(class CShader* pShader, class CVIBuffer* pVIBuffer, _bool bPBR);

	void Clear_Lights();

	void Set_DirectionalLight(const LIGHT_DESC& LightDesc);

	_vector Get_LightDir() const;

	void Set_Active_DirectionalLight(_bool bActive) {
		m_bActiveDirectional = bActive;
	}
private:
	list<CLight*>				m_Lights;

	CLight*						m_pDirectionalLight = { nullptr };
	_bool						m_bActiveDirectional = { true };


public:
	static CLight_Manager* Create();
	void Free() override;

};

END
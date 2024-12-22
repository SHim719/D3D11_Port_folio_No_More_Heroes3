#pragma once
#include "Base.h"

BEGIN(Engine)

class CFrustum : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

	_bool In_WorldFrustum(_fvector vWorldPos, _float fRadius);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

	_float4					m_vProjFrustumPoints[8] = {};
	_float4					m_vWorldFrustumPoints[8] = {};

	_float4					m_WorldFrustumPlane[6] = {};
public:
	static CFrustum* Create();
	void Free() override;
};

END

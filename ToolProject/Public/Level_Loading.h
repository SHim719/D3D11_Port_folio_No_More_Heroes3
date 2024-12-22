#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)
class CLevel_Loading final : public CLevel
{
public:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	HRESULT Initialize(LEVEL eNextLevel);
	void Tick(_float TimeDelta)				override;
	HRESULT Render()						override;

private:
	LEVEL			m_eNextLevel = LEVEL_END;
	class CLoader*	m_pLoader = nullptr;

public:
	static CLevel_Loading* Create(LEVEL eNextLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free()	override;
};
END

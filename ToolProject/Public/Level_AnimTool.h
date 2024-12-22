#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

class CLevel_AnimTool final : public CLevel
{
private:
	CLevel_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_AnimTool() = default;

public:
	HRESULT Initialize()			override;
	void Tick(_float fTimeDelta)	override;
	HRESULT Render()				override;

public:
	static CLevel_AnimTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END





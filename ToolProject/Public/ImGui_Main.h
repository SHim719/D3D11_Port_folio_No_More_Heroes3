#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END


BEGIN(Tool)
class CImGui_Main : public CBase
{
private:
	CImGui_Main();
	virtual ~CImGui_Main() = default;

	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ToolType eToolType);

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Render();

private:
	class CTool_Super*		m_pNowTool = { nullptr };

public:
	static CImGui_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ToolType eToolType);
	void Free() override;


};
END


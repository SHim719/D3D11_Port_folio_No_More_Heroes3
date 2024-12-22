#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Tool)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() { }

public:
	HRESULT Initialize();

	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	CGameInstance*			m_pGameInstance = nullptr;

	class CImGui_Main*		m_pImGui_Main = nullptr;

	 _float					m_fTimeAcc = { 0.0f };
	 _float					m_fFrameAcc = { 0.f };
	 _int					m_iFrameCnt = { 0 };

private:
	HRESULT	Ready_Prototype_Component();
	HRESULT Ready_Prototype_Shader();
	HRESULT Ready_Default();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END


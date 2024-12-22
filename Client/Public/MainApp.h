#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Client)

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

	 _float		m_fTimeAcc = { 0.0f };
	 _float     m_fFrameAcc = { 0.f };
	 _int       m_iFrameCnt = { 0 };

	 _bool		m_bActiveShaderOptions = { false };
private:
	HRESULT	Ready_Prototype_Component();
	HRESULT Ready_Prototype_Shader();
	HRESULT Ready_Prototype_UI();

	HRESULT Load_Effects();
	HRESULT Load_EffectTexture(const wstring& wstrTexturePath);
	HRESULT Load_EffectMesh(const wstring& wstrMeshPath);
	HRESULT Load_Travis_Effect(const wstring& wstrEffetPath);
	HRESULT Load_Blood_Effect(const wstring& wstrEffetPath);

private:
	class CTravis_Manager*	m_pTravis_Manager = { nullptr };
	class CImGui_Shader*	m_pImGui_Shader = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END


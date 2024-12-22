#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CImGui_Shader : public CBase
{
private:
	CImGui_Shader();
	virtual ~CImGui_Shader() = default;

	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Shader_Window();

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Render();

// Shader Option
private:
	CGameInstance* m_pGameInstance = { nullptr };

	_bool	m_bSSAO = { false };
	_float	m_fSSAOPower = { 1.f };

	_bool	m_bSSR = { false };

	_bool	m_bDOF = { false };
	_float	m_fDOFDistMin = { 10.f };
	_float  m_fDOFDistMax = { 200.f };

	_float  m_fBloomStrength = { 0.7f };
	_float  m_fBloomThreshold = { 1.f };	


public:
	static CImGui_Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free()	override;
};

END


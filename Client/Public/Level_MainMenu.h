#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_MainMenu final : public CClientLevel
{
private:
	CLevel_MainMenu(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_MainMenu() = default;

public:
	virtual HRESULT Initialize()				override;
	virtual void	Tick(_float _fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	virtual void	OnExitLevel()				override;

#ifdef _DEBUG

private:
	class CFree_Camera* m_pDebugFreeCamera = { nullptr };

private:
	HRESULT Ready_Camera();

#endif // _DEBUG

private:
	HRESULT Ready_Map();
	HRESULT Ready_Player();
	HRESULT Ready_Fog();
	HRESULT Ready_UI();
	
public:
	static CLevel_MainMenu* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
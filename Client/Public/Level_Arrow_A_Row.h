#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_Arrow_A_Row final : public CClientLevel
{
private:
	CLevel_Arrow_A_Row(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Arrow_A_Row() = default;

public:
	virtual HRESULT Initialize()						override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual HRESULT Render()							override;

private:
	virtual void	OnExitLevel()						override;

#ifdef _DEBUG

private:
	class CFree_Camera* m_pDebugFreeCamera = { nullptr };

private:
	HRESULT Ready_Camera();

#endif // _DEBUG

private:
	HRESULT Ready_Map();
	HRESULT Ready_Light();

public:
	static CLevel_Arrow_A_Row* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
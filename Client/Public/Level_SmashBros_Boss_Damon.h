#pragma once

#include "ClientLevel.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CLevel_SmashBros_Boss_Damon final : public CClientLevel
{
private:
	CLevel_SmashBros_Boss_Damon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_SmashBros_Boss_Damon() = default;

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
	HRESULT Ready_Player();
	HRESULT Ready_Monster();
	HRESULT Ready_UI();

public:
	static _bool s_bLevelCleared;

public:
	static CLevel_SmashBros_Boss_Damon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
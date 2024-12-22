#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_Boss_SonicJuice final : public CClientLevel
{
private:
	CLevel_Boss_SonicJuice(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Boss_SonicJuice() = default;

public:
	virtual HRESULT Initialize()						override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual HRESULT Render()							override;

private:
	virtual void	OnExitLevel()						override;

	HRESULT Start_Game();

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
	HRESULT Ready_Cinematic();

public:
	static _bool IS_BOSS_DEAD;

private:
	_float m_fTimeAcc_LevelClear = { 0.f };
	_float m_fTimeAcc_FinalDelay = { 0.f };
	_bool m_bLevelCleard = { false };


public:
	static CLevel_Boss_SonicJuice* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
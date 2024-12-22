#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_Boss_BlackHole_Space final : public CClientLevel
{
private:
	CLevel_Boss_BlackHole_Space(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Boss_BlackHole_Space() = default;

public:
	virtual HRESULT Initialize()				override;
	virtual void	Tick(_float _fTimeDelta)	override;
	virtual HRESULT Render()					override;

#ifdef _DEBUG

private:
	class CFree_Camera* m_pDebugFreeCamera = { nullptr };

private:
	HRESULT Ready_Camera();
#endif // _DEBUG


public:
	virtual void OnExitLevel() override;



private:
	HRESULT Ready_SkyBox();
	HRESULT Ready_Lights();
	HRESULT Ready_Map();
	HRESULT Ready_Player();
	HRESULT Ready_Monster();
	HRESULT Ready_Effects();
	HRESULT Ready_UI();

public:
	static _bool IS_BOSS_DEAD;

private:
	_float m_fTimeAcc_LevelClear = { 0.f };
	_float m_fTimeAcc_FinalDelay = { 0.f };

	_bool m_bLevelCleard = { false };

public:
	static CLevel_Boss_BlackHole_Space* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
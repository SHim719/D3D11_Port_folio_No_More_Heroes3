#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_Battle_UFO_Inside final : public CClientLevel
{
private:
	CLevel_Battle_UFO_Inside(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Battle_UFO_Inside() = default;

public:
	virtual HRESULT Initialize()				override;
	virtual void	Tick(_float _fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	virtual void	OnExitLevel()					override;

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
	HRESULT Ready_Effects();
	HRESULT Ready_UI();

private:
	class CMonster_Manager* m_pMonster_Manager = { nullptr };

private:
	_float m_fTimeAcc_LevelClear = { 0.f };
	_float m_fTimeAcc_FinalDelay = { 0.f };
	_bool m_bLevelCleard = { false };

public:
	static CLevel_Battle_UFO_Inside* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
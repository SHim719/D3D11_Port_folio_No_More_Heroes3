#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_World_UFO final : public CClientLevel
{
private:
	CLevel_World_UFO(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_World_UFO() = default;

public:
	virtual HRESULT Initialize()				override;
	virtual void	Tick(_float _fTimeDelta)	override;
	virtual HRESULT Render()					override;

public:
	virtual void OnExitLevel() override;

#ifdef _DEBUG

private:
	class CFree_Camera* m_pDebugFreeCamera = { nullptr };

private:
	HRESULT Ready_Camera();

#endif // _DEBUG

private:
	HRESULT Ready_Map();
	HRESULT Ready_Player();
	HRESULT Ready_Effects();
	HRESULT Ready_UI();

private:
	void Check_Level_Change();
	void Bind_Trigger_Functions();
	void Open_Wesn_UI(wstring wstrTriggerName);

private:
	class CTrigger_Manager* m_pTriggerManager = { nullptr };

private:
	_bool m_bGoLevelBattleUFOInside = { false };
	_bool m_bGoLevelBossBlackHoleGround = { false };

private:
	void Go_Level_Battle_UFO_Inside()		{ m_bGoLevelBattleUFOInside = true; }
	void Go_Level_Boss_BlackHole_Ground()	{ m_bGoLevelBossBlackHoleGround = true; }

public:
	static CLevel_World_UFO* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
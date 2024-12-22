#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_World_City final : public CClientLevel
{
private:
	CLevel_World_City(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_World_City() = default;

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
	HRESULT Ready_NPC();
	HRESULT Ready_Effects();
	HRESULT Ready_UI();

private:
	void Check_Level_Change();
	void Bind_Trigger_Functions();

private:	// Trigger Funcs
	void Go_Level_Battle_UFO_Outside() { m_bGoLevelBattelUFOOutside = true; }
	void Go_Level_World_Lab_B1F();
	void Go_Level_World_TravisHouse_1F();
	void Go_Level_World_TravisHouse_2F();
	void Go_Level_World_City_Toilet() { m_bGoLevelWorldToilet = true; }
	void Go_Level_Battle_Island() { m_bGoLevelBattleIsland = true; }
	void Go_Level_Wolrd_TrashCollector() { m_bGoLevelWorldTrashCollector = true; }
	void Go_Level_Arrow_A_Row() { m_bGoLevelArrowARow = true; }
	void Go_Level_Mario() { m_bGoLevelMario = true; }
	void Activate_Interaction_With_ToiletNPC(wstring wstrTriggerName);
	void Activate_Interaction_With_BattleQuestNPC(wstring wstrTriggerName);
	void Activate_Interaction_With_TrashQuestNPC(wstring wstrTriggerName);

private:
	class CTrigger_Manager* m_pTriggerManager = { nullptr };
	class CNPC_Manager* m_pNPC_Manager = { nullptr };

private:
	_bool m_bGoLevelBattelUFOOutside = { false };
	_bool m_bGoLevelWorldLabB1F = { false };
	_bool m_bGoLevelWorldTravisHouse1F = { false };
	_bool m_bGoLevelWorldTravisHouse2F = { false };
	_bool m_bGoLevelWorldToilet = { false };
	_bool m_bGoLevelBattleIsland = { false };
	_bool m_bGoLevelWorldTrashCollector = { false };
	_bool m_bGoLevelArrowARow= { false };
	_bool m_bGoLevelMario= { false };

	_bool m_bPhoneCall = { false };

private:
	_float m_fTimeAcc = { 0.f };
	_bool m_bFlowControl = { false };

public:
	static CLevel_World_City* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
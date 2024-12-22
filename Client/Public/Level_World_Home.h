#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_World_Home final : public CClientLevel
{
private:
	CLevel_World_Home(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_World_Home() = default;

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

private:
	HRESULT Ready_Map();
	HRESULT Ready_Player();
	HRESULT Ready_Effects();
	HRESULT Ready_UI();
	HRESULT Ready_NPC();

private:
	void Check_Level_Change();
	void Bind_Trigger_Functions();

private:
	class CTrigger_Manager* m_pTriggerManager = { nullptr };

private:
	_bool m_bGo_Level_Battle_Motel_Front = { false };
	_bool m_bGo_Level_World_City = { false };
	_bool m_bGo_Level_World_Home_Lab = { false };

public:
	void Go_Level_Battle_Motel_Front();
	void Go_Level_World_City();
	void Go_Level_World_Home_Lab();
	
public:
	static CLevel_World_Home* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
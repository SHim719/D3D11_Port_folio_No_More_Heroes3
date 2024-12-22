#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_World_Home_Lab final : public CClientLevel
{
private:
	CLevel_World_Home_Lab(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_World_Home_Lab() = default;

public:
	virtual HRESULT Initialize()				override;
	virtual void	Tick(_float _fTimeDelta)	override;
	virtual HRESULT Render()					override;

	void OnExitLevel()							override;

#ifdef _DEBUG

private:
	class CFree_Camera* m_pDebugFreeCamera = { nullptr };

private:
	HRESULT Ready_Camera();

#endif // _DEBUG

private:
	HRESULT Ready_Map();
	HRESULT Ready_Player();
	HRESULT Ready_UI();

private:
	void Check_Level_Change();
	void Bind_Trigger_Functions();
	void Open_Wesn_UI(wstring wstrTriggerName);

private:
	class CTrigger_Manager* m_pTriggerManager = { nullptr };
	_int					m_iEffectIndex = { -1 };
	_bool					m_bEffectStarted = { false };

private:
	static _bool s_bFirstEnterWesn;
	_bool m_bGoLevelWorldCity = { false };

private:
	void Go_Level_World_City();

public:
	static CLevel_World_Home_Lab* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
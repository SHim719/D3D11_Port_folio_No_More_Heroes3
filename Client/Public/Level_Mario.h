#pragma once

#include "ClientLevel.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CLevel_Mario final : public CClientLevel
{
private:
	CLevel_Mario(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Mario() = default;

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
	HRESULT Ready_Background();

private:
	void Check_Level_Change();
	void Bind_Trigger_Functions();

private:
	void Killer_Shot(_float _fTimeDelta);
	void Togezo_WaveGame(_float _fTimeDelta);
	void Start_EndCutScene();

private:
	class CTrigger_Manager* m_pTriggerManager = { nullptr };

private:
	/* Killer */
	vector<class CKiller*> m_vecKiller;
	_float	m_fCreKillerTime= { 0.f };
	_int	m_iKillerIndex	= { 0 };
	_bool	m_bCreKillerEnd	= { false };
	/* Goomba */
	vector<class CGoomba*> m_vecGoomba;
	_int m_iGoomba = { 0 };
	/* Togezo */
	vector<class CTogezo*> m_vecTogezo;
	_float m_fCreTogezoTime = { 0.f };
	_int m_iTogezoIndex		= { 0 };
	_bool m_bWave			= { false };
	_bool m_bCreTogezoEnd	= { false };

	/* Effect */
	_int	m_iEffectIndex = { -1 };

	_bool m_bGoLevel_SmashBros = { false };

	_bool m_bLevelClear = { false };

private:	// Trigger Funcs
	void Go_Level_SmashBros() { m_bGoLevel_SmashBros = true; }

public:
	static CLevel_Mario* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
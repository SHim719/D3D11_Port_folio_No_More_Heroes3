#pragma once

#include "ClientLevel.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CLevel_World_City_Toilet final : public CClientLevel
{
private:
	CLevel_World_City_Toilet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_World_City_Toilet() = default;

public:
	virtual HRESULT Initialize()			override;
	virtual void Tick(_float _fTimeDelta)	override;
	virtual HRESULT Render()				override;

#ifdef  _DEBUG
	
private:
	class CFree_Camera* m_pDebugFreeCamera = { nullptr };

private:
	HRESULT Ready_Camera();

#endif

private:
	HRESULT Ready_Map();
	HRESULT Ready_Player();
	HRESULT Ready_Effects();
	HRESULT Ready_UI();

public:
	static _bool IS_TOILET_END;
	static _bool IS_MINIGAME_END;

private:
	CGameObject* m_pMozaic = { nullptr };

public:
	static CLevel_World_City_Toilet* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END
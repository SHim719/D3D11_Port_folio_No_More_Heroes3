#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCinema_Damon : public CCinemaActor
{
private:
	CCinema_Damon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Damon(const CCinema_Damon& _rhs);
	virtual ~CCinema_Damon() = default;

private:
	HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;

	_bool Bind_CinemaEtcFunc() override;

private:
	void Damon_Disappear();

public:
	void Get_Off();

private:
	_bool m_bDisappear = { false };

public:
	static CCinema_Damon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;
};

END
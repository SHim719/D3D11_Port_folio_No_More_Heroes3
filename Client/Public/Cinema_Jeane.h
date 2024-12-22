#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCinema_Jeane : public CCinemaActor
{
private:
	CCinema_Jeane(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Jeane(const CCinema_Jeane& _rhs);
	virtual ~CCinema_Jeane() = default;

private:
	HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;
	HRESULT Render() override;

	_bool Bind_CinemaEtcFunc() override;

private:
	void Jeane_Disappear();

public:
	void Get_Off();

private:
	_bool m_bDisappear = { false };

public:
	static CCinema_Jeane* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;
};

END
#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCinema_Travis_Motel : public CCinemaActor
{
private:
	CCinema_Travis_Motel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Travis_Motel(const CCinema_Travis_Motel& _rhs);
	virtual ~CCinema_Travis_Motel() = default;

private:
	HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;

	_bool Bind_CinemaEtcFunc() override;

public:
	void Get_Off();

private:
	_bool m_bDisappear = { false };

public:
	static CCinema_Travis_Motel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;
};

END
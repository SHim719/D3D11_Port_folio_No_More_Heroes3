#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCinema_Mbone : public CCinemaActor
{
private:
	CCinema_Mbone(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Mbone(const CCinema_Mbone& _rhs);
	virtual ~CCinema_Mbone() = default;

private:
	HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;

	_bool Bind_CinemaEtcFunc() override;

private:
	void Mbone_Disappear();

public:
	void Get_Off();

private:
	_bool m_bDisappear = { false };

public:
	static CCinema_Mbone* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;
};

END
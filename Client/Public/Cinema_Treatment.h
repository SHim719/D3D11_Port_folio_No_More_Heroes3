#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCinema_Treatment : public CCinemaActor
{
private:
	CCinema_Treatment(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Treatment(const CCinema_Treatment& _rhs);
	virtual ~CCinema_Treatment() = default;

private:
	HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;

	_bool Bind_CinemaEtcFunc() override;

private:
	void Treatment_Disappear();

public:
	void Get_Off();

private:
	_bool m_bDisappear = { false };

public:
	static CCinema_Treatment* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;
};

END
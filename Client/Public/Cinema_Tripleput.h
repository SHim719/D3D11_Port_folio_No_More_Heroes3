#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCinema_Tripleput : public CCinemaActor
{
private:
	CCinema_Tripleput(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Tripleput(const CCinema_Tripleput& _rhs);
	virtual ~CCinema_Tripleput() = default;

private:
	HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;

	_bool Bind_CinemaEtcFunc() override;

private:
	void Tripleput_Disappear();

public:
	void Get_Off();

private:
	_bool m_bDisappear = { false };

public:
	static CCinema_Tripleput* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;
};

END
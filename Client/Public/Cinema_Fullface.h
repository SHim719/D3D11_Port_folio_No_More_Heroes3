#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCinema_Fullface : public CCinemaActor
{
private:
	CCinema_Fullface(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Fullface(const CCinema_Fullface& _rhs);
	virtual ~CCinema_Fullface() = default;

private:
	HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta) override;

	_bool Bind_CinemaEtcFunc() override;

private:
	void Fullface_Disappear();

public:
	void Get_Off();

private:
	_bool m_bDisappear = { false };

public:
	static CCinema_Fullface* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;
};

END
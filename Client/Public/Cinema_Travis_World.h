#pragma once
#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END


BEGIN(Client)

class CCinema_Travis_World : public CCinemaActor
{
private:
	CCinema_Travis_World(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Travis_World(const CCinema_Travis_World& _rhs);
	virtual ~CCinema_Travis_World() = default;

private:
	HRESULT Initialize(void* _pArg)		override;
	void	Tick(_float _fTimeDelta)	override;

public:
	void Get_Off();

public:
	static CCinema_Travis_World* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END
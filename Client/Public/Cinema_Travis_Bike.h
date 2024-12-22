#pragma once

#include "CinemaActor.h"

BEGIN(Engine)
class CBone;
END


BEGIN(Client)

class CCinema_Travis_Bike : public CCinemaActor
{
private:
	CCinema_Travis_Bike(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinema_Travis_Bike(const CCinema_Travis_Bike& _rhs);
	virtual ~CCinema_Travis_Bike() = default;

private:
	HRESULT Initialize(void* _pArg)		override;
	void	Tick(_float _fTimeDelta)	override;

	_bool	Bind_CinemaEtcFunc()		override;
private:
	class CCinema_Travis_World*		m_pTravis		= { nullptr };
	CBone*							m_pBone_Tire_F	= { nullptr };
	CBone*							m_pBone_Tire_R	= { nullptr };
	CBone*							m_pBone_Tire_L	= { nullptr };
	_float							m_fTireRotation = { 0.f };

	_bool							m_bGetOff = { false };
	_bool							m_bDisappear = { false };

private:
	HRESULT Ready_TireBones();
	void	Rotate_Tires(_float fTimeDelta);
	void	Get_Off();
	void	Bike_Disappear();

public:
	void Set_CinemaTravis(CCinema_Travis_World* pTravis) {
		m_pTravis = pTravis;
	}


public:
	static CCinema_Travis_Bike* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END
#pragma once

#include "Travis_Bike_State_Base.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CTravis_Bike_State_Ride final : public CTravis_Bike_State_Base
{
private:
	enum ANIM_DIR { LEFT, NEUTRAL, RIGHT };

private:
	CTravis_Bike_State_Ride(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTravis_Bike_State_Ride() = default;

private:
	HRESULT Initialize(void* _pArg)					override;
	void	OnState_Start(void* _pArg = nullptr)	override;
	void	Update(_float _fTimeDelta)				override;
	void	Late_Update(_float _fTimeDelta)			override;
	void	OnState_End()							override;

private:
	void Rotate_Tire(_float _fTimeDelta);

private:
	CRigidbody*		m_pRigidbody = { nullptr };
	CBone*			m_pBone_Tire_R = { nullptr };
	CBone*			m_pBone_Tire_L = { nullptr };

private:
	ANIM_DIR		m_eCurDir = { NEUTRAL };
	_bool			m_bDrifting = { false };
	_bool			m_bAccel = { false };

private:
	_float			m_fTireRotation = { 0.f };
	_float			m_fMaxTireRotSpeed = { 1000.f };
	_float			m_fAccel = { 5.f };
	_float			m_fOriginRotAccel = { 300.f };
	_float			m_fCurRotAccel = { };

public:
	static CTravis_Bike_State_Ride* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	void Free() override;

};

END
#pragma once

#include "Travis_Bike_State_Base.h"

BEGIN(Client)

class CTravis_Bike_State_NonRide : public CTravis_Bike_State_Base
{
protected:
	CTravis_Bike_State_NonRide(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTravis_Bike_State_NonRide() = default;

public:
	HRESULT Initialize(void* _pArg)					override;
	void	OnState_Start(void* _pArg = nullptr)	override;
	void	Update(_float _fTimeDelta)				override;
	void	Late_Update(_float _fTimeDelta)			override;
	void	OnState_End()							override;

public:
	static CTravis_Bike_State_NonRide* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	void Free() override;

};

END
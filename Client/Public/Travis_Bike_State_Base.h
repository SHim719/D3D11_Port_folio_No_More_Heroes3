#pragma once

#include "State_Base.h"
#include "Travis_Bike.h"

BEGIN(Client)

class CTravis_Bike_State_Base abstract : public CState_Base
{
protected:
	CTravis_Bike_State_Base(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTravis_Bike_State_Base() = default;

public:
	HRESULT Initialize(void* _pArg)					override;
	void	OnState_Start(void* _pArg = nullptr)	override;
	void	Update(_float _fTimeDelta)				override;
	void	Late_Update(_float _fTimeDelta)			override;
	void	OnState_End()							override;

public:
	void	OnHit(const ATTACKDESC& _tagAtkDesc)	override;

protected:
	void Key_Input(_float _fTimeDelta);

protected:
	CTravis_Bike* m_pOwner = { nullptr };

public:
	void Free() override;

};

END
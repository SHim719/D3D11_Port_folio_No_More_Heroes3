#pragma once

#include "Travis_World_State_Base.h"

BEGIN(Client)

class CTravis_World_State_Riding_Out : public CTravis_World_State_Base
{
private:
	CTravis_World_State_Riding_Out(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTravis_World_State_Riding_Out() = default;

public:
	virtual void	OnState_Start(void* _pArg = nullptr)	override;
	virtual void	OnState_End()							override;
	virtual void	Update(_float _fTimeDelta)				override;
	virtual void	Late_Update(_float _fTimeDelta)			override;

public:
	static CState_Base* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;

};

END
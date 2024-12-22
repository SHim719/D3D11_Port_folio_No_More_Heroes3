#pragma once

#include "Travis_World_State_Base.h"

BEGIN(Client)

class CTravis_World_State_Riding : public CTravis_World_State_Base
{
private:
	enum ANIM_DIR { LEFT, NEUTRAL, RIGHT };

private:
	CTravis_World_State_Riding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTravis_World_State_Riding() = default;

public:
	virtual void	OnState_Start(void* _pArg = nullptr)	override;
	virtual void	Update(_float _fTimeDelta)				override;

private:
	ANIM_DIR	m_eCurDir = { NEUTRAL };


public:
	static CState_Base* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;

};

END
#pragma once

#include "Various_Camera_State_Base.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CVarious_Camera_State_Cinematic_Jo final : public CVarious_Camera_State_Base
{
private:
	CVarious_Camera_State_Cinematic_Jo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CVarious_Camera_State_Cinematic_Jo() = default;

public:
	virtual HRESULT Initialize(void* _pArg)			override;
	virtual void	EnterState(void* _pArg)			override;
	virtual void	Update(_float _fTimeDelta)		override;
	virtual void	Late_Update(_float _fTimeDelta)	override;
	virtual void	ExitState()						override;


public:
	static CVarious_Camera_State_Cinematic_Jo* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;

};

END
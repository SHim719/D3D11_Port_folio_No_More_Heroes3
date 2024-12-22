#pragma once

#include "Various_Camera_State_Base.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CVarious_Camera_State_AW_Default final : public CVarious_Camera_State_Base
{
private:
	CVarious_Camera_State_AW_Default(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CVarious_Camera_State_AW_Default() = default;

public:
	virtual HRESULT Initialize(void* _pArg)			override;
	virtual void	EnterState(void* _pArg)			override;
	virtual void	Update(_float _fTimeDelta)		override;
	virtual void	Late_Update(_float _fTimeDelta)	override;
	virtual void	ExitState()						override;

private:
	_float4		m_vStartPos = {};
	_float4		m_vStartLook = {};
	_float4		m_vTargetPos = {};
	_float4		m_vTargetLook = {};

	_bool		m_bLerping = { false };
	_float		m_fTimeAcc = { 0.f };
	_float		m_fLerpTime = { 2.f };


public:
	static CVarious_Camera_State_AW_Default* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;

};

END
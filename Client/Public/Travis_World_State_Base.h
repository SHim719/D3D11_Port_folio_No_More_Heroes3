#pragma once

#include "State_Base.h"
#include "Travis_World_Enums.h"

#define CHECK_CONTROL_LOCK		\
if(m_pOwner->Get_ControlLock())	\
	return;						\

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CTravis_World_State_Base abstract : public CState_Base
{
public:
	typedef struct tagStateChangeDesc {
		CGameObject* pObject = { nullptr };
		_uint iLastAnimIndex = { 0 };

	}STATE_CHANGE_DESC, SCD;

protected:
	CTravis_World_State_Base(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTravis_World_State_Base() = default;

public:
	virtual HRESULT Initialize(void* _pArg)					override;
	virtual void	OnState_Start(void* _pArg = nullptr)	override;
	virtual void	Priority_Update(_float _fTimeDelta)		override;
	virtual void	Update(_float _fTimeDelta)				override;
	virtual void	Late_Update(_float _fTimeDelta)			override;
	virtual void	OnState_End()							override;

public:
	virtual void	OnHit(const ATTACKDESC& _tagAtkDesc)	override;

protected:
	virtual void Key_Input(_float _fTimeDelta);
	virtual void Key_Input_TC(_float _fTimeDelta);

protected:
	class CTravis_World* m_pOwner = { nullptr };
	CModel* m_SubModels[(_uint)WORLDTRAVIS_SUBMODELLIST::SUBMODELLIST_END] = {};
	_bool m_bIsRunning = { false };
	static _bool s_bConvertable;

protected:
	_bool Check_AnimProgress(_float _fRatio);
	_float Check_AnimPlayRatio();

public:
	virtual void Free() override;

};

END
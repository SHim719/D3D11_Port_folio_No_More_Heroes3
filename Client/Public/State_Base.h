#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

BEGIN(Client)

class CState_Base abstract : public CBase
{
protected:
	CState_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CState_Base() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void OnState_Start(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual void OnState_End();

	virtual void OnHit(const ATTACKDESC& AttackDesc);

	void Set_Navigation(CNavigation* pNavigation) {
		m_pNavigation = pNavigation;
	}

	/* Root Animation true || false */
	void Set_Root(_bool _ChangeRoot) {
		m_bRoot = _ChangeRoot;
	};

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CGameInstance*			m_pGameInstance = { nullptr };

protected:
	CModel*					m_pModel = { nullptr };
	CTransform*				m_pOwnerTransform = { nullptr };
	CNavigation*			m_pNavigation = { nullptr };

protected:
	_float					m_fPatternTime = { 0.f };
	_float4x4				m_OffsetMatrix = {};

	vector<ATTACKDESC>		m_AttackDescs; // ¹«±â ÀÎµ¦½º + ATTACKDESC
	
	size_t					m_iNowAttackIdx = { 0 };

	_bool	m_bRoot = { false };

protected:
	virtual void Init_AttackDesc();
	virtual void Reset_AttackDesc();

	void Reset_AttackIdx() {
		m_iNowAttackIdx = 0;
	}

	void Setup_RootRotation();

public:
	void Free() override;
};

END


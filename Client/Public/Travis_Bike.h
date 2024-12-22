#pragma once

#include "Character.h"

#include "Travis_Bike_Enums.h"
//#include "Travis_Bike_Structs.h"



BEGIN(Client)

class CTravis_Bike final : public CCharacter
{
private:
	CTravis_Bike(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTravis_Bike(const CTravis_Bike& _rhs);
	virtual ~CTravis_Bike() = default;

public:
	 HRESULT Initialize_Prototype()				override;
	 HRESULT Initialize(void* _pArg)			override;
	 void	PriorityTick(_float _fTimeDelta)	override;
	 void	Tick(_float _fTimeDelta)			override;
	 void	LateTick(_float _fTimeDelta)		override;
	 HRESULT Render()							override;
	 HRESULT Render_ShadowDepth()				override;

	 HRESULT OnEnter_Layer(void* pArg)			override;

	 void    Get_Off();
	 void	 Set_Active_Colliders_By_RidingState(_bool bActive);
	 void	 Update_NonRide_Colliders();

private:
	void Bind_KeyFrames()						override;
	void Bind_KeyFrameEffects()					override;
	void Bind_KeyFrameSounds()					override;

private:
	void OnCollisionEnter(CGameObject* _pOther, CCollider* pCollider)	override;
	void OnCollisionExit(CGameObject* _pOther, CCollider* pCollider)	override;

private:
	void Compute_YPos();

private:
	CRigidbody*		m_pRigidbody = { nullptr };
	CNavigation*	m_pNavigation = { nullptr };
	CCollider*		m_pRidingCollider = { nullptr };
	CCollider*		m_pBikeCollider = { nullptr };


private:
	HRESULT Bind_ShaderResources()	override;
	HRESULT Ready_Components();
	HRESULT Ready_States();

public:
	static CTravis_Bike* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END


#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidbody final : public CComponent
{
public:
	enum FORCEMODE { Force, Acceleration, Impulse, VelocityChange };

	typedef struct tagRigidbodyDesc
	{
		class CTransform*	pOwnerTransform = { nullptr };
		_float				fMass = { 1.f };
		_float3				vMaxVelocity = { 0.f, 0.f, 0.f };
		_float				fFrictionScale = { 1.f };

		_float				fMaxAngularVelocity = {};
		_float				fAngularFrictionScale = { 1.f };
		_bool				bMoveLocal = { true };
	}RIGIDBODY_DESC;


private:
	CRigidbody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRigidbody(const CRigidbody& _rhs);
	virtual ~CRigidbody() = default;

	HRESULT Initialize(void* _pArg)		override;

public:
	void Update(_float fTimeDelta);

private:
	class CTransform* m_pOwnerTransform = { nullptr };
	class CNavigation* m_pNavigation = { nullptr };

private:
	_float		m_fMass = { 1.f };

	_float3		m_vVelocity = {};
	_float3		m_vMaxVelocity = { 0.f, 0.f, 0.f };
	_float3		m_vLinearAccel = {};
	_float		m_fFrictionScale = { 1.f };

	_float4		m_vRotAxis = {};
	_float		m_fAngularVelocity = {};
	_float		m_fAngularAccel = {};
	_float		m_fMaxAngularVelocity = { 60.f};
	_float		m_fAngularFrictionScale = { 5.f };

	_bool		m_bMoveLocal = { true };
public:
	void Change_Navigation(CNavigation* pNavigation);

public:
	void Add_Force(_fvector vForce, FORCEMODE eMode);
	void Add_Torque(_fvector vAxis, _float fTorque, FORCEMODE eMode);


	_vector Get_Velocity() const {
		return XMLoadFloat3(&m_vVelocity);
	}

	_vector Get_MaxVelocity() const {
		return XMLoadFloat3(&m_vMaxVelocity);
	}


	_float Get_VelocityLength() const {
		return XMVector3Length(Get_Velocity()).m128_f32[0];
	}

	void Set_MaxVelocity(_fvector vMaxVelocity) {
		XMStoreFloat3(&m_vMaxVelocity, vMaxVelocity);
	}

	void Set_MaxAngularVelocity(_float fAngularVelocity) {
		m_fMaxAngularVelocity = fAngularVelocity;
	}

	void Set_FrictionScale(_float fScale) {
		m_fFrictionScale = fScale;
	}

	void Set_AngularFrictionScale(_float fScale) {
		m_fAngularFrictionScale = fScale;
	}

	void Set_MoveLocal(_bool b) {
		m_bMoveLocal = b;
	}
	
	void Set_Velocity(_fvector vVelocity) {
		XMStoreFloat3(&m_vVelocity, vVelocity);
	}

	void Set_AngularVelocity(_float fVelocity) {
		m_fAngularVelocity = fVelocity;
	}

public:
	static CRigidbody* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CComponent* Clone(void* _pArg) override;
	void Free() override;

};

END
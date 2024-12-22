#include "Rigidbody.h"
#include "GameInstance.h"

CRigidbody::CRigidbody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent{ _pDevice,_pContext }
{
}

CRigidbody::CRigidbody(const CRigidbody& _rhs)
	: CComponent{ _rhs }
{
}


HRESULT CRigidbody::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	RIGIDBODY_DESC* pDesc = (RIGIDBODY_DESC*)_pArg;

	if (nullptr == pDesc->pOwnerTransform)
		return E_FAIL;

	m_pOwnerTransform = pDesc->pOwnerTransform;
	Safe_AddRef(m_pOwnerTransform);

	m_fMass = pDesc->fMass;
	m_fMaxAngularVelocity = pDesc->fMaxAngularVelocity;
	m_vMaxVelocity = pDesc->vMaxVelocity;
	m_fFrictionScale = pDesc->fFrictionScale;
	m_fAngularFrictionScale = pDesc->fAngularFrictionScale;
	m_bMoveLocal = pDesc->bMoveLocal;

	return S_OK;
}

void CRigidbody::Update(_float fTimeDelta)
{
	m_fAngularVelocity += m_fAngularAccel * fTimeDelta;
	m_fAngularVelocity = clamp(m_fAngularVelocity, -m_fMaxAngularVelocity, m_fMaxAngularVelocity);

	m_pOwnerTransform->Set_RotationSpeed(m_fAngularVelocity);

	m_pOwnerTransform->Turn(XMLoadFloat4(&m_vRotAxis), fTimeDelta);

	if (0.f != m_fAngularFrictionScale && 0.f == m_fAngularAccel)
	{
		_float fInvAngularVelocity = m_fAngularVelocity * m_fAngularFrictionScale * fTimeDelta;

		if (m_fAngularVelocity < fInvAngularVelocity)
			m_fAngularVelocity = 0.f;
		else
			m_fAngularVelocity -= fInvAngularVelocity;

		m_pOwnerTransform->Set_RotationSpeed(m_fAngularVelocity);
	}

	m_fAngularAccel = 0.f;

	_vector vVelocity = XMLoadFloat3(&m_vVelocity);
	_vector vLinearAccel = XMLoadFloat3(&m_vLinearAccel);

	vVelocity = XMVectorClamp(vVelocity + vLinearAccel * fTimeDelta, XMVectorSet(-m_vMaxVelocity.x, -m_vMaxVelocity.y, -m_vMaxVelocity.z, 0.f)
		, XMLoadFloat3(&m_vMaxVelocity));

	if (false == m_pOwnerTransform->Add_Position(vVelocity * fTimeDelta, m_bMoveLocal, m_pNavigation))
		vVelocity /= 2.f;

	if (0.f != m_fFrictionScale && 0.f == XMVector3Length(vLinearAccel).m128_f32[0])
	{
		_vector vInvVelocity = vVelocity * m_fFrictionScale * fTimeDelta;

		if (XMVector3Length(vVelocity).m128_f32[0] < XMVector3Length(vInvVelocity).m128_f32[0])
			vVelocity = XMVectorZero();
		else
			vVelocity -= vInvVelocity;
	}

	XMStoreFloat3(&m_vLinearAccel, XMVectorZero());
	XMStoreFloat3(&m_vVelocity, vVelocity);

}


void CRigidbody::Change_Navigation(CNavigation* pNavigation)
{
	Safe_Release(m_pNavigation);
	m_pNavigation = pNavigation;
	Safe_AddRef(m_pNavigation);
}

void CRigidbody::Add_Force(_fvector vForce, FORCEMODE eMode)
{
	switch (eMode)
	{
	case Force:
	{
		_vector vLinearAccel = XMLoadFloat3(&m_vLinearAccel);
		vLinearAccel += vForce / m_fMass;
		XMStoreFloat3(&m_vLinearAccel, vLinearAccel);
		break;
	}
		
	case Acceleration:
	{
		_vector vLinearAccel = XMLoadFloat3(&m_vLinearAccel);
		vLinearAccel += vForce;
		XMStoreFloat3(&m_vLinearAccel, vLinearAccel);
		break;
	}
		
	case Impulse:
	{
		_vector vVelocity = XMLoadFloat3(&m_vVelocity);
		vVelocity += vForce / m_fMass;
		XMStoreFloat3(&m_vVelocity, vVelocity);
		break;
	}
		
	case VelocityChange:
	{
		_vector vVelocity = XMLoadFloat3(&m_vVelocity);
		vVelocity += vForce;
		XMStoreFloat3(&m_vVelocity, vVelocity);
		break;
	}	
	}

}

void CRigidbody::Add_Torque(_fvector vAxis, _float fTorque, FORCEMODE eMode)
{
	switch (eMode)
	{
	case Force:
	{
		m_fAngularAccel += fTorque / m_fMass;
		break;
	}
	case Acceleration:
	{
		m_fAngularAccel += fTorque;
		break;
	}
	case Impulse:
	{
		m_fAngularVelocity += fTorque / m_fMass;
		break;
	}
	case VelocityChange:
	{
		m_fAngularVelocity += fTorque;
		break;
	}
	}

	XMStoreFloat4(&m_vRotAxis, vAxis);
}


CRigidbody* CRigidbody::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRigidbody* pPrototype = new CRigidbody(_pDevice, _pContext);

	if (FAILED(pPrototype->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CRigidbody");
		Safe_Release(pPrototype);
	}

	return pPrototype;
}

CComponent* CRigidbody::Clone(void* _pArg)
{
	CComponent* pClone = new CRigidbody(*this);

	if (FAILED(pClone->Initialize(_pArg))) {
		MSG_BOX(L"Failed to Clone : CRigidbody");
		Safe_Release(pClone);
	}

	return pClone;
}

void CRigidbody::Free()
{
	__super::Free();

	Safe_Release(m_pOwnerTransform);
	Safe_Release(m_pNavigation);
}

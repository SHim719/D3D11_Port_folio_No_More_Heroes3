#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	m_vProjFrustumPoints[0] = { -1.f, 1.f, 0.f, 1.f };
	m_vProjFrustumPoints[1] = { 1.f, 1.f, 0.f, 1.f };
	m_vProjFrustumPoints[2] = { 1.f, -1.f, 0.f, 1.f };
	m_vProjFrustumPoints[3] = { -1.f, -1.f, 0.f, 1.f };
	m_vProjFrustumPoints[4] = { -1.f, 1.f, 1.f, 1.f };
	m_vProjFrustumPoints[5] = { 1.f, 1.f, 1.f, 1.f };
	m_vProjFrustumPoints[6] = { 1.f, -1.f, 1.f, 1.f };
	m_vProjFrustumPoints[7] = { -1.f, -1.f, 1.f, 1.f };

	return S_OK;
}

void CFrustum::Update()
{
	_matrix InverseViewMatrix = XMMatrixInverse(nullptr, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	_matrix InverseProjMatrix = XMMatrixInverse(nullptr, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	
	_vector vWorldFrustumPoints[8] = {};

	for (_uint i = 0; i < 8; ++i)
	{
		vWorldFrustumPoints[i] = XMLoadFloat4(&m_vProjFrustumPoints[i]);
	
		vWorldFrustumPoints[i] = XMVector3TransformCoord(vWorldFrustumPoints[i], InverseProjMatrix);
		vWorldFrustumPoints[i] = XMVector3TransformCoord(vWorldFrustumPoints[i], InverseViewMatrix);
	}
	
	XMStoreFloat4(&m_WorldFrustumPlane[0], XMPlaneFromPoints(vWorldFrustumPoints[0], vWorldFrustumPoints[1], vWorldFrustumPoints[2]));
	XMStoreFloat4(&m_WorldFrustumPlane[1], XMPlaneFromPoints(vWorldFrustumPoints[1], vWorldFrustumPoints[5], vWorldFrustumPoints[6]));
	XMStoreFloat4(&m_WorldFrustumPlane[2], XMPlaneFromPoints(vWorldFrustumPoints[4], vWorldFrustumPoints[0], vWorldFrustumPoints[3]));
	XMStoreFloat4(&m_WorldFrustumPlane[3], XMPlaneFromPoints(vWorldFrustumPoints[4], vWorldFrustumPoints[5], vWorldFrustumPoints[1]));
	XMStoreFloat4(&m_WorldFrustumPlane[4], XMPlaneFromPoints(vWorldFrustumPoints[3], vWorldFrustumPoints[2], vWorldFrustumPoints[6]));
	XMStoreFloat4(&m_WorldFrustumPlane[5], XMPlaneFromPoints(vWorldFrustumPoints[5], vWorldFrustumPoints[4], vWorldFrustumPoints[7]));
}

_bool CFrustum::In_WorldFrustum(_fvector vWorldPos, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (XMPlaneDotCoord(XMLoadFloat4(&m_WorldFrustumPlane[i]), vWorldPos).m128_f32[0] > fRadius)
			return false;
	}

	return true;
}


CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	Safe_Release(m_pGameInstance);
}

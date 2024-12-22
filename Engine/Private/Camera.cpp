#include "Camera.h"
#include "PipeLine.h"


CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_CameraDesc{}
{
}

CCamera::CCamera(const CCamera& rhs, CTransform::TRANSFORMDESC* pArg)
	: CGameObject(rhs)
{
	m_pTransform = (CTransform*)rhs.m_pTransform->Clone(pArg);
}

HRESULT CCamera::Initialize_Prototype()
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransform->Set_Position(XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	m_fCascadeEnd[0] = m_CameraDesc.fNear;
	m_fCascadeEnd[1] = 20.f;
	m_fCascadeEnd[2] = 50.f;
	m_fCascadeEnd[3] = 100.f;

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	
}

void CCamera::LateTick(_float fTimeDelta)
{
	
}

void CCamera::Update_ViewProj()
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrixInverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(To_Radian(m_CameraDesc.fFovy), m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
}

void CCamera::Update_Cascade()
{
	//float fTanHalfVFov = tanf(m_CameraDesc.fFovy / 2.f);
	//float fTanHalfHFov = fTanHalfVFov * m_CameraDesc.fAspect;
	//
	//for (_uint i = 0; i < 3; ++i)
	//{
	//	float xn = m_fCascadeEnd[i] * fTanHalfHFov;
	//	float xf = m_fCascadeEnd[i + 1] * fTanHalfHFov;
	//	float yn = m_fCascadeEnd[i] * fTanHalfVFov;
	//	float yf = m_fCascadeEnd[i + 1] * fTanHalfVFov;
	//
	//	_vector vFrustumCorners[8] =
	//	{
	//		{xn, yn,   m_fCascadeEnd[i], 1.f}		,
	//		{-xn, yn,  m_fCascadeEnd[i], 1.f}		,
	//		{xn, -yn,  m_fCascadeEnd[i], 1.f}		,
	//		{-xn, -yn, m_fCascadeEnd[i], 1.f}		,
	//
	//		{xf,   yf, m_fCascadeEnd[i + 1], 1.f}	,
	//		{-xf ,yf,  m_fCascadeEnd[i + 1], 1.f}	,
	//		{xf, -yf,  m_fCascadeEnd[i + 1], 1.f}	,
	//		{-xf, -yf, m_fCascadeEnd[i + 1], 1.f}	,
	//	};
	//
	//	_vector vCenterPos = XMVectorZero();
	//	for (_uint j = 0; j < 8; ++j)
	//	{
	//		vFrustumCorners[j] = XMVector3TransformCoord(vFrustumCorners[j], m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW));
	//		vCenterPos += vFrustumCorners[j];
	//	}
	//
	//	vCenterPos /= 8.f;
	//	XMVectorSetW(vCenterPos, 1.f);
	//
	//	_float fRadius = 0.f;
	//
	//	for (_uint j = 0; j < 8; ++j)
	//	{
	//		_float fDistance = XMVector3Length(vFrustumCorners[j] - vCenterPos).m128_f32[0];
	//		fRadius = max(fRadius, fDistance);
	//	}
	//
	//	fRadius = ceil(fRadius * 16.f) / 16.f;
	//
	//	_vector vMaxExtents = XMVectorSet(fRadius, fRadius, fRadius, 0.f);
	//	_vector vMinExtents = -vMaxExtents;
	//
	//	_vector vShadowCameraPos = vCenterPos + m_pGameInstance->Get_LightDir() * XMVectorGetZ(vMinExtents);
	//
	//	//if (fabsf(XMVector3Length(vShadowCameraPos - XMLoadFloat3(&m_PrevShadowCamPos[i])).m128_f32[0]) < 5.f) 
	//	//	break;
	//
	//	XMStoreFloat3(&m_PrevShadowCamPos[i], vShadowCameraPos);
	//
	//	_matrix LightMatrix = XMMatrixLookAtLH(vShadowCameraPos, vCenterPos, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//
	//	_vector vCascadeExtents = vMaxExtents - vMinExtents;
	//
	//	_matrix CascadeViewProj = LightMatrix * XMMatrixOrthographicLH(vCascadeExtents.m128_f32[0], vCascadeExtents.m128_f32[1], 0.f, vCascadeExtents.m128_f32[2]);
	//
	//	m_pGameInstance->Set_CascadeViewProj(i, CascadeViewProj);
	//}


	for (_uint i = 0; i < 3; ++i)
	{
		_vector vFrustumCorners[8] =
		{
			{ -1.f, 1.f, 0.f, 1.f },
			{ 1.f, 1.f, 0.f, 1.f },
			{ 1.f, -1.f, 0.f, 1.f },
			{ -1.f, -1.f, 0.f, 1.f },
			{ -1.f, 1.f, 1.f, 1.f },
			{ 1.f, 1.f, 1.f, 1.f },
			{ 1.f, -1.f, 1.f, 1.f },
			{ -1.f, -1.f, 1.f, 1.f },
		};
	
		_matrix CascadeCameraViewProj = m_pTransform->Get_WorldMatrixInverse();
		CascadeCameraViewProj *= XMMatrixPerspectiveFovLH(To_Radian(m_CameraDesc.fFovy), m_CameraDesc.fAspect, m_fCascadeEnd[i], m_fCascadeEnd[i + 1]);
	
		_matrix CascadeCameraViewProjInv = XMMatrixInverse(nullptr, CascadeCameraViewProj);
	
		_vector vFrustumCenter = XMVectorZero();
		for (_uint j = 0; j < 8; ++j)
		{
			vFrustumCorners[j] = XMVector3TransformCoord(vFrustumCorners[j], CascadeCameraViewProjInv);
			vFrustumCenter += vFrustumCorners[j];
		}
		vFrustumCenter /= 8.f;
		XMVectorSetW(vFrustumCenter, 1.f);
		
		_float fRadius = XMVector3Length(vFrustumCorners[0] - vFrustumCorners[6]).m128_f32[0] * 0.5f;
		_vector vLightDir = m_pGameInstance->Get_LightDir();
		//_float fFrustumSize = fRadius * 2.f;
	
		//_float fTexelSize = fFrustumSize / m_pGameInstance->Get_ShadowMapScale();
		//
		//vFrustumCenter /= fTexelSize;
		//vFrustumCenter = XMVectorFloor(vFrustumCenter);
		//vFrustumCenter *= fTexelSize;
	
		_float fTexelPerUnit = m_pGameInstance->Get_CascadeShadowMapScale() / (fRadius * 2.f);
		
		_matrix ScalrMatrix = XMMatrixScaling(fTexelPerUnit, fTexelPerUnit, fTexelPerUnit);
		
		_matrix BaseViewMatrix = XMMatrixLookAtLH(XMVectorZero(), -vLightDir, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		BaseViewMatrix = XMMatrixMultiply(ScalrMatrix, BaseViewMatrix);
		
		_matrix BaseViewMatrixInv = XMMatrixInverse(nullptr, BaseViewMatrix);
		
		vFrustumCenter = XMVector3TransformCoord(vFrustumCenter, BaseViewMatrix);
		XMVectorSetX(vFrustumCenter, floor(XMVectorGetX(vFrustumCenter)));
		XMVectorSetY(vFrustumCenter, floor(XMVectorGetY(vFrustumCenter)));
		vFrustumCenter = XMVector3TransformCoord(vFrustumCenter, BaseViewMatrixInv);
	
		_vector vShadowCameraPos = vFrustumCenter - vLightDir * fRadius * 2.f;
	
		_matrix LightViewMatrix = XMMatrixLookAtLH(vShadowCameraPos, vFrustumCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		
		_matrix CascadeViewProj = LightViewMatrix * XMMatrixOrthographicOffCenterLH(-fRadius, fRadius, -fRadius, fRadius, 
			-fRadius * 6.f, fRadius * 6.f);
	
		m_pGameInstance->Set_CascadeViewProj(i, CascadeViewProj);
	}
}
	




HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}

#include "..\Public\PipeLine.h"


CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eTransformState], TransformMatrix);
}

void CPipeLine::Set_CascadeViewProj(_uint iIndex, _fmatrix CascadeViewProj)
{
	XMStoreFloat4x4(&m_CascadeViewProj[iIndex], CascadeViewProj);
}

void CPipeLine::Update_ShadowViewProj(_fvector vLightDir)
{
	_matrix LightViewProj = XMMatrixLookAtLH(XMVectorSet(0.f, 100.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)) *
		XMMatrixPerspectiveFovLH(To_Radian(90.f), 1280.f / 720.f, 0.1f, 3000.f);

	XMStoreFloat4x4(&m_LightViewProj, LightViewProj);
}

void CPipeLine::Update()
{
	for (_uint i = 0; i < D3DTS_END; ++i)
		XMStoreFloat4x4(&m_TransformInverseMatrix[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));

	memcpy(&m_vCamPosition, &m_TransformInverseMatrix[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pPipeLine = new CPipeLine;

	return pPipeLine;
}

void CPipeLine::Free()
{

}

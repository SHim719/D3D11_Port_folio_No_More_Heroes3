#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"


CChannel::CChannel()
{
}



HRESULT CChannel::Initialize(ifstream& fin)
{
	fin.read(m_szName, sizeof(m_szName));

	fin.read((char*)&m_iNumKeyFrames, sizeof(_uint));

	m_KeyFrames.resize(m_iNumKeyFrames);
	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		fin.read((char*)&m_KeyFrames[i], sizeof(KEYFRAME));
	}

	return S_OK;
}

_uint CChannel::Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CBone* pBone)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;
	}
	else
	{
		while (iCurrentKeyFrame + 1 < (_uint)m_KeyFrames.size() && fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;

		_float		fRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
			(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	if (nullptr != pBone)
	{
		pBone->Set_Transformation(TransformationMatrix);
		KEYFRAME lastKeyFrame;
		memcpy(&lastKeyFrame.vScale, &vScale, sizeof(_float3));
		memcpy(&lastKeyFrame.vRotation, &vRotation, sizeof(_float4));
		memcpy(&lastKeyFrame.vPosition, &vPosition, sizeof(_float3));
		pBone->Set_LastKeyFrame(lastKeyFrame);
	}
		

	return iCurrentKeyFrame;
}

_uint CChannel::Blend_Transformation(_float fBlendRatio, _float fPlayTime, _uint iCurrentKeyFrame, CBone* pBone)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;
	}
	else
	{
		while (iCurrentKeyFrame + 1 < (_uint)m_KeyFrames.size() && fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;

		_float		fCurAnimRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
			(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fCurAnimRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fCurAnimRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fCurAnimRatio));
	}

	if (nullptr != pBone)
	{
		KEYFRAME curKeyFrame;
		memcpy(&curKeyFrame.vScale, &vScale, sizeof(_float3));
		memcpy(&curKeyFrame.vRotation, &vRotation, sizeof(_float4));
		memcpy(&curKeyFrame.vPosition, &vPosition, sizeof(_float3));
		pBone->Set_BlendTransformation(curKeyFrame, fBlendRatio);
	}

	return iCurrentKeyFrame;
}

_matrix CChannel::Get_CurTransformation(_float fPlayTime, _uint iCurrentKeyFrame)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	_float		fRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
		(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

	_float3		vSourScale, vDestScale;
	_float4		vSourRotation, vDestRotation;
	_float3		vSourPosition, vDestPosition;

	vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
	vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

	vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
	vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

	vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
	vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
	XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	
	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	return TransformationMatrix;
}

void CChannel::Set_CurrentKeyFrame(_float fPlayTime)
{

}

CChannel* CChannel::Create(ifstream& fin)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(fin)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{

}


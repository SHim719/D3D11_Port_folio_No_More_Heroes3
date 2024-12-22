#include "Bone.h"
#include "Model.h"

CBone::CBone()
{

}

CBone::CBone(const CBone& rhs)
	: m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_Transformation(rhs.m_Transformation)
	, m_iDepth(rhs.m_iDepth)
	, m_iParentBoneIdx(rhs.m_iParentBoneIdx)
	, m_bRootBone(rhs.m_bRootBone)
{
	strcpy_s(m_szName, rhs.m_szName);
	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());
}


HRESULT CBone::Initialize(ifstream& fin, CModel* pModel)
{
	fin.read(m_szName, sizeof(m_szName));
	fin.read((char*)&m_OffsetMatrix, sizeof(_float4x4));
	fin.read((char*)&m_Transformation, sizeof(_float4x4));
	fin.read((char*)&m_iDepth, sizeof(_uint));
	fin.read((char*)&m_iParentBoneIdx, sizeof(_int));
	return S_OK;
}

void CBone::Set_CombinedTransformation(const vector<CBone*>& Bones, _fmatrix PivotMatrix)
{
	if (m_iParentBoneIdx >= 0)
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&Bones[m_iParentBoneIdx]->m_CombinedTransformation));
	else
		XMStoreFloat4x4(&m_CombinedTransformation,  XMLoadFloat4x4(&m_Transformation) * PivotMatrix);
}


void CBone::Set_BlendTransformation(const KEYFRAME& curKeyFrame, _float fRatio)
{
	_float3			vScale = {};
	_float4			vRotation = {};
	_float3			vPosition = {};

	if (!m_bRootBone)
	{
		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_LastKeyFrame.vScale;
		vDestScale = curKeyFrame.vScale;

		vSourRotation = m_LastKeyFrame.vRotation;
		vDestRotation = curKeyFrame.vRotation;

		vSourPosition = m_LastKeyFrame.vPosition;
		vDestPosition = curKeyFrame.vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}
	else
	{
		vScale = curKeyFrame.vScale;
		vRotation = curKeyFrame.vRotation;
		vPosition = curKeyFrame.vPosition;
	}
	
	_matrix	TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f)
		, XMLoadFloat4(&vRotation)
		, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	XMStoreFloat4x4(&m_Transformation, TransformationMatrix);
}

CBone* CBone::Create(ifstream& fin, CModel* pModel)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(fin, pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	CBone* pInstance = new CBone(*this);

	return pInstance;
}

void CBone::Free()
{
}

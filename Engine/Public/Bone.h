#pragma once

#include "Base.h"

/* 계층구조(뼈끼리의 상속관계)를 표현하기위한 aiNode이다. */

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}
	_uint Get_Depth() const {
		return m_iDepth;
	}

	_matrix Get_Transformation(){
		return XMLoadFloat4x4(&m_Transformation);
	}

	_matrix Get_OffSetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformation() {
		return XMLoadFloat4x4(&m_CombinedTransformation);
	}

	void Set_ExtraOffset(_fmatrix OffsetMatrix) {
		 XMStoreFloat4x4(&m_CombinedTransformation, OffsetMatrix * XMLoadFloat4x4(&m_CombinedTransformation));
	}

public:
	HRESULT Initialize(ifstream& fin, class CModel* pModel);
	void Set_CombinedTransformation(const vector<CBone*>& Bones, _fmatrix PivotMatrix);
	void Set_OffsetMatrix(_fmatrix OffsetMatrix) {
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}

	void Set_Transformation(_fmatrix Transformation) {
		XMStoreFloat4x4(&m_Transformation, Transformation);
	}

	void Set_ParentIdx(_int iIdx) {
		m_iParentBoneIdx = iIdx;
	}

	void Set_LastKeyFrame(const KEYFRAME& KeyFrame)	{
		memcpy(&m_LastKeyFrame, &KeyFrame, sizeof(KEYFRAME));
	}

	const KEYFRAME& Get_LastKeyFrame() const {
		return m_LastKeyFrame;
	}

	void Reset_Position() {
		XMStoreFloat4((_float4*)m_Transformation.m[3], XMVectorSetW(XMVectorZero(), 1.f));
	}

	void Set_RootBone() {
		m_bRootBone = true;
	}
		
	
public:
	void Set_BlendTransformation(const KEYFRAME& curKeyFrame, _float fRatio);


private:
	char				m_szName[MAX_PATH] = "";
	_float4x4			m_OffsetMatrix;
	_float4x4			m_Transformation;
	_float4x4			m_CombinedTransformation;
	KEYFRAME			m_LastKeyFrame;

	_int				m_iParentBoneIdx = -1;
	_uint				m_iDepth = 0;
	_bool				m_bRootBone = false;
public:
	static CBone* Create(ifstream& fin, class CModel* pModel);
	CBone* Clone();
	virtual void Free();
};

END
#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	typedef vector<class CBone*>			BONES;
	typedef vector<class CMeshContainer*>	MESHES;
	typedef vector<class CAnimation*>		ANIMATIONS;
	typedef unordered_map<string, class CKeyFrameEvent*> KEYFRAMEEVENTS;

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CBone* Get_Bone(const char* pNodeName);

	_uint Find_BoneIndex(const char* pBoneName);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

public:
	HRESULT Initialize_Prototype(const string& strModelFilePath, const string& strModelFileName, const string& strKeyFrameFilePath);
	HRESULT Initialize(const BONES& Bones, const ANIMATIONS& Anims, const KEYFRAMEEVENTS& Events);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader);
	void	Update_BoneMatrices();
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName);
	HRESULT Bind_Buffers(_uint iMeshIdx);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);
	void Release_Vertices();

	void Set_PivotMatrix(_fmatrix PivotMatrix);

	_bool Picking(_fmatrix InvWorldMat, _fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist);
private:
	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

	_uint									m_iNumMeshes = { 0 };
	vector<class CMeshContainer*>			m_Meshes;

private:
	static unordered_map<string, class CTexture*>			g_ModelTextures;
	_uint													m_iNumMaterials = { 0 };
	vector<MATERIALDESC>									m_Materials;

private:
	vector<class CBone*>					m_Bones;
	vector<_uint>							m_BoneIndices;
	_uint									m_iNumBones = { 0 };
	_uint									m_iRootBoneIdx = { 0 };
	_float4									m_vPrevRootPos = {};
	_float4									m_vDeltaRootPos = {};
	_float4x4								m_BoneMatrices[256] = {};

public:
	const BONES& Get_Bones() { 
		return m_Bones; }

	_vector Get_DeltaRootPos() {
		return Organize_RootPos(XMLoadFloat4(&m_vDeltaRootPos));
	}

	_vector Organize_RootPos(_fvector OriginRootPos) const {
		return XMVectorSet(OriginRootPos.m128_f32[0], OriginRootPos.m128_f32[1], OriginRootPos.m128_f32[2], 1.f);
	}

	void Reset_PrevRootPos();
	_vector Get_NowRootQuat() const;
	
	void Set_RootBoneIdx(_uint iIdx) {
		m_iRootBoneIdx = iIdx;
	}

	void Copy_BoneMatrices(_float4x4* pBoneMatrices) {
		if (nullptr != pBoneMatrices)
			memcpy(pBoneMatrices, m_BoneMatrices, sizeof(_float4x4) * 256);
	}

private:
	void Calc_DeltaRootPos();

private:
	_uint											m_iCurrentAnimIndex = { 0 };
	_uint											m_iNumAnimations = { 0 };
	vector<class CAnimation*>						m_Animations;

	_bool											m_bPreview = { false };
	_bool											m_bIsPlaying = { false };
	_bool											m_bBlending = { false };
	_bool											m_bComplete = { false };
	_bool											m_bLoop = { true };
	_float											m_fPlayRate = { 1.f };

	unordered_map<string, class CKeyFrameEvent*>	m_AllKeyFrameEvents;
public:
	const vector<CAnimation*>& Get_Animations() const { return m_Animations; }

	_uint	Get_NumAnimations() const { return m_iNumAnimations; }
	_uint	Get_CurrentAnimIndex() const { return m_iCurrentAnimIndex; }

	void		Play_Animation(_float fTimeDelta);
	void		Change_Animation(_uint iAnimIdx, _float fBlendingTime = 0.1f, _bool bLoop = true);
	void		Change_AnimationWithStartFrame(_uint iAnimIdx, _uint iStartKeyFrame, _float fBlendingTime = 0.1f, _bool bLoop = true);
	_bool		Is_Playing() const { return m_bIsPlaying; }
	void		Set_AnimPlay() { m_bIsPlaying = true; }
	void		Set_AnimPause() { m_bIsPlaying = false; }
	void		Set_Preview(_bool b) { m_bPreview = b; }
	_bool		Is_AnimComplete() const { return m_bComplete; }
	void		Set_NowAnimKeyFrame(_uint iKeyFrame, _bool bBlend);
	void		Set_PlayRate(_float fPlayRate);

	// BH - For Get Current KeyFrame
	_uint Get_NowAnimKeyFrame() const;

	HRESULT		Bind_Func(const string& strEventName, function<void()> pFunc);

	// BH - Seperate by Specific Bone BEGIN
public:
	void Play_Animation_Seperated(_float _fTimeDelta);
	void Devide_Model_by_Bone(const string& _strParentBoneName, const string& _strLowerFirstBoneName, const string& _strUpperFirstBoneName);
	void Change_Animation_Sperated(_int iLowerAnimIdx, _int iUpperAnimIdx, _float fLowerBlendingTime = 0.1f, _float fUpperBlendingTime = 0.1f, _bool bLowerLoop = true, _bool bUpperLoop = true);
	void Set_LowerAnimPlay() { m_bLowerPlaying = true; }
	void Set_LowerAnimPause() { m_bLowerPlaying = false; }
	void Set_UpperAnimPlay() { m_bUpperPlaying = true; }
	void Set_UpperAnimPause() { m_bUpperPlaying = false; }
	_bool Is_LowerAnimComplete() const { return m_bLowerComplete; }
	_bool Is_UpperAnimComplete() const { return m_bUpperComplete; }

private:
	string m_strParentBoneName = {};
	_uint m_iParentBoneIndex = { 0 };
	string m_strLowerFirstBoneName = {};
	_uint m_iLowerFirstBoneIndex = { 0 };
	string m_strUpperFirstBoneName = {};
	_uint m_iUpperFirstBoneIndex = { 0 };
	_uint m_iLowerCurAnimIndex = { 0 };
	_uint m_iUpperCurAnimIndex = { 0 };
	_bool m_bLowerLoop = { false };
	_bool m_bUpperLoop = { false };
	_bool m_bLowerPlaying = { false };
	_bool m_bUpperPlaying = { false };
	_bool m_bLowerBlending = { false };
	_bool m_bUpperBlending = { false };
	_bool m_bLowerComplete = { false };
	_bool m_bUpperComplete = { false };


	// BH - Seperate by Specific Bone END

private:
	HRESULT Import_Model(const string& strFilePath, const string& strFileName, const string& strKeyFramefolderPath);
	HRESULT Import_Meshes(ifstream& fin);
	HRESULT Import_MaterialInfo(ifstream& fin, const string& strFilePath);
	HRESULT Import_Bones(ifstream& fin);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	HRESULT Import_Animations(ifstream& fin);
	HRESULT Import_KeyFrameEvents(const string& strKeyFramefolderPath);

	HRESULT Import_ExtraMaterialInfo(MATERIALDESC& MaterialDesc, const wstring& wstrFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, const string& strModelFileName,
		const string& strKeyFrameFilePath = "");
	static void Release_Textures();
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

	friend class CModel_Instance;
};

END




//private:
//	class CVTF* m_pVTF = nullptr;
//	void	Update_VTF(_uint iMeshIndex);
//HRESULT Bind_VTF(CShader* pShader, const char* pConstantName, _uint iMeshIndex);
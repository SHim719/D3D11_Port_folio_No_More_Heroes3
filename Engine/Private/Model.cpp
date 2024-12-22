#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "Shader.h"
#include "KeyFrameEvent.h"


unordered_map<string, class CTexture*>	CModel::g_ModelTextures;

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	XMStoreFloat4x4(&m_PivotMatrix, XMMatrixIdentity());
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	, m_BoneIndices(rhs.m_BoneIndices)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iRootBoneIdx(rhs.m_iRootBoneIdx)
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
}

CBone* CModel::Get_Bone(const char* pNodeName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pNode)
		{
			return !strcmp(pNodeName, pNode->Get_Name());
		});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

_uint CModel::Find_BoneIndex(const char* pBoneName)
{
	_uint iBoneIdx = 0;
	for (CBone* pBone : m_Bones)
	{
		if (!strcmp(pBoneName, pBone->Get_Name()))
			break;

		++iBoneIdx;
	}

	return iBoneIdx;
}


HRESULT CModel::Initialize_Prototype(const string& strModelFilePath, const string& strModelFileName, const string& strKeyFrameFilePath)
{
	if (FAILED(Import_Model(strModelFilePath, strModelFileName, strKeyFrameFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(const BONES& Bones, const ANIMATIONS& Anims, const KEYFRAMEEVENTS& Events)
{
	m_Bones.reserve(Bones.size());
	for (size_t i = 0; i < Bones.size(); ++i)
	{
		CBone* pBone = Bones[i]->Clone();
		m_Bones.emplace_back(pBone);
	}

	for (auto& Pair : Events)
	{
		CKeyFrameEvent* pEvent = Pair.second->Clone();
		m_AllKeyFrameEvents.emplace(Pair.first, pEvent);
	}


	m_Animations.reserve(Anims.size());
	for (size_t i = 0; i < Anims.size(); ++i)
	{
		CAnimation* pAnimation = Anims[i]->Clone(m_AllKeyFrameEvents);
		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader)
{
	return pShader->Bind_RawValue("g_BoneMatrices", m_BoneMatrices, sizeof(_float4x4) * 256);
}

void CModel::Update_BoneMatrices()
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixTranspose(m_Bones[m_BoneIndices[i]]->Get_OffSetMatrix()
			* m_Bones[m_BoneIndices[i]]->Get_CombinedTransformation()));
	}

}

HRESULT CModel::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials || nullptr == m_Materials[iMaterialIndex].pTexture[eTextureType])
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
}

HRESULT CModel::Bind_Buffers(_uint iMeshIdx)
{
	return m_Meshes[iMeshIdx]->Bind_Buffers();
}


void CModel::Reset_PrevRootPos()
{
	if (m_iRootBoneIdx >= (_uint)m_Bones.size())
		return;

	_vector vInitRootPos = m_Animations[m_iCurrentAnimIndex]->Get_InitRootPos();
	vInitRootPos = XMVector3TransformCoord(vInitRootPos, XMLoadFloat4x4(&m_PivotMatrix));

	XMStoreFloat4(&m_vPrevRootPos, vInitRootPos);
}

_vector CModel::Get_NowRootQuat() const
{
	return XMLoadFloat4(&m_Bones[m_iRootBoneIdx]->Get_LastKeyFrame().vRotation);
}

void CModel::Calc_DeltaRootPos()
{
	if (m_bPreview || m_iRootBoneIdx >= (_uint)m_Bones.size())
		return;

	_matrix TransformMatrix = m_Bones[m_iRootBoneIdx]->Get_Transformation() * XMLoadFloat4x4(&m_PivotMatrix);

	_vector vNowRootPos = TransformMatrix.r[3];
	_vector vPrevRootPos = XMLoadFloat4(&m_vPrevRootPos);

	XMStoreFloat4(&m_vDeltaRootPos, vNowRootPos - vPrevRootPos);
	XMStoreFloat4(&m_vPrevRootPos, vNowRootPos);

	m_Bones[m_iRootBoneIdx]->Reset_Position();
}


void CModel::Play_Animation(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	fTimeDelta *= m_fPlayRate;

	if (m_bBlending)
		m_bBlending = m_Animations[m_iCurrentAnimIndex]->Play_Animation_Blend(fTimeDelta, m_Bones, m_bIsPlaying);
	else
		m_bComplete = m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta, m_Bones, m_bLoop, m_bIsPlaying);
	
	Calc_DeltaRootPos();
	if (m_bComplete)
		Reset_PrevRootPos();

	for (auto& pBone : m_Bones)
	{
		pBone->Set_CombinedTransformation(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
	}
}


HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	pShader->Begin(iPassIndex);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Release_Vertices()
{
	for (auto& Mesh : m_Meshes)
		Mesh->Release_Vertices();
}

void CModel::Set_PivotMatrix(_fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
}

_bool CModel::Picking(_fmatrix InvWorldMat, _fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist)
{
	for (auto& pMesh : m_Meshes)
	{
		if (true == pMesh->Picking(InvWorldMat, vRayStartPos, vRayDir, vPickedPos, fDist))
			return true;
	}
		
	return false;
}
void CModel::Change_Animation(_uint iAnimIdx, _float fBlendingTime, _bool bLoop)
{
	if (iAnimIdx > (_uint)m_Animations.size())
		return;

	m_Animations[iAnimIdx]->Reset();

	m_iCurrentAnimIndex = iAnimIdx;
	m_bComplete = false;
	m_bLoop = bLoop;

	Reset_PrevRootPos();

	if (fBlendingTime > 0.f)
	{
		m_bBlending = true;
		m_Animations[m_iCurrentAnimIndex]->Set_BlendingTime(fBlendingTime);
	}
}

void CModel::Change_AnimationWithStartFrame(_uint iAnimIdx, _uint iStartKeyFrame, _float fBlendingTime, _bool bLoop)
{
	if (iAnimIdx > (_uint)m_Animations.size())
		return;
	
	m_Animations[iAnimIdx]->Reset();

	m_iCurrentAnimIndex = iAnimIdx;
	m_bComplete = false;
	m_bLoop = bLoop;

	Set_NowAnimKeyFrame(iStartKeyFrame, fBlendingTime > 0.f);
	
	if (fBlendingTime > 0.f)
	{
		m_bBlending = true;
		m_Animations[m_iCurrentAnimIndex]->Set_BlendingTime(fBlendingTime);
	}
}

void CModel::Set_NowAnimKeyFrame(_uint iKeyFrame, _bool bBlend)
{
	m_Animations[m_iCurrentAnimIndex]->Set_CurrentKeyFrames(iKeyFrame);

	_matrix RootMatrix = m_Animations[m_iCurrentAnimIndex]->Get_RootTransformation();

	_matrix TransformMatrix = RootMatrix * XMLoadFloat4x4(&m_PivotMatrix);

	XMStoreFloat4(&m_vPrevRootPos, TransformMatrix.r[3]);

	XMStoreFloat4(&m_vDeltaRootPos, XMVectorZero());
}

void CModel::Set_PlayRate(_float fPlayRate)
{
	m_fPlayRate = fPlayRate;
}

_uint CModel::Get_NowAnimKeyFrame() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_NowKeyFrame();
}

HRESULT CModel::Bind_Func(const string& strEventName, function<void()> pFunc)
{
	auto it = m_AllKeyFrameEvents.find(strEventName);
	
	if (m_AllKeyFrameEvents.end() == it)
		return E_FAIL;

	it->second->Bind_Func(move(pFunc));

	return S_OK;
}

void CModel::Play_Animation_Seperated(_float _fTimeDelta)
{
	// Lower 
	if (m_iLowerCurAnimIndex >= m_iNumAnimations)
		return;

	if (m_bLowerBlending)
		m_bLowerBlending = m_Animations[m_iLowerCurAnimIndex]->Play_Animation_Blend(_fTimeDelta, m_Bones, m_bLowerPlaying);
	else
		m_bLowerComplete = m_Animations[m_iLowerCurAnimIndex]->Play_Animation(_fTimeDelta, m_Bones, m_bLowerLoop, m_bLowerPlaying);

	Calc_DeltaRootPos();
	if (m_bLowerComplete)
		Reset_PrevRootPos();

	for (_uint i = 0; i < m_iUpperFirstBoneIndex; ++i) {
		m_Bones[i]->Set_CombinedTransformation(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
	}

	// Upper
	if (m_iUpperCurAnimIndex >= m_iNumAnimations)
		return;

	if (m_bUpperBlending)
		m_bUpperBlending = m_Animations[m_iUpperCurAnimIndex]->Play_Animation_Blend(_fTimeDelta, m_Bones, m_bUpperPlaying);
	else
		m_bUpperComplete = m_Animations[m_iUpperCurAnimIndex]->Play_Animation(_fTimeDelta, m_Bones, m_bUpperLoop, m_bUpperPlaying);

	//Calc_DeltaRootPos();
	//if (m_bUpperComplete)
	//	Reset_PrevRootPos();

	for (_uint i = m_iUpperFirstBoneIndex; i < m_Bones.size(); ++i) {
		m_Bones[i]->Set_CombinedTransformation(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
	}
}

void CModel::Devide_Model_by_Bone(const string& _strParentBoneName, const string& _strLowerFirstBoneName, const string& _strUpperFirstBoneName)
{
	_uint iBoneIndex = { 0 };

	for (auto& iter : m_Bones) {
		if (!strcmp(_strParentBoneName.c_str(), iter->Get_Name()))
			m_iParentBoneIndex = iBoneIndex;
		if (!strcmp(_strLowerFirstBoneName.c_str(), iter->Get_Name()))
			m_iLowerFirstBoneIndex = iBoneIndex;
		if (!strcmp(_strUpperFirstBoneName.c_str(), iter->Get_Name()))
			m_iUpperFirstBoneIndex = iBoneIndex;

		iBoneIndex++;
	}
}

void CModel::Change_Animation_Sperated(_int iLowerAnimIdx, _int iUpperAnimIdx, _float fLowerBlendingTime, _float fUpperBlendingTime, _bool bLowerLoop, _bool bUpperLoop)
{
	if (iLowerAnimIdx > (_int)m_Animations.size())
		return;

	if (0 <= iLowerAnimIdx) {
		m_Animations[iLowerAnimIdx]->Reset();

		m_iLowerCurAnimIndex = iLowerAnimIdx;
		m_bLowerComplete = false;
		m_bLowerLoop = bLowerLoop;

		Reset_PrevRootPos();

		if (fLowerBlendingTime > 0.f)
		{
			m_bLowerBlending = true;
			m_Animations[m_iLowerCurAnimIndex]->Set_BlendingTime(fLowerBlendingTime);
		}
	}

	if (iUpperAnimIdx > (int)m_Animations.size())
		return;

	if (0 <= iUpperAnimIdx) {
		m_Animations[iUpperAnimIdx]->Reset();

		m_iUpperCurAnimIndex = iUpperAnimIdx;
		m_bUpperComplete = false;
		m_bUpperLoop = bUpperLoop;

		if (fUpperBlendingTime > 0.f)
		{
			m_bUpperBlending = true;
			m_Animations[m_iUpperCurAnimIndex]->Set_BlendingTime(fUpperBlendingTime);
		}
	}
}

HRESULT CModel::Import_Model(const string& strFilePath, const string& strFileName, const string& strKeyFramefolderPath)
{
	string strFullPath = strFilePath + strFileName;

	ifstream fin;
	fin.open(strFullPath, ios::binary);

	if (!fin.is_open())
		return E_FAIL;

	fin.read((char*)&m_eModelType, sizeof(TYPE));
	fin.read((char*)&m_PivotMatrix, sizeof(_float4x4));

	if (FAILED(Import_Meshes(fin)))
		return E_FAIL;

	if (FAILED(Import_MaterialInfo(fin, strFilePath)))
		return E_FAIL;

	if (TYPE_ANIM == m_eModelType)
	{
		if (FAILED(Import_Bones(fin)))
			return E_FAIL;

		if (FAILED(Import_Animations(fin)))
			return E_FAIL;

		if (FAILED(Import_KeyFrameEvents(strKeyFramefolderPath)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Import_Meshes(ifstream& fin)
{
	fin.read((char*)&m_iNumMeshes, sizeof(_uint));

	m_Meshes.reserve(m_iNumMeshes);
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer* pMesh = CMeshContainer::Create(m_pDevice, m_pContext, fin, m_eModelType);
		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Import_MaterialInfo(ifstream& fin, const string& strFilePath)
{
	fin.read((char*)&m_iNumMaterials, sizeof(_uint));

	m_Materials.reserve(m_iNumMaterials);
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC MaterialDesc = {};
		wstring wstrFilePath = L"";

		for (_uint j = 0; j < TextureType_ORM; ++j)
		{
			_uint iFileLength = 0;
			fin.read((char*)&iFileLength, sizeof(_uint));
			if (0 == iFileLength)
				continue;

			char szFileName[MAX_PATH] = "";
			fin.read(szFileName, iFileLength);

			string strFullPath = strFilePath + string("Tex/") + string(szFileName);
			wstrFilePath = wstring().assign(strFullPath.begin(), strFullPath.end());

			CTexture* pMatTexture = nullptr;
			auto it = g_ModelTextures.find(szFileName);
			if (g_ModelTextures.end() == it)
			{
				pMatTexture = CTexture::Create(m_pDevice, m_pContext, wstrFilePath.c_str());
				if (nullptr == pMatTexture)
					return E_FAIL;

				g_ModelTextures.emplace(szFileName, pMatTexture);
			}
			else
				pMatTexture = it->second;

			MaterialDesc.pTexture[j] = pMatTexture;
			Safe_AddRef(pMatTexture);
		}

		Import_ExtraMaterialInfo(MaterialDesc, wstrFilePath);

		m_Materials.emplace_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Import_Bones(ifstream& fin)
{
	_uint iOriginBoneSize = 0;
	fin.read((char*)&iOriginBoneSize, sizeof(_uint));

	m_Bones.reserve(iOriginBoneSize);
	for (size_t i = 0; i < iOriginBoneSize; ++i)
	{
		CBone* pBone = CBone::Create(fin, this);
		if (nullptr == pBone)
			return E_FAIL;
		m_Bones.emplace_back(pBone);
	}

	m_iNumBones = 0;
	fin.read((char*)&m_iNumBones, sizeof(_uint));

	m_BoneIndices.reserve(m_iNumBones);
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_uint iBoneIdx = 0;
		fin.read((char*)&iBoneIdx, sizeof(_uint));
		m_BoneIndices.emplace_back(iBoneIdx);
	}

	m_iRootBoneIdx = Find_BoneIndex("root");
	if (m_iRootBoneIdx < m_Bones.size())
		m_Bones[m_iRootBoneIdx]->Set_RootBone();
	else {	// BH - Exception for Camera Model
		m_iRootBoneIdx = Find_BoneIndex("cam_root");
		if (m_iRootBoneIdx < m_Bones.size())
			m_Bones[m_iRootBoneIdx]->Set_RootBone();
	}

	return S_OK;
}

HRESULT CModel::Import_Animations(ifstream& fin)
{
	if (TYPE_NONANIM == m_eModelType)
		return S_OK;

	fin.read((char*)&m_iNumAnimations, sizeof(_uint));

	m_Animations.reserve(m_iNumAnimations);
	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnim = CAnimation::Create(fin);
		m_Animations.emplace_back(pAnim);
	}
	return S_OK;
}

HRESULT CModel::Import_KeyFrameEvents(const string& strKeyFramefolderPath)
{
	if (strKeyFramefolderPath.empty())
		return S_OK;
	fs::path keyFramefolderPath(strKeyFramefolderPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(keyFramefolderPath))
	{
		if (entry.is_directory())
			continue;
		if (".json" != entry.path().extension().generic_string())
			continue;


		//_int iAnimNameLength = 0;
		//_char szAnimName[MAX_PATH] = "";
		//_int iKeyFrame = 0;
		//_int iEventLength = 0;
		//_char szEvent[MAX_PATH] = "";

		if (entry.is_directory())
			continue;

		if (entry.path().extension().generic_string() != ".json")
			continue;

		wstring wstrPath = entry.path().generic_wstring();

		ifstream fin(wstrPath);

		if (!fin.is_open())
			return E_FAIL;

		json jsonData;
		fin >> jsonData;
		fin.close();

		string strAnimationName = jsonData["Animation Name"];

		size_t iAnimIndex = 0;
		for (; iAnimIndex < m_Animations.size(); ++iAnimIndex)
		{
			if (strAnimationName == m_Animations[iAnimIndex]->Get_AnimName())
				break;
		}

		if (m_Animations.size() == iAnimIndex)
			return E_FAIL;

		_int iKeyFrame = 0;

		for (const auto& KeyFrame : jsonData["KeyFrames"])
		{
			CKeyFrameEvent::KEYFRAME_DESC KeyFrameDesc{};

			iKeyFrame = KeyFrame["KeyFrame_Index"];
			KeyFrameDesc.eKeyFrameType = KeyFrame["KeyFrame_Type"];
			KeyFrameDesc.strEventName = KeyFrame["KeyFrame_Name"];
			KeyFrameDesc.fVolume = KeyFrame["Volume"];

			CKeyFrameEvent* pEvent = nullptr;

			auto it = m_AllKeyFrameEvents.find(KeyFrameDesc.strEventName);
			if (m_AllKeyFrameEvents.end() == it)
			{
				pEvent = CKeyFrameEvent::Create(&KeyFrameDesc);
				m_AllKeyFrameEvents.emplace(KeyFrameDesc.strEventName, pEvent);
			}
			else
				pEvent = it->second;

			if (nullptr == pEvent)
				return E_FAIL;

			m_Animations[iAnimIndex]->Add_KeyFrameEvent(iKeyFrame, pEvent);
		}
	}
	return S_OK;
}

HRESULT CModel::Import_ExtraMaterialInfo(MATERIALDESC& MaterialDesc, const wstring& wstrFilePath)
{
	wstring wstrFile = wstrFilePath.substr(0, wstrFilePath.size() - 5);
	wstring wstrSemantics[11] = { L"N", L"ORM", L"MRAO", L"E", L"MRM", L"OP", L"M", L"R", L"RMA", L"Mask", L"FN"};
	TextureType eTextureTypes[11] = { TextureType_NORMALS, TextureType_ORM, TextureType_MRAO, TextureType_EMISSIVE, TextureType_MRM,
	TextureType_OPACITY, TextureType_METALNESS, TextureType_ROUGHNESS, TextureType_RMA, TextureType_MASK, TextureType_FRENSEL_NORMAL };

	for (_uint i = 0; i < 11; ++i)
	{
		if (nullptr != MaterialDesc.pTexture[(_uint)eTextureTypes[i]])
			continue;

		wstring wstrPath = wstrFile + wstrSemantics[i] + L".dds";

		MaterialDesc.pTexture[(_uint)eTextureTypes[i]] = CTexture::Create_ExtraMaterial(m_pDevice, m_pContext, wstrPath.c_str());
	}

	return S_OK;
}


CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, const string& strModelFileName,
	const string& strKeyFrameFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, strModelFileName, strKeyFrameFilePath)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Release_Textures()
{
	for (auto& Pairs : g_ModelTextures)
	{
		Safe_Release(Pairs.second);
	}

	g_ModelTextures.clear();
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(m_Bones, m_Animations, m_AllKeyFrameEvents)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	for (auto& Pair : m_AllKeyFrameEvents)
		Safe_Release(Pair.second);
	
	m_AllKeyFrameEvents.clear();
}


//void CModel::Update_VTF(_uint iMeshIndex)
//{
//	if (TYPE_ANIM != m_eModelType)
//		return;
//
//	_float4x4		BoneMatrices[1024];
//
//	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
//
//	m_pVTF->Update_VTF(BoneMatrices, iMeshIndex);
//
//}
//
//HRESULT CModel::Bind_VTF(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
//{
//	return m_pVTF->Bind_ShaderResourceView(pShader, pConstantName, iMeshIndex);
//}

#include "Model_Instance.h"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh_Instance.h"
#include "Transform.h"

CModel_Instance::CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel_Instance::CModel_Instance(const CModel_Instance& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_InstanceMeshes(rhs.m_InstanceMeshes)
	, m_Materials(rhs.m_Materials)
{
	for (auto& pInstanceMesh : m_InstanceMeshes)
		Safe_AddRef(pInstanceMesh);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
}

HRESULT CModel_Instance::Initialize_Prototype(CModel* pTargetModel)
{
	m_iNumMeshes = pTargetModel->m_iNumMeshes;

	m_InstanceMeshes.reserve(m_iNumMeshes);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
		m_InstanceMeshes.emplace_back(CMesh_Instance::Create(m_pDevice, m_pContext, pTargetModel->m_Meshes[i]));
	
	m_iNumMaterials = pTargetModel->m_iNumMaterials;

	m_Materials = pTargetModel->m_Materials;

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	return S_OK;
}

HRESULT CModel_Instance::SetUp_OnShader(CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials || nullptr == m_Materials[iMaterialIndex].pTexture[eTextureType])
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
}

HRESULT CModel_Instance::Bind_Buffers(_uint iMeshIdx)
{
	return m_InstanceMeshes[iMeshIdx]->Bind_Buffers();
}

HRESULT CModel_Instance::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	pShader->Begin(iPassIndex);

	m_InstanceMeshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel_Instance::Ready_Instancing(vector<_float4x4>& WorldMatrices, vector<_int>& ObjFlags)
{
	for (auto& pMeshInstance : m_InstanceMeshes)
		pMeshInstance->Update_InstanceBuffer(WorldMatrices, ObjFlags);

	return S_OK;
}

CModel_Instance* CModel_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel* pTargetModel)
{
	CModel_Instance* pInstance = new CModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTargetModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel_Instance"));
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel_Instance::Clone(void* pArg)
{
	return new CModel_Instance(*this);
}

void CModel_Instance::Free()
{
	__super::Free();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshInstance : m_InstanceMeshes)
		Safe_Release(pMeshInstance);

	m_InstanceMeshes.clear();

}

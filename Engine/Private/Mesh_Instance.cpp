#include "Mesh_Instance.h"
#include "GameInstance.h"
#include "MeshContainer.h"


CMesh_Instance::CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CMesh_Instance::CMesh_Instance(const CMesh_Instance& rhs)
	: CVIBuffer_Instance(rhs)
	, m_pInstanceVertices(rhs.m_pInstanceVertices)
{
}

HRESULT CMesh_Instance::Initialize_Prototype(CMeshContainer* pMesh)
{
	if (FAILED(Ready_Vertices(pMesh)))
		return E_FAIL;

	if (FAILED(Ready_IndexBuffer(pMesh)))
		return E_FAIL;

	m_iInstanceVertexStride = sizeof(VTXMATRIX);

	m_iNumIndexPerInstance = m_iNumPrimitives * m_iNumIndicesofPrimitive;

	return S_OK;
}

HRESULT CMesh_Instance::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CMesh_Instance::Init_InstanceBuffer(_uint iInstanceCount)
{
	if (iInstanceCount == m_iNumInstance)
		return S_OK;

	m_iNumInstance = iInstanceCount;

	Safe_Release(m_pVBInstance);

	m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceVertexStride;
	
	ZeroMemory(&m_SubResourceData, sizeof(m_SubResourceData));
	m_SubResourceData.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(m_pInstanceVertices);
	
	return S_OK;
}

HRESULT CMesh_Instance::Update_InstanceBuffer(vector<_float4x4>& WorldMatrices, vector<_int>& ObjFlags)
{
	// ÄÃ¸µ
	vector<_float4x4> InstanceDatas;
	InstanceDatas.reserve(WorldMatrices.size());

	Culling_Frustum(WorldMatrices, InstanceDatas);

	if (FAILED(Init_InstanceBuffer((_uint)InstanceDatas.size())))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < InstanceDatas.size(); ++i)
	{
		_matrix WorldMatrix = XMLoadFloat4x4(&InstanceDatas[i]);

		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vRight), WorldMatrix.r[0]);
		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vUp), WorldMatrix.r[1]);
		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vLook), WorldMatrix.r[2]);
		XMStoreFloat4(&(static_cast<VTXMATRIX*>(SubResource.pData)[i].vTranslation), WorldMatrix.r[3]);

		static_cast<VTXMATRIX*>(SubResource.pData)[i].iObjFlag = ObjFlags[i];
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}


void CMesh_Instance::Culling_Frustum(vector<_float4x4>& WorldMatrices, OUT vector<_float4x4>& InstanceDatas)
{
	for (auto& WorldMatrix : WorldMatrices)
	{
		// Culling
		InstanceDatas.emplace_back(WorldMatrix);
	}

}

HRESULT CMesh_Instance::Ready_Vertices(CMeshContainer* pMesh)
{
	m_iNumVertices = pMesh->m_iNumVertices;

	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pMesh->m_pModelVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh_Instance::Ready_IndexBuffer(CMeshContainer* pMesh)
{
	m_iNumPrimitives = pMesh->m_iNumPrimitives;

	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pMesh->m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

CMesh_Instance* CMesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CMeshContainer* pMesh)
{
	CMesh_Instance* pInstance = new CMesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pMesh)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CMesh_Instance::Clone(void* pArg)
{
	CMesh_Instance* pInstance = new CMesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Instance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pInstanceVertices);

}

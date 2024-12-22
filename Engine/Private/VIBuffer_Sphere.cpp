#include "VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs)
	: CVIBuffer(rhs)

{

}

HRESULT CVIBuffer_Sphere::Initialize_Prototype(const _uint iNumStacks, const _uint iNumSlices)
{
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = iNumStacks * iNumSlices;
	m_iStride = sizeof(VTXPOINT);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;
	
	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	_float fStackDeltaTheta = -2.f * XM_PI / (_float)iNumStacks;
	_float fSliceDeltaTheta = -2.f * XM_PI / (_float)iNumSlices;
	for (_uint i = 0; i < iNumStacks; ++i)
	{
		_vector vStackStartPos = XMVectorSet(0.f, -0.5f, 0.f, 1.f);
		vStackStartPos = XMVector3TransformCoord(vStackStartPos, XMMatrixRotationZ(fStackDeltaTheta * (float)i));
		for (_uint j = 0; j < iNumSlices; ++j)
		{
			_vector vPoint = XMVector3TransformCoord(vStackStartPos, XMMatrixRotationY(fSliceDeltaTheta * (float)j));
			XMStoreFloat3(&(pVertices[i * iNumSlices + j].vPosition), vPoint);
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = iNumStacks * iNumSlices * 2;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint iIdx = 0;

	FACEINDICES16* pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);

	for (_uint i = 0; i < iNumStacks; i++) {

		const int iOffset = (iNumSlices + 1) * i;

		for (_uint j = 0; j < iNumSlices; j++) {

			pIndices[iIdx]._0 = (iOffset + j);
			pIndices[iIdx]._1 = (iOffset + j + iNumSlices + 1);
			pIndices[iIdx]._2 = (iOffset + j + 1 + iNumSlices + 1);
			iIdx++;

			pIndices[iIdx]._0 = (iOffset + j);
			pIndices[iIdx]._1 = (iOffset + j + 1 + iNumSlices + 1);
			pIndices[iIdx]._2 = (iOffset + j + 1);
			iIdx++;
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion
	

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void* pArg)
{
	return S_OK;
}


CVIBuffer_Sphere* CVIBuffer_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint iNumStacks, const _uint iNumSlices)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumStacks, iNumSlices)))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Sphere::Clone(void* pArg)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Sphere"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();
}

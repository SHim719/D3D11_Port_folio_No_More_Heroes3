#include "VIBuffer_Particle.h"

CVIBuffer_Particle::CVIBuffer_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Particle::CVIBuffer_Particle(const CVIBuffer_Particle & rhs)
	: CVIBuffer_Instance(rhs)
{
}

HRESULT CVIBuffer_Particle::Initialize_Prototype()
{
	m_iNumVertices = 1;
	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXPOINT);
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = XMFLOAT3(0.f, 0.0f, 0.f);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete(pVertices);

	return S_OK;
}


HRESULT CVIBuffer_Particle::Initialize(void * pArg)
{


	return S_OK;
}

HRESULT CVIBuffer_Particle::Init_InstanceBuffer(_uint iNumInstance)
{
	m_iNumInstance = iNumInstance;

	Safe_Release(m_pVBInstance);

	VTXPARTICLE* pInstanceVertices = new VTXPARTICLE[m_iNumInstance];

	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceVertexStride;

	ZeroMemory(&m_SubResourceData, sizeof(m_SubResourceData));
	m_SubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);

	return S_OK;
}

void CVIBuffer_Particle::Update_Particle(const vector<VTXPARTICLE>& ParticleDatas)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (size_t i = 0; i < ParticleDatas.size(); ++i)
	{
		memcpy(&static_cast<VTXPARTICLE*>(SubResource.pData)[i], &ParticleDatas[i], sizeof(VTXPARTICLE));
	}

	m_pContext->Unmap(m_pVBInstance, 0);


}

void CVIBuffer_Particle::Update_Particle_Compute(ID3D11Buffer* pCurVerticesBuffer, _uint iNumInstance)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};


	m_pContext->CopyResource(m_pVBInstance, pCurVerticesBuffer);
	
	//m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	//
	//
	//memcpy(static_cast<VTXPARTICLE*>(SubResource.pData), &ParticleDatas[i], sizeof(VTXPARTICLE));
	//
	//
	//m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Particle::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceVertexStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CVIBuffer_Particle::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

	return S_OK;
}


CVIBuffer_Particle * CVIBuffer_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Particle*		pInstance = new CVIBuffer_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CVIBuffer_Particle::Clone(void * pArg)
{
	CVIBuffer_Particle*		pInstance = new CVIBuffer_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle::Free()
{
	__super::Free();
}

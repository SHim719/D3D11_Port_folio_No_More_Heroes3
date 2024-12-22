#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Buffers();
	virtual HRESULT Render();
	
	virtual _bool Picking(_fmatrix InvWorldMat, _fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist);
protected:
	ID3D11Buffer*		m_pVB = {nullptr};
	ID3D11Buffer*		m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC				m_BufferDesc;
	D3D11_SUBRESOURCE_DATA			m_SubResourceData;
	D3D11_PRIMITIVE_TOPOLOGY		m_eTopology;

	_uint						m_iNumVertexBuffers = { 0 };
	_uint						m_iNumVertices = { 0 };
	_uint						m_iStride = { 0 };
	_uint						m_iNumPrimitives = { 0 };
	_uint						m_iIndexSizeofPrimitive = { 0 };
	_uint						m_iNumIndicesofPrimitive = { 0 };
	DXGI_FORMAT					m_eIndexFormat;

	_uint						m_iIndexStride = {0};
	_uint						m_iNumIndices = {0};

	_float3*					m_pVertexPositions = nullptr;

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
};

END
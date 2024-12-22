#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CMesh_Instance final : public CVIBuffer_Instance
{
private:
	CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Instance(const CMesh_Instance& rhs);
	virtual ~CMesh_Instance() = default;

public:
	HRESULT Initialize_Prototype(class CMeshContainer* pMesh);
	HRESULT Initialize(void* pArg) override;

	HRESULT Update_InstanceBuffer(vector<_float4x4>& WorldMatrices, vector<_int>& ObjFlags);

private:
	VTXMATRIX*		m_pInstanceVertices = { nullptr };

private:
	void Culling_Frustum(vector<_float4x4>& WorldMatrices, OUT vector<_float4x4>& InstanceDatas);

private:
	HRESULT Ready_Vertices(class CMeshContainer* pMesh);
	HRESULT Ready_IndexBuffer(class CMeshContainer* pMesh);

	HRESULT Init_InstanceBuffer(_uint iInstanceCount);
	
public:
	static CMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CMeshContainer* pMesh);
	CComponent* Clone(void* pArg) override;
	void Free() override;
};

END
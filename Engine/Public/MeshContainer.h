#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;
public:
	HRESULT Initialize(ifstream& fin, CModel::TYPE eModelType);

	_bool Picking(_fmatrix InvWorldMat, _fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist)	override;
private:
	VTXMODEL*			m_pModelVertices = nullptr;
	VTXANIMMODEL*		m_pAnimVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;

private:
	HRESULT Ready_Vertices(ifstream& fin);
	HRESULT Ready_AnimVertices(ifstream& fin);
	HRESULT Ready_IndexBuffer(ifstream& fin);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin, CModel::TYPE eModelType);
	CComponent* Clone(void* pArg = nullptr) override;
	void Free() override;

	void Release_Vertices();


	friend class CMesh_Instance;
};

END
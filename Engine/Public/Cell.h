#pragma once

#include "Base.h"

/* 네비게이션을 구성하는 삼각형 중 하나. */
/* 이 삼각형이 전체 삼각형 중 몇번째 인지도 저장해두자. */

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMVectorSetW(XMLoadFloat3(&m_vPoints[ePoint]), 1.f);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighborCell) {
		m_iNeighborIndices[eLine] = pNeighborCell->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool isIn(_fvector vLocalPos, _int* pNeighborIndex, OUT _float4* pNormal = nullptr);

	_bool Compare_Points(_fvector vSour, _fvector vDest);

	_float Calc_YPos(_fvector vPosition);

#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_float3					m_vPoints[POINT_END] = {};
	_int					m_iIndex = { -1 };
	_int					m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	_uint					m_iCellAttribute = { 0 };

#ifdef _DEBUG
private:
	class CVIBuffer* m_pVIBuffer = { nullptr };
#endif


public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

END
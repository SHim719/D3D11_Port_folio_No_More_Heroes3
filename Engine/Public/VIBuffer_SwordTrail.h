#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_SwordTrail final : public CVIBuffer
{
public:
	CVIBuffer_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& rhs);
	virtual ~CVIBuffer_SwordTrail() = default;

private:
	deque<_float3>		m_TrailPoint;

	_uint				m_iEndIndex = 0;
	_uint				m_iCatmullRomCount = 0;
	_uint				m_iVtxCount = 0;
	_uint				m_iCatmullRomIndex[4] = { 0 };

	_uint				m_iNumCatmullRomVertices = 0;
	TRAIL_DESC			m_tTrailDesc = {};
	_float				m_fTimeAcc = { 0.f };
	_bool				m_bDeletePhase = { false };

	_float3				m_vLastStartPos;
	_float3				m_vLastEndPos;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	HRESULT Update(_matrix Parentmat, _bool b = false);
	HRESULT Clear();
	HRESULT Remakee_Buffers(const TRAIL_DESC& Desc);
	void Reset_Points();
	_bool IsEmpty();
	void Set_DeleteStart(_bool bStart) { m_bDeletePhase = bStart; }

	vector<VTXTEX>     m_Vertex;
	vector<VTXTEX>     m_CatMulRomVertices;
	//void DeleteVertex();

public:
	static CVIBuffer_SwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END
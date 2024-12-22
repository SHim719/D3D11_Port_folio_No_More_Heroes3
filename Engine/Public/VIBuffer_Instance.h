#pragma once

#include "VIBuffer.h"

/* 같은 모델을 다수 찍어내야하는 클래스들의 부모를 담당하는 클래스 */
/* 그려지고 있는 각각의 로컬 상태변환을 담당(행렬)하는 정점들을 가지고 있는 정점버퍼하나를 추가적으로 보유한다. */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	HRESULT Bind_Buffers()			override;
	HRESULT Render()				override;

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	/* 추가적 정점(행렬)을 가진다. */
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iInstanceVertexStride = { 0 };
	_uint						m_iNumIndexPerInstance = { 0 };

public:
	CComponent* Clone(void* pArg) = 0;
	void Free() override;
};

END
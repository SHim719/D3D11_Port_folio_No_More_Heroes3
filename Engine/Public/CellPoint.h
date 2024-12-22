#pragma once

#include "Base.h"


BEGIN(Engine)

class CCellPoint final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };

private:
	CCellPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCellPoint() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMVectorSetW(XMLoadFloat3(&m_vPoints), 1.f);
	}

public:
	HRESULT Initialize(const _float3* pPoints);

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader, _int iCurIdx);

#endif

private:
	HRESULT Tool_SetColor(_int iCurIdx);

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_float3					m_vPoints = {};
	_uint					m_iCellAttribute = { 0 };
	_float4					m_vPointColor = {};

#ifdef _DEBUG
private:
	class CVIBuffer_Sphere* m_pVIBufferSphere = { nullptr };
#endif

public:
	static CCellPoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual void Free() override;
};

END
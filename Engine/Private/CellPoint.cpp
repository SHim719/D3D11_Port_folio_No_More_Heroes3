#include "..\Public\CellPoint.h"
#include "VIBuffer_Sphere.h"
#include "Shader.h"


CCellPoint::CCellPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCellPoint::Initialize(const _float3* pPoints)
{
	m_vPoints = *pPoints;

#ifdef _DEBUG
	m_pVIBufferSphere = CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 3, 3);
	if (nullptr == m_pVIBufferSphere)
		return E_FAIL;
#endif

	return S_OK;
}

#ifdef _DEBUG

HRESULT CCellPoint::Render(CShader* pShader, _int iCurIdx)
{
	_float4x4 world;
	XMStoreFloat4x4(&world, XMMatrixScaling(0.15f, 0.15f, 0.15f) * XMMatrixTranslation(m_vPoints.x, m_vPoints.y, m_vPoints.z));

	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &world)))
		return E_FAIL;

	Tool_SetColor(iCurIdx);

	if (FAILED(pShader->Bind_RawValue("g_vColor", &m_vPointColor, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(0);

	m_pVIBufferSphere->Bind_Buffers();
	return m_pVIBufferSphere->Render();
}

HRESULT CCellPoint::Tool_SetColor(_int iCurIdx)
{
	switch (iCurIdx)
	{
	case 1:
		m_vPointColor = { 1.f, 0.f, 0.f, 1.f };
		break;
	case 2:
		m_vPointColor = { 0.f, 0.f, 1.f, 1.f };
		break;
	case 3:
		m_vPointColor = { 0.f, 1.f, 0.f, 1.f };
		break;
	default:
		m_vPointColor = { 0.f, 1.f, 0.f, 1.f };
		break;
	}

	return S_OK;
}

#endif

CCellPoint* CCellPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
	CCellPoint* pInstance = new CCellPoint(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCellPoint::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBufferSphere);
#endif
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
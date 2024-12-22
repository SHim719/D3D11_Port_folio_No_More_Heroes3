#include "Water.h"

CWater::CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CWater::CWater(const CWater& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWater::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWater::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Add_PitchInput(XM_PIDIV2);

	return S_OK;
}

void CWater::PriorityTick(_float fTimeDelta)
{
}

void CWater::Tick(_float fTimeDelta)
{
	m_fFlowTime += fTimeDelta;
	if (m_fFlowTime >= 1000000.f)
		m_fFlowTime = 0.f;
}

void CWater::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CWater::Render()
{
	if (FAILED(m_pShader->Bind_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fTime", &m_fFlowTime, sizeof(_float))))
		return E_FAIL;

	Bind_ShaderResources();

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();
} 

HRESULT CWater::Bind_ShaderResources()
{

	if (FAILED(m_pShader->Bind_RawValue("g_fFlowSpeed", &m_tWater_Descs.vFlowSpeed, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fVoronoiScale", &m_tWater_Descs.fVoronoiScale, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fWaterNoiseScale", &m_tWater_Descs.fWaterNoiseScale, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vWaterColor", &m_tWater_Descs.fWaterColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vLightColor", &m_tWater_Descs.fLightColor, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

HRESULT CWater::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_Water"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CWater* CWater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWater* pInstance = new CWater(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWater"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWater::Clone(void* pArg)
{
	CWater* pInstance = new CWater(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWater"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWater::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}

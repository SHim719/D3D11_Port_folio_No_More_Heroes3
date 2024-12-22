#include "Mozaic_Quad.h"

CMozaic_Quad::CMozaic_Quad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMozaic_Quad::CMozaic_Quad(const CMozaic_Quad& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMozaic_Quad::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMozaic_Quad::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	m_pTransform->Set_Scale({ 150.f, 150.f, 1.f });
	m_pTransform->Set_Position(XMVectorSet(30.f, -110.f, 1.f, 1.f));

	return S_OK;
}

void CMozaic_Quad::PriorityTick(_float fTimeDelta)
{
}

void CMozaic_Quad::Tick(_float fTimeDelta)
{

}

void CMozaic_Quad::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_STENCIL, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_MOZAIC, this);
}

HRESULT CMozaic_Quad::Render()
{
	if (FAILED(m_pShader->Bind_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();
}


HRESULT CMozaic_Quad::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxQuad"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CMozaic_Quad* CMozaic_Quad::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMozaic_Quad* pInstance = new CMozaic_Quad(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMozaic_Quad"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMozaic_Quad::Clone(void* pArg)
{
	CMozaic_Quad* pInstance = new CMozaic_Quad(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMozaic_Quad"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMozaic_Quad::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
}

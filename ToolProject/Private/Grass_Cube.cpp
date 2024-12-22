#include "Grass_Cube.h"

CGrass_Cube::CGrass_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGrass_Cube::CGrass_Cube(const CGrass_Cube& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGrass_Cube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrass_Cube::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_Scale(_float3(1.5f, 1.5f, 1.5f));

	return S_OK;
}

void CGrass_Cube::PriorityTick(_float fTimeDelta)
{
}

void CGrass_Cube::Tick(_float fTimeDelta)
{
}

void CGrass_Cube::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CGrass_Cube::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass_Cube::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"),
		TEXT("Transform"), reinterpret_cast<CComponent**>(&m_pTransform))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxBlock"),
		TEXT("Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Cube"),
		TEXT("VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Texture_Grass_Cube"),
		TEXT("Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
		return E_FAIL;

	if (FAILED(m_pTexture->Set_SRV(m_pShader, "g_BlockTexture")))
		return E_FAIL;

	return S_OK;
}

CGrass_Cube* CGrass_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGrass_Cube* pInstance = new CGrass_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGrass_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrass_Cube::Clone(void* pArg)
{
	CGrass_Cube* pInstance = new CGrass_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGrass_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrass_Cube::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);
}

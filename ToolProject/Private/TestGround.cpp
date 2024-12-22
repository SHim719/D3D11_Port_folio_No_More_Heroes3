#include "TestGround.h"


CTestGround::CTestGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTestGround::CTestGround(const CTestGround& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestGround::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), To_Radian(90.f));
	m_pTransform->Set_Scale({ 10.f, 10.f, 1.f });
	m_pTransform->Set_Position(XMVectorSet(0.f, 0.f, 5.f, 1.f));

	return S_OK;
}

void CTestGround::Tick(_float fTimeDelta)
{
}

void CTestGround::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTestGround::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;
	
	if (FAILED(m_pTexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;
	
	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;
	
	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestGround::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxNorTex"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect_NorTex"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Texture_TestGround"), TEXT("Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

CTestGround* CTestGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestGround* pInstance = new CTestGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTestGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestGround::Clone(void* pArg)
{
	CTestGround* pInstance = new CTestGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTestGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestGround::Free()
{
	__super::Free();

	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}

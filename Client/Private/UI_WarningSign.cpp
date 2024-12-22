#include "UI_WarningSign.h"

#include "AW_Manager.h"

CUI_WarningSign::CUI_WarningSign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_WarningSign::CUI_WarningSign(const CUI_WarningSign& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_WarningSign::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WarningSign::Initialize(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale({ 1000.f, 200.f, 1.f });

	PLAY_SOUND("Warning", false, 1.f);

	return S_OK;
}

void CUI_WarningSign::Tick(_float fTimeDelta)
{
	m_fFlickerAcc += fTimeDelta;
	if (m_fFlickerAcc >= m_fFlickerTime)
	{
		m_fFlickerAcc = 0.f;
		m_bRender = !m_bRender;
	}

	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
		Set_Destroy(true);
}

void CUI_WarningSign::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_WarningSign::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTexture->Set_SRV(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();

	return S_OK;
}



HRESULT CUI_WarningSign::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Texture_Warning_Sign"), TEXT("Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;


	return S_OK;
}

CUI_WarningSign* CUI_WarningSign::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WarningSign* pInstance = new CUI_WarningSign(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_WarningSign"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WarningSign::Clone(void* pArg)
{
	CUI_WarningSign* pInstance = new CUI_WarningSign(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_WarningSign"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WarningSign::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);

}

#include "UI_AbilitySelect_AW.h"

CUI_AbilitySelect_AW::CUI_AbilitySelect_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_AbilitySelect_AW::CUI_AbilitySelect_AW(const CUI_AbilitySelect_AW& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_AbilitySelect_AW::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_AbilitySelect_AW::Initialize(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale({ 253.f, 403.f, 1.f });

	return S_OK;
}

void CUI_AbilitySelect_AW::Tick(_float fTimeDelta)
{
    
}

void CUI_AbilitySelect_AW::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_AbilitySelect_AW::Render()
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
}


HRESULT CUI_AbilitySelect_AW::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Texture_Ability_Select"), TEXT("Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

CUI_AbilitySelect_AW* CUI_AbilitySelect_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AbilitySelect_AW* pInstance = new CUI_AbilitySelect_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_AbilitySelect_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_AbilitySelect_AW::Clone(void* pArg)
{
	CUI_AbilitySelect_AW* pInstance = new CUI_AbilitySelect_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_LevelProgress_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_AbilitySelect_AW::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
}

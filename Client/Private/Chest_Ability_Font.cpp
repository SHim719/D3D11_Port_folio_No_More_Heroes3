#include "Chest_Ability_Font.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"
#include "CustomFont.h"
#include "Chest_Ability_Font.h"

#include "Bike_AW.h"

CChest_Ability_Font::CChest_Ability_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CChest_Ability_Font::CChest_Ability_Font(const CChest_Ability_Font& rhs)
	: CGameObject(rhs)
{
}


HRESULT CChest_Ability_Font::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChest_Ability_Font::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_Scale(4.f);

	return S_OK;
}

void CChest_Ability_Font::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_AFTER_POSTPROCESS, this);
}


HRESULT CChest_Ability_Font::Render()
{
	m_pGameInstance->Render_TextToTarget(L"Font_AW", m_wstrText, { 200.f, 200.f }, XMVectorSet(1.f, 0.7f, 0.f, 1.f), CCustomFont::FontAlignCenter);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Text", m_pShader, "g_FontTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();
}



HRESULT CChest_Ability_Font::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxQuad"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CChest_Ability_Font* CChest_Ability_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChest_Ability_Font* pInstance = new CChest_Ability_Font(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CChest_Ability_Font"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChest_Ability_Font::Clone(void* pArg)
{
	CChest_Ability_Font* pInstance = new CChest_Ability_Font(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChest_Ability_Font"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest_Ability_Font::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}

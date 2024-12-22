#include "UI_Ability_Icon.h"

#include "UI_AbilityWindow_AW.h"

CUI_Ability_Icon::CUI_Ability_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Ability_Icon::CUI_Ability_Icon(const CUI_Ability_Icon& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Ability_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Ability_Icon::Initialize(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale({ 150.f, 150.f, 1.f });

	return S_OK;
}

void CUI_Ability_Icon::Tick(_float fTimeDelta)
{
	m_fRatio += m_fSpeed * fTimeDelta;
	if (m_fRatio >= 1.f) {
		m_fRatio = 0.f;
		m_iNowIconIdx = (m_iNowIconIdx + 1) % REWARD_TYPE_END;
	}
}

void CUI_Ability_Icon::LateTick(_float fTimeDelta)
{
}

HRESULT CUI_Ability_Icon::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;


	if (m_bRolling)
	{
		size_t iNextIconIdx = (m_iNowIconIdx + 1) % REWARD_TYPE_END;

		vector<CTexture*>& IconTextures = CUI_AbilityWindow_AW::Get_AbilityIcons();

		if (FAILED(IconTextures[m_iNowIconIdx]->Set_SRV(m_pShader, "g_PachinkoTexture1")))
			return E_FAIL;

		if (FAILED(IconTextures[iNextIconIdx]->Set_SRV(m_pShader, "g_PachinkoTexture2")))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_fPachinkoRatio", &m_fRatio, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(11)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Set_SRV(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Ability_Icon::Start_Rolling()
{
	m_iNowIconIdx = (size_t)JoRandom::Random_Int(0, REWARD_TYPE_END - 1);
	m_bRolling = true;
}

void CUI_Ability_Icon::End_Rolling(size_t iIdx)
{
	m_iNowIconIdx = iIdx;
	m_bRolling = false;

	vector<CTexture*>& IconTextures = CUI_AbilityWindow_AW::Get_AbilityIcons();
	m_pTexture = IconTextures[m_iNowIconIdx];
	Safe_AddRef(m_pTexture);
}

void CUI_Ability_Icon::Release_Texture()
{
	Safe_Release(m_pTexture);
}

HRESULT CUI_Ability_Icon::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CUI_Ability_Icon* CUI_Ability_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Ability_Icon* pInstance = new CUI_Ability_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Ability_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Ability_Icon::Clone(void* pArg)
{
	CUI_Ability_Icon* pInstance = new CUI_Ability_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Ability_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Ability_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
}

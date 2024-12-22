#include "UI_Ability_Section.h"
#include "UI_Ability_Icon.h"

#include "UI_AbilityWindow_AW.h"

#include "CustomFont.h"

_float4 CUI_Ability_Section::s_IconPoses[3] = { { -306.f, 10.f, 1.f, 1.f }, { 0.f, 10.f, 1.f, 1.f }, { 288.f, 10.f, 1.f, 1.f } };
_float2 CUI_Ability_Section::s_TitlePoses[3] = { { 334.f, 195.f, }, { 640.f, 195.f }, { 928.f, 195.f } };
_float2 CUI_Ability_Section::s_AbilityInfoPoses[3] = { { 334.f, 430.f, }, { 640.f, 430.f }, { 928.f, 430.f} };


CUI_Ability_Section::CUI_Ability_Section(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Ability_Section::CUI_Ability_Section(const CUI_Ability_Section& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Ability_Section::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Ability_Section::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_pIcon = static_cast<CUI_Ability_Icon*>(m_pGameInstance->Clone_GameObject(L"Prototype_UI_Ability_Icon"));
	if (nullptr == m_pIcon)
		return E_FAIL;

	_uint* iIndex = (_uint*)pArg;

	m_pIcon->Get_Transform()->Set_Position(XMLoadFloat4(&s_IconPoses[*iIndex]));

	m_iSeqIdx = *iIndex;


	return S_OK;
}

void CUI_Ability_Section::Tick(_float fTimeDelta)
{
	m_pIcon->Tick(fTimeDelta);

	m_fTitleChangeAcc -= fTimeDelta;

	if (m_fTitleChangeAcc <= 0.f)
	{
		vector<wstring>& vecAbilityTitles = CUI_AbilityWindow_AW::Get_AbilityTitles();
		m_fTitleChangeAcc = 0.1f;
		m_wstrAbilityTitle = vecAbilityTitles[m_iNowIdx];
		m_iNowIdx = (m_iNowIdx + 1) % REWARD_TYPE_END;
	}


}


HRESULT CUI_Ability_Section::Render()
{
	if (FAILED(m_pIcon->Render()))
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pGameInstance->Render_Font(L"FontUIBold", m_wstrAbilityTitle, s_TitlePoses[m_iSeqIdx], 1.5f, XMVectorSet(0.5f, 0.5f, 0.5f, 1.f), 0.f,
		CCustomFont::FontAlignCenter);

	if (false == m_bRolling)
	{
		m_pGameInstance->Render_Font(L"FontUIBold", m_wstrAbilityInfo, s_AbilityInfoPoses[m_iSeqIdx], 1.2f, XMVectorSet(0.5f, 0.5f, 0.5f, 1.f), 0.f,
			CCustomFont::FontAlignCenter);
	}
	

	return S_OK;
}

void CUI_Ability_Section::Start_Rolling()
{
	m_pIcon->Start_Rolling();

	m_bRolling = true;
}

void CUI_Ability_Section::End_Rolling()
{
	m_iNowIdx = (size_t)JoRandom::Random_Int(0, REWARD_TYPE_END - 1);

	m_pIcon->End_Rolling(m_iNowIdx);

	vector<wstring>& wstrAbilityTitles = CUI_AbilityWindow_AW::Get_AbilityTitles();
	vector<wstring>& wstrAbilityInfos = CUI_AbilityWindow_AW::Get_AbilityInfos();

	m_wstrAbilityTitle = wstrAbilityTitles[m_iNowIdx];
	m_wstrAbilityInfo = wstrAbilityInfos[m_iNowIdx];

	m_bRolling = false;

}

void CUI_Ability_Section::End_Menu()
{
	m_pIcon->Release_Texture();

}


CUI_Ability_Section* CUI_Ability_Section::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Ability_Section* pInstance = new CUI_Ability_Section(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Ability_Section"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Ability_Section::Clone(void* pArg)
{
	CUI_Ability_Section* pInstance = new CUI_Ability_Section(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_LevelProgress_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Ability_Section::Free()
{
	__super::Free();

	Safe_Release(m_pIcon);
}

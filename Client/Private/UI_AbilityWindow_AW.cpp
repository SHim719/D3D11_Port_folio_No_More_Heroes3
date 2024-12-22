#include "UI_AbilityWindow_AW.h"

#include "UI_AbilitySelect_AW.h"
#include "UI_Ability_Section.h"

#include "AW_Manager.h"


vector<CTexture*>	CUI_AbilityWindow_AW::s_AbilityIcons{};
vector<wstring>		CUI_AbilityWindow_AW::s_AbilityTitles{};
vector<wstring>		CUI_AbilityWindow_AW::s_AbilityInfos{};

CUI_AbilityWindow_AW::CUI_AbilityWindow_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_AbilityWindow_AW::CUI_AbilityWindow_AW(const CUI_AbilityWindow_AW& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_AbilityWindow_AW::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_AbilityWindow_AW::Initialize(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale({ 887.f, 547.f, 1.f });

	m_pSelectUI = static_cast<CUI_AbilitySelect_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_UI_AbilitySelect_AW"));
	if (nullptr == m_pSelectUI)
		return E_FAIL;

	m_vAbilityPoses[0] = { -293.f, -7.f, 0.f, 1.f };
	m_vAbilityPoses[1] = { 1.f, -7.f, 0.f, 1.f };
	m_vAbilityPoses[2] = { 294.f, -7.f, 0.f, 1.f };

	for (_uint i = 0; i < 3; ++i)
	{
		m_pAbilitySections[i] = static_cast<CUI_Ability_Section*>(m_pGameInstance->Clone_GameObject(L"Prototype_UI_Ability_Section", &i));
	}
		

	if (FAILED(Init_Icons()))
		return E_FAIL;

	if (FAILED(Init_Titles()))
		return E_FAIL;

	if (FAILED(Init_Infos()))
		return E_FAIL;
	

	return S_OK;
}

void CUI_AbilityWindow_AW::Tick(_float fTimeDelta)
{
	if (m_bAppearing)
	{
		if (!m_bRollingSound)
		{
			PLAY_SOUND("SE_sys_retry_roulette_roll", true, 0.5f);

			m_bRollingSound = true;
		}
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc >= 3.f)
		{
			STOP_SOUND("SE_sys_retry_roulette_roll");
			PLAY_SOUND("SE_sys_roulette_bonus777", false, 1.f);

			m_fTimeAcc = 0.f;
			m_bAppearing = false;
			m_bRollingSound = false;

			for (_uint i = 0; i < 3; ++i)
				m_pAbilitySections[i]->End_Rolling();
		}
		else
		{
			for (_uint i = 0; i < 3; ++i)
				m_pAbilitySections[i]->Tick(fTimeDelta);
		}
	}
	else
	{
		Move_AbilityIdx();
		Select_AbilityIdx();
	}
}

void CUI_AbilityWindow_AW::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_AbilityWindow_AW::Render()
{
	if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTexture->Set_SRV(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	for (size_t i = 0; i < 3; ++i)
		m_pAbilitySections[i]->Render();


	if (false == m_bAppearing)
	{
		if (FAILED(m_pSelectUI->Render()))
			return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CUI_AbilityWindow_AW::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;

	m_iSelectIdx = 0;

	m_pSelectUI->Get_Transform()->Set_Position(XMLoadFloat4(&m_vAbilityPoses[m_iSelectIdx]));

	m_bAppearing = true;

	for (size_t i = 0; i < 3; ++i)
		m_pAbilitySections[i]->Start_Rolling();


	return S_OK;
}

void CUI_AbilityWindow_AW::OnExit_Layer()
{
	for (_uint i = 0; i < 3; ++i)
		m_pAbilitySections[i]->End_Menu();
}

HRESULT CUI_AbilityWindow_AW::Init_Icons()
{
	vector<wstring> wstrIconTags
	{
		L"Prototype_Texture_Icon_Stronger_Bullet",
		L"Prototype_Texture_Icon_Speed_Up",
		L"Prototype_Texture_Icon_Stronger_Dragon"
	};

	s_AbilityIcons.resize(wstrIconTags.size());

	for (size_t i = 0; i < wstrIconTags.size(); ++i)
	{
		s_AbilityIcons[i] = static_cast<CTexture*>(m_pGameInstance->Clone_Component(GET_CURLEVEL, wstrIconTags[i]));
		if (nullptr == s_AbilityIcons[i])
			return E_FAIL;
	}
		

	return S_OK;
}

HRESULT CUI_AbilityWindow_AW::Init_Titles()
{
	s_AbilityTitles = {
		L"»Ð»Ð»Ð!",
		L"¾Ñ ¶ß°Å!",
		L"½¹½¹½¹~",
	};

	return S_OK;
}

HRESULT CUI_AbilityWindow_AW::Init_Infos()
{
	s_AbilityInfos = {
		L"Æ®·¹ºñ½ºÀÇ ÃÑ¾ËÀÌ\n°­È­µË´Ï´Ù.",
		L"¿ëÀÇ ¹ß»çÃ¼°¡\n°­È­µË´Ï´Ù.",
		L"º¸ÀÌ¸é ¼±ÅÃÇÏ¼¼¿ä.",
	};

	return S_OK;
}

void CUI_AbilityWindow_AW::Move_AbilityIdx()
{
	if (KEY_DOWN(eKeyCode::Left))
	{
		PLAY_SOUND("SE_sys_common_cursor", false, 1.f);

		m_iSelectIdx = (m_iSelectIdx + 3 - 1) % 3;
		m_pSelectUI->Get_Transform()->Set_Position(XMLoadFloat4(&m_vAbilityPoses[m_iSelectIdx]));
	}
	
	else if (KEY_DOWN(eKeyCode::Right))
	{
		PLAY_SOUND("SE_sys_common_cursor", false, 1.f);

		m_iSelectIdx = (m_iSelectIdx + 1) % 3;
		m_pSelectUI->Get_Transform()->Set_Position(XMLoadFloat4(&m_vAbilityPoses[m_iSelectIdx]));
	}

}

void CUI_AbilityWindow_AW::Select_AbilityIdx()
{
	if (KEY_DOWN(eKeyCode::Enter))
	{
		PLAY_SOUND("SE_sys_roulette_stop3hit", false, 1.f);

		AWMGR->Adjust_Reward((REWARD_TYPE)m_pAbilitySections[m_iSelectIdx]->Get_AbilityIdx());
		AWMGR->Set_Stop(false);
		m_bReturnToPool = true;
	}
}

HRESULT CUI_AbilityWindow_AW::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Texture_Ability_Window"), TEXT("Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

CUI_AbilityWindow_AW* CUI_AbilityWindow_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_AbilityWindow_AW* pInstance = new CUI_AbilityWindow_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_AbilityWindow_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_AbilityWindow_AW::Clone(void* pArg)
{
	CUI_AbilityWindow_AW* pInstance = new CUI_AbilityWindow_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_LevelProgress_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_AbilityWindow_AW::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pSelectUI);

	for (size_t i = 0; i < 3; ++i)
		Safe_Release(m_pAbilitySections[i]);

	for (auto& pTexture : s_AbilityIcons)
		Safe_Release(pTexture);

	s_AbilityIcons.clear();
}

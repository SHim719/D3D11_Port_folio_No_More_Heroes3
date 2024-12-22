#include "UI_Text_Ability.h"

#include "AW_Manager.h"
#include "Bike_AW.h"

#include "CustomFont.h"

CUI_Text_Ability::CUI_Text_Ability(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Text_Ability::CUI_Text_Ability(const CUI_Text_Ability& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Text_Ability::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Text_Ability::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CUI_Text_Ability::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= m_fLifeTime)
	{
		m_fTimeAcc = m_fLifeTime;
		m_bReturnToPool = true;
	}

	m_fNowScale = JoMath::Lerp(0.f, m_fTargetScale, m_fTimeAcc / m_fLifeTime);
	m_vTextPos.y = JoMath::Lerp(m_fStartY, m_fTargetY, m_fTimeAcc / m_fLifeTime);

}

void CUI_Text_Ability::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI_FONT, this);
}

HRESULT CUI_Text_Ability::Render()
{
	m_pGameInstance->Render_Font(L"Font_AW", m_wstrAbilityText, m_vTextPos, m_fNowScale, XMVectorSet(0.f, 0.f, 1.f, 1.f), 0.f,
		CCustomFont::FontAlignCenter);

	return S_OK;
}

HRESULT CUI_Text_Ability::OnEnter_Layer(void* pArg)
{
	m_wstrAbilityText = *(wstring*)pArg;

	_vector vPosition = AWMGR->Get_Player()->Get_Transform()->Get_Position();

	_vector vUIPos = Convert_WorldToScreen(vPosition);
	vUIPos.m128_f32[1] -= 200.f;

	m_fStartY = vUIPos.m128_f32[1];
	m_fTargetY = m_fStartY - 100.f;
	XMStoreFloat2(&m_vTextPos, vUIPos);

	m_fTimeAcc = 0.f;

	m_fTargetScale = JoRandom::Random_Float(0.5f, 0.7f);

	m_bReturnToPool = false;
	m_bInLayer = true;

	m_eState = SCALING;

	return S_OK;
}


CUI_Text_Ability* CUI_Text_Ability::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Text_Ability* pInstance = new CUI_Text_Ability(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Text_Ability"));
		Safe_Release(pInstance);
	}

	return pInstance;
}       

CGameObject* CUI_Text_Ability::Clone(void* pArg)
{
	CUI_Text_Ability* pInstance = new CUI_Text_Ability(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Text_Ability"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Text_Ability::Free()
{
	__super::Free();


}

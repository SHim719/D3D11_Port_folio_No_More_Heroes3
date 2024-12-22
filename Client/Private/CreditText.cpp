#include "CreditText.h"
#include "CustomFont.h"


CCreditText::CCreditText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCreditText::CCreditText(const CCreditText& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCreditText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCreditText::Initialize(void* pArg)
{
	m_wstrCreditText =
	{
		L"      쥬산 143기\n\n\n\n\n\n팀장: 공병학 - 플레이어, 카메라\n\n\n\n팀원: 구하람 - 이펙트\n\n\n\n팀원: 박경현 - UI\n\n\n\n팀원: 서동재 - 맵\n\n\n\n팀원: 심재욱 - 프레임워크, 쉐이더\n\n\n\n팀원: 안유진 - 맵\n\n\n\n팀원: 윤정섭 - 몬스터\n\n\n\n팀원: 최윤영 - UI\n\n     시청해주셔서 감사합니다!"
	};
	return S_OK;
}

void CCreditText::Tick(_float fTimeDelta)
{
	m_vTextPos.y -= fTimeDelta * m_fCreditSpeed;
}

void CCreditText::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI_FONT, this);
}

HRESULT CCreditText::Render()
{
	m_pGameInstance->Render_Font(L"BasicFont", m_wstrCreditText, m_vTextPos, 1.f, {1.f, 1.f, 1.f, 1.f}, 0.f, CCustomFont::FontAlignCenter);
	return S_OK;
}

CCreditText* CCreditText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCreditText* pInstance = new CCreditText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CCreditText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCreditText::Clone(void* pArg)
{
	CCreditText* pInstance = new CCreditText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCreditText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCreditText::Free()
{
	__super::Free();
}

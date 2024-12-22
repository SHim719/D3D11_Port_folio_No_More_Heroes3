#include "FadeScreen.h"

CFadeScreen::CFadeScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CFadeScreen::CFadeScreen(const CFadeScreen& rhs)
	: CUI(rhs)
{
}

HRESULT CFadeScreen::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CFadeScreen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer_Point)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_FadeScreen"), TEXT("Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	m_pTransform->Set_Scale({ _float(g_iWinSizeX), _float(g_iWinSizeY), 0.f });


	return S_OK;
}

void CFadeScreen::Tick(_float fTimeDelta)
{
	switch (m_eFadeState)
	{
	case CFadeScreen::FADEOUT:
	{
		m_fAlpha += m_fFadeOutSpeed * fTimeDelta;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			if (m_Callback_FadeOutEnd)
			{
				m_Callback_FadeOutEnd();
				m_Callback_FadeOutEnd = nullptr;
			}
				
			m_eFadeState = EXTRA;
		}
		break;
	}
	case CFadeScreen::EXTRA:
	{
		m_fExtraTime -= fTimeDelta;
		if (m_fExtraTime < 0.f)
		{
			if (m_Callback_FadeInStart)
			{
				m_Callback_FadeInStart();
				m_Callback_FadeInStart = nullptr;
			}
				
			m_eFadeState = FADEIN;
		}
	}
		break;
	case CFadeScreen::FADEIN:
		m_fAlpha -= m_fFadeInSpeed * fTimeDelta;
		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;

			if (m_Callback_FadeInEnd)
			{
				m_Callback_FadeInEnd();
				m_Callback_FadeInEnd = nullptr;
			}
			
			m_bReturnToPool = true;
		}
		break;
	}

	
}

void CFadeScreen::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CFadeScreen::Render()
{
	if (FAILED(m_pShader->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vFadeColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Point->Bind_Buffers()))
		return E_FAIL;
	
	if (FAILED(m_pVIBuffer_Point->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFadeScreen::OnEnter_Layer(void* pArg)
{
	if (FAILED(__super::OnEnter_Layer(pArg)))
		return E_FAIL;

	FADEDESC* pFadeDesc = (FADEDESC*)pArg;

	m_vFadeColor = pFadeDesc->eFadeColor == BLACK ? _float4(0.f, 0.f, 0.f, 1.f) : _float4(1.f, 1.f, 1.f, 1.f);

	m_fFadeOutSpeed = pFadeDesc->fFadeOutSpeed;
	m_fFadeInSpeed = pFadeDesc->fFadeInSpeed;
	m_fExtraTime = pFadeDesc->fExtraTime;
	m_fAlpha = 0.f;
	m_eFadeState = FADEOUT;

	if (pFadeDesc->pCallback_FadeOutStart)
		m_Callback_FadeOutStart = move(pFadeDesc->pCallback_FadeOutStart);

	if (pFadeDesc->pCallback_FadeOutEnd)
		m_Callback_FadeOutEnd = move(pFadeDesc->pCallback_FadeOutEnd);

	if (pFadeDesc->pCallback_FadeInStart)
		m_Callback_FadeInStart = move(pFadeDesc->pCallback_FadeInStart);

	if (pFadeDesc->pCallback_FadeInEnd)
		m_Callback_FadeInEnd = move(pFadeDesc->pCallback_FadeInEnd);

	if (m_Callback_FadeOutStart)
	{
		m_Callback_FadeOutStart();
		m_Callback_FadeOutStart = nullptr;
	}

	return S_OK;
}

CFadeScreen* CFadeScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFadeScreen* pInstance = new CFadeScreen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFadeScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CFadeScreen::Clone(void* pArg)
{
	CFadeScreen* pInstance = new CFadeScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFadeScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFadeScreen::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Point);
}

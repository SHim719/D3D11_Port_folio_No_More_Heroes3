#include "HP_Number.h"

#include "AW_Manager.h"

#include "CustomFont.h"


CHP_Number::CHP_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CHP_Number::CHP_Number(const CHP_Number& rhs)
	: CGameObject(rhs)
{
}


HRESULT CHP_Number::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHP_Number::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransform->Set_Scale(5.f);

	return S_OK;
}

void CHP_Number::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_AFTER_POSTPROCESS, this);
}


HRESULT CHP_Number::Render()
{
	m_pGameInstance->Render_TextToTarget(L"Font_AW", m_wstrCurHp, { 200.f, 200.f }, XMVectorSet(0.941f, 0.337f, 0.313f, 1.f), CCustomFont::FontAlignCenter);

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

void CHP_Number::Update_Position(_fvector vPosition)
{
	m_pTransform->Set_Position(vPosition);
	m_pTransform->Add_Position(XMLoadFloat4(&m_vOffset), true);
}



HRESULT CHP_Number::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxQuad"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CHP_Number* CHP_Number::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHP_Number* pInstance = new CHP_Number(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHP_Number"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHP_Number::Clone(void* pArg)
{
	CHP_Number* pInstance = new CHP_Number(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHP_Number"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHP_Number::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
}
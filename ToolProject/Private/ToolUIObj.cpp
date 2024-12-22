#include "ToolUIObj.h"

CToolUIObj::CToolUIObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolUIObj::CToolUIObj(const CToolUIObj& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolUIObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolUIObj::Initialize(void* pArg)
{
	if (nullptr == pArg)
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	else
	{
		UI_DESC* UIDesc = static_cast<UI_DESC*>(pArg);
	
		m_tUIDesc.eUIType = UIDesc->eUIType;
		m_tUIDesc.WorldMatrix = UIDesc->WorldMatrix;
		m_tUIDesc.iPass = UIDesc->iPass;
	
		lstrcpy(m_tUIDesc.szProCom, UIDesc->szProCom);
		lstrcpy(m_tUIDesc.szLayer, UIDesc->szLayer);
		lstrcpy(m_tUIDesc.szTexture, UIDesc->szTexture);

		if (m_tUIDesc.eUIType == UI_INSTANCE)
		{
			UI_INSTANCE_DESC* UIDesc = static_cast<UI_INSTANCE_DESC*>(pArg);

			m_bColorUse = true;
			m_vColor = UIDesc->vColor;
			m_bAlpha = UIDesc->iAlpha == 1? true : false;
		}
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_tUIDesc.WorldMatrix));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CToolUIObj::Tick(_float fTimeDelta)
{
}

void CToolUIObj::LateTick(_float fTimeDelta)
{
	if(m_bAlpha)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CToolUIObj::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vColorUse", &m_bColorUse, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_tUIDesc.iPass)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CToolUIObj::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_UI"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_UITOOL, m_tUIDesc.szTexture, TEXT("Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

CToolUIObj* CToolUIObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolUIObj* pInstance = new CToolUIObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolUIObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolUIObj::Clone(void* pArg)
{
	CToolUIObj* pInstance = new CToolUIObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolUIObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolUIObj::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pUITexture);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBufferInstance);
}
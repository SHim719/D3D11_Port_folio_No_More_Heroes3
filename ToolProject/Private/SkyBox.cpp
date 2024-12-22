#include "SkyBox.h"

CSkyBox::CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components(*(wstring*)pArg)))
		return E_FAIL;

	m_pTransform->Set_Scale(_float3(20.f, 20.f, 20.f));

	return S_OK;
}

void CSkyBox::PriorityTick(_float fTimeDelta)
{
}

void CSkyBox::Tick(_float fTimeDelta)
{

}

void CSkyBox::LateTick(_float fTimeDelta)
{
	m_pTransform->Set_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
	static _bool b = { false };
	
	if (KEY_DOWN(eKeyCode::F6))
		b = true == b ? false : true;
	if(b)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSkyBox::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTexture->Set_SRV(m_pShader, "g_SkyBoxTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CSkyBox::Ready_Components(const wstring& wstrTextureTag)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"),
		TEXT("Transform"), reinterpret_cast<CComponent**>(&m_pTransform))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxCube"),
		TEXT("Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Cube"),
		TEXT("VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, wstrTextureTag, TEXT("Texture"), reinterpret_cast<CComponent**>(&m_pTexture))))
		return E_FAIL;

	return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkyBox* pInstance = new CSkyBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkyBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyBox::Clone(void* pArg)
{
	CSkyBox* pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSkyBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyBox::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);

}

#include "Chest_Down.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"
#include "CustomFont.h"
#include "Chest_Down.h"

#include "Bike_AW.h"

CChest_Down::CChest_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CChest_Down::CChest_Down(const CChest_Down& rhs)
	: CGameObject(rhs)
{
}


HRESULT CChest_Down::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChest_Down::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_Scale(2.f);

	return S_OK;
}

void CChest_Down::Tick(_float fTimeDelta)
{
	
}


HRESULT CChest_Down::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_int iTextureFlag = 0;

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_iTextureFlag", &iTextureFlag, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CChest_Down::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Chest_Down"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CChest_Down* CChest_Down::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChest_Down* pInstance = new CChest_Down(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CChest_Down"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChest_Down::Clone(void* pArg)
{
	CChest_Down* pInstance = new CChest_Down(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChest_Down"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest_Down::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}

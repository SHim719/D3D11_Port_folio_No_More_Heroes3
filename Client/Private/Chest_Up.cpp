#include "Chest_Up.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"
#include "CustomFont.h"
#include "Chest_Up.h"

#include "Bike_AW.h"

CChest_Up::CChest_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CChest_Up::CChest_Up(const CChest_Up& rhs)
	: CGameObject(rhs)
{
}


HRESULT CChest_Up::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChest_Up::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_RotationSpeed(m_fRotationSpeed);

	m_pTransform->Set_Scale(2.f);

	return S_OK;
}

void CChest_Up::Tick(_float fTimeDelta)
{
	if (OPENING == m_eState)
	{
		m_pTransform->Turn(m_pTransform->Get_Right(), fTimeDelta);
		m_fAngle += m_fRotationSpeed * fTimeDelta;
		if (m_fAngle >= 60.f)
			m_eState = IDLE;
	}
}


HRESULT CChest_Up::Render()
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

void CChest_Up::Init_Spawn()
{
	m_fAngle = 0.f;
	m_eState = IDLE;
	m_pTransform->Rotation_Quaternion(XMQuaternionIdentity());


}


HRESULT CChest_Up::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Chest_Up"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CChest_Up* CChest_Up::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChest_Up* pInstance = new CChest_Up(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CChest_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChest_Up::Clone(void* pArg)
{
	CChest_Up* pInstance = new CChest_Up(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChest_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest_Up::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}

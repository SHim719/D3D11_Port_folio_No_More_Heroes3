#include "Tool_Weapon.h"

#include "Bone.h"

//#include "Effect_Trail.h"


CTool_Weapon::CTool_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTool_Weapon::CTool_Weapon(const CTool_Weapon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTool_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTool_Weapon::Initialize(void* pArg)
{
	WEAPONDESC* pWeaponDesc = (WEAPONDESC*) pArg;

	if (FAILED(Ready_Components(pWeaponDesc)))
		return E_FAIL;

	m_pSocketBone = pWeaponDesc->pSocketBone;
	m_pParentTransform = pWeaponDesc->pParentTransform;

	Safe_AddRef(m_pSocketBone);
	Safe_AddRef(m_pParentTransform);

	return S_OK;
}

void CTool_Weapon::Tick(_float fTimeDelta)
{
	_matrix SocketMatrix = m_pSocketBone->Get_CombinedTransformation();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransform->Set_WorldMatrix(SocketMatrix * m_pParentTransform->Get_WorldMatrix());
}

void CTool_Weapon::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTool_Weapon::Render()
{
	_uint iPassIndex = 0;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture");

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, iPassIndex)))
			return E_FAIL;
	}
	return S_OK;
}


HRESULT CTool_Weapon::Ready_Components(WEAPONDESC* pDesc)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (pDesc->wstrModelTag.size())
	{
		if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, pDesc->wstrModelTag, TEXT("Model"), (CComponent**)&m_pModel)))
			return E_FAIL;
	}

	return S_OK;
}


CTool_Weapon* CTool_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTool_Weapon* pInstance = new CTool_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTool_Weapon::Clone(void* pArg)
{
	CTool_Weapon* pInstance = new CTool_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTool_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pSocketBone);
	Safe_Release(m_pParentTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

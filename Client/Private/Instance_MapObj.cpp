#include "Instance_MapObj.h"

CInstance_MapObj::CInstance_MapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInstance_MapObj::CInstance_MapObj(const CInstance_MapObj& rhs)
	: CGameObject(rhs)
	, m_ModelDesc{ rhs.m_ModelDesc }
{
}

HRESULT CInstance_MapObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstance_MapObj::Initialize(void* pArg)
{
	INSTANCE_MAPOBJ_DESC* pDesc = static_cast<INSTANCE_MAPOBJ_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iLevel = pDesc->iLevel;
	m_strInstanceComponentTag = pDesc->strInstanceComponentTag;
	m_iObjFlag = pDesc->iObjFlag;
	m_fMipLevel = pDesc->fMipLevel;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInstance_MapObj::PriorityTick(_float fTimeDelta)
{
}

void CInstance_MapObj::Tick(_float fTimeDelta)
{
}

void CInstance_MapObj::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_ShadowObject(this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CInstance_MapObj::Render()
{
	_uint		iNumMeshes = m_pModel_Instance->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_int iTextureFlag = 0;

		if (FAILED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (SUCCEEDED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::NORMALS);

		if (SUCCEEDED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_ORM, "g_ORMTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::ORM);

		if (SUCCEEDED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_MRAO, "g_MRAOTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::MRAO);

		if (SUCCEEDED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_RMA, "g_RMATexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::RMA);

		if (SUCCEEDED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_METALNESS, "g_MetalTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::METAL);

		if (SUCCEEDED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_ROUGHNESS, "g_RoughnessTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::ROUGHNESS);

		if (SUCCEEDED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_EMISSIVE, "g_EmissiveTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::EMISSIVE);

		if (FAILED(m_pShader->Bind_RawValue("g_iTextureFlag", &iTextureFlag, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_fMipLevel", &m_fMipLevel, sizeof _float)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInstance_MapObj::Render_Cascade()
{
	_uint		iNumMeshes = m_pModel_Instance->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel_Instance->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->Render(m_pShader, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInstance_MapObj::Render_ShadowDepth()
{
	_uint		iNumMeshes = m_pModel_Instance->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel_Instance->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->Render(m_pShader, i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CInstance_MapObj::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, nullptr)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModelInstance"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iLevel, m_strInstanceComponentTag, TEXT("Model"), (CComponent**)&m_pModel_Instance)))
		return E_FAIL;

	return S_OK;
}

CInstance_MapObj* CInstance_MapObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstance_MapObj* pInstance = new CInstance_MapObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInstance_MapObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInstance_MapObj::Clone(void* pArg)
{
	CInstance_MapObj* pInstance = new CInstance_MapObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CInstance_MapObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstance_MapObj::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel_Instance);

	m_WorldMatrices.clear();
}

#include "Instancing_Object.h"


CInstancing_Object::CInstancing_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInstancing_Object::CInstancing_Object(const CInstancing_Object& rhs)
	: CGameObject(rhs)
{
}

HRESULT CInstancing_Object::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstancing_Object::Initialize(void* pArg)
{
	wstring* pStrModelTag = (wstring*)pArg;

	*pStrModelTag += L"_Instance";

	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Shader_VtxModelInstance"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelID(), *pStrModelTag, L"Model", (CComponent**)&m_pModel_Instance)))
		return E_FAIL;

	return S_OK;
}

void CInstancing_Object::LateTick(_float fTimeDelta)
{
	if (m_WorldMatrices.empty())
		return;

	m_pModel_Instance->Ready_Instancing(m_WorldMatrices, m_vecObjFlags);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CInstancing_Object::Render()
{
	_uint		iNumMeshes = m_pModel_Instance->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel_Instance->Render(m_pShader, i, 0)))
			return E_FAIL;
	}


	return S_OK;
}

CInstancing_Object* CInstancing_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstancing_Object* pInstance = new CInstancing_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInstancing_Object"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInstancing_Object::Clone(void* pArg)
{
	CInstancing_Object* pInstance = new CInstancing_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolMapObjInstance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstancing_Object::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel_Instance);

	m_WorldMatrices.clear();
}

#include "Travis_AW.h"

#include "Travis_World_Enums.h"

CTravis_AW::CTravis_AW(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CTravis_AW::CTravis_AW(const CTravis_AW& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CTravis_AW::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTravis_AW::Initialize(void* _pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_pModel->Change_Animation((_uint)WORLDTRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Vehicle_Neutral);
	
	return S_OK;
}

void CTravis_AW::PriorityTick(_float _fTimeDelta)
{

}

void CTravis_AW::Tick(_float _fTimeDelta)
{
	m_pModel->Play_Animation(_fTimeDelta);
}

void CTravis_AW::LateTick(_float _fTimeDelta)
{
	m_pModel->Update_BoneMatrices();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_ShadowObject(this);
}

HRESULT CTravis_AW::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_int iTextureFlag = 0;

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::NORMALS);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_MRAO, "g_MRAOTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::MRAO);

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, m_iPassIdx)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CTravis_AW::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Travis_World"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;


	m_pModel->Set_AnimPlay();

	return S_OK;
}

CTravis_AW* CTravis_AW::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTravis_AW* pInstance = new CTravis_AW(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(nullptr))) {
		MSG_BOX(L"Failed to Create : CTravis_AW");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTravis_AW::Clone(void* _pArg)
{
	CTravis_AW* pInstance = new CTravis_AW(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTravis_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_AW::Free()
{
	__super::Free();

}

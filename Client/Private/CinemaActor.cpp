#include "CinemaActor.h"
#include "Cinematic_Manager.h"


CCinemaActor::CCinemaActor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CCinemaActor::CCinemaActor(const CCinemaActor& _rhs)
	: CCharacter(_rhs)
{
}


HRESULT CCinemaActor::Initialize(void* _pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Initialize(nullptr);

	return S_OK;
}

void CCinemaActor::PriorityTick(_float _fTimeDelta)
{
}

void CCinemaActor::Tick(_float _fTimeDelta)
{
	Play_Cinematic(CINEMAMGR->Get_PlayTimeAcc());

	m_pModel->Play_Animation(_fTimeDelta);

	m_pModel->Update_BoneMatrices();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}


HRESULT CCinemaActor::Render()
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

		if (FAILED(m_pModel->Render(m_pShader, i, PASS_DEFAULT)))
			return E_FAIL;
	}

	return S_OK;
}

_bool CCinemaActor::Bind_CinemaEtcFunc()
{
	return true;
}


HRESULT CCinemaActor::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}



CCinemaActor* CCinemaActor::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinemaActor* pInstance = new CCinemaActor(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(nullptr))) {
		MSG_BOX(L"Failed to Create : CCinemaActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinemaActor::Clone(void* _pArg)
{
	CCinemaActor* pInstance = new CCinemaActor(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCinemaActor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinemaActor::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

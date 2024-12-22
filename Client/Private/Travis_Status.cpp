#include "Travis_Status.h"
#include "Travis_Enums.h"

CTravis_Status::CTravis_Status(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CTravis_Status::CTravis_Status(const CTravis_Status& _rhs)
	: CGameObject(_rhs)
{
}


HRESULT CTravis_Status::Initialize(void* _pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Add_YAxisInput(To_Radian(160.f));

	_float4 vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	_float	fLightStrength = 1.f;
	_float4 vLightDir = _float4(1.f, -1.f, 1.f, 0.f);

	m_pShader->Bind_RawValue("g_vLightDiffuse", &vLightDiffuse, sizeof(_float4));
	m_pShader->Bind_RawValue("g_fLightStrength", &fLightStrength, sizeof(_float));
	m_pShader->Bind_RawValue("g_vLightDir", &vLightDir, sizeof(_float4));

	CALC_TF->Set_Position(XMLoadFloat4(&m_vCamPosition));
	CALC_TF->LookAt2D(m_pTransform->Get_Position());

	XMStoreFloat4x4(&m_ViewMatrix, CALC_TF->Get_WorldMatrixInverse());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(To_Radian(55.f), 528.f / 404.f, 0.1f, 100.f));

	m_pModel->Change_Animation((_uint)TRAVIS_ANIMLIST::ANI_PL_Travis_IDL_Neutral);

	return S_OK;
}

void CTravis_Status::PriorityTick(_float _fTimeDelta)
{
}

void CTravis_Status::Tick(_float _fTimeDelta)
{
	m_pModel->Play_Animation(_fTimeDelta);

	m_pModel->Update_BoneMatrices();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_STATUS, this);
}


HRESULT CTravis_Status::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_vCamPosition, sizeof(_float4))))
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

		if (FAILED(m_pModel->Render(m_pShader, i, PASS_FORWARD_PBR)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CTravis_Status::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Travis"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}



CTravis_Status* CTravis_Status::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTravis_Status* pInstance = new CTravis_Status(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(nullptr))) {
		MSG_BOX(L"Failed to Create : CTravis_Status");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTravis_Status::Clone(void* _pArg)
{
	CTravis_Status* pInstance = new CTravis_Status(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTravis_Status"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_Status::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

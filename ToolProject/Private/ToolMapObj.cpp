#include "ToolMapObj.h"
#include "Tool_Weapon.h"

_uint CToolMapObj::m_iCurR = 0;
_uint CToolMapObj::m_iCurG = 0;
_uint CToolMapObj::m_iCurB = 0;
_uint CToolMapObj::m_iCurA = 0;

CToolMapObj::CToolMapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolMapObj::CToolMapObj(const CToolMapObj& rhs)
	: CGameObject(rhs)
	, m_ModelDesc{ rhs.m_ModelDesc }
{
}

HRESULT CToolMapObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolMapObj::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_ModelDesc.matModel = ((MODEL_DESC*)pArg)->matModel;
		m_ModelDesc.fRadius = ((MODEL_DESC*)pArg)->fRadius;
		m_ModelDesc.fMipLevel = ((MODEL_DESC*)pArg)->fMipLevel;
		strcpy_s(m_ModelDesc.strName, ((MODEL_DESC*)pArg)->strName);
		strcpy_s(m_ModelDesc.strNumbered, ((MODEL_DESC*)pArg)->strNumbered);
		strcpy_s(m_ModelDesc.strAddress, ((MODEL_DESC*)pArg)->strAddress);
		strcpy_s(m_ModelDesc.strFileLayer, ((MODEL_DESC*)pArg)->strFileLayer);
		m_ModelDesc.iObjFlag = ((MODEL_DESC*)pArg)->iObjFlag;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_fmatrix matModel = XMLoadFloat4x4(&m_ModelDesc.matModel);
	m_pTransform->Set_WorldMatrix(matModel);

	m_ModelDesc.vRGBValue = Generate_RGBValue();

	return S_OK;
}

void CToolMapObj::Tick(_float fTimeDelta)
{
	m_pSampleCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pSampleCollider->Set_Size(XMVectorSet(m_ModelDesc.fRadius, m_ModelDesc.fRadius, m_ModelDesc.fRadius, 1.f));
}

void CToolMapObj::LateTick(_float fTimeDelta)
{
	if (m_ModelDesc.iObjFlag & (1 << SHADOW))
		m_pGameInstance->Add_ShadowObject(this);

	_vector vPos = m_pTransform->Get_Position();

	if (m_pGameInstance->In_WorldFrustum(vPos, m_ModelDesc.fRadius * m_pTransform->Get_Scale().x))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_COLORPICKING, this);
	}

#ifdef _DEBUG
	if (m_bDebug)
		m_pGameInstance->Add_RenderComponent(m_pSampleCollider);

#endif
}

HRESULT CToolMapObj::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fMipLevel", &m_ModelDesc.fMipLevel, sizeof _float)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_int iTextureFlag = 0;

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::NORMALS);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_ORM, "g_ORMTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::ORM);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_MRAO, "g_MRAOTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::MRAO);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_RMA, "g_RMATexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::RMA);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_METALNESS, "g_MetalTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::METAL);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_ROUGHNESS, "g_RoughnessTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::ROUGHNESS);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_EMISSIVE, "g_EmissiveTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::EMISSIVE);

		if (FAILED(m_pShader->Bind_RawValue("g_iTextureFlag", &iTextureFlag, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CToolMapObj::Render_Picking()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vPickingColor", &m_ModelDesc.vRGBValue, sizeof _float4)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CToolMapObj::Render_ShadowDepth()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 3)))
			return E_FAIL;
	}

	return S_OK;
}

void CToolMapObj::Save_ModelData(HANDLE hFile, _ulong& dwByte)
{
	const _float4x4 WorldMatrix = m_pTransform->Get_WorldFloat4x4();

	WriteFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);
	WriteFile(hFile, &m_ModelDesc.fRadius, sizeof _float, &dwByte, nullptr);
	WriteFile(hFile, &m_ModelDesc.fMipLevel, sizeof _float, &dwByte, nullptr);

	DWORD iLen1 = static_cast<DWORD>(strlen(m_ModelDesc.strName));
	DWORD iLen2 = static_cast<DWORD>(strlen(m_ModelDesc.strNumbered));
	DWORD iLen3 = static_cast<DWORD>(strlen(m_ModelDesc.strAddress));
	DWORD iLen4 = static_cast<DWORD>(strlen(m_ModelDesc.strFileLayer));
	WriteFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen3, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen4, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, m_ModelDesc.strName, iLen1, &dwByte, nullptr);
	WriteFile(hFile, m_ModelDesc.strNumbered, iLen2, &dwByte, nullptr);
	WriteFile(hFile, m_ModelDesc.strAddress, iLen3, &dwByte, nullptr);
	WriteFile(hFile, m_ModelDesc.strFileLayer, iLen4, &dwByte, nullptr);
	WriteFile(hFile, &m_ModelDesc.iObjFlag, sizeof(_int), &dwByte, nullptr);
}

void CToolMapObj::Save_AddressData(HANDLE hFile, _ulong& dwByte)
{
	DWORD iLen1 = static_cast<DWORD>(strlen(m_ModelDesc.strName));
	DWORD iLen2 = static_cast<DWORD>(strlen(m_ModelDesc.strAddress));
	WriteFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, m_ModelDesc.strName, iLen1, &dwByte, nullptr);
	WriteFile(hFile, m_ModelDesc.strAddress, iLen2, &dwByte, nullptr);
}

_float4 CToolMapObj::Generate_RGBValue()
{
	_float4 vRGB = {};

	m_iCurR += 1;
	if (m_iCurR > 255)
	{
		m_iCurR = 0;
		
		m_iCurG += 1;
		if (m_iCurG > 255)
		{
			m_iCurG = 0;
			
			m_iCurB += 1;
			if (m_iCurB > 255)
			{
				m_iCurB = 0;
				
				m_iCurA += 1;
				if (m_iCurA > 255)
				{
					m_iCurA = 0;
				}
			}
		}
	}

	vRGB = _float4(
		static_cast<_float>(m_iCurR),
		static_cast<_float>(m_iCurG),
		static_cast<_float>(m_iCurB),
		static_cast<_float>(m_iCurA)
	);

	return vRGB;
}

HRESULT CToolMapObj::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	m_strComponentTag = L"Prototype_Model_" + Convert_StrToWStr(m_ModelDesc.strName);

	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, m_strComponentTag, L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDERDESC SampleDesc = {};
	SampleDesc.eType = CCollider::SPHERE;
	SampleDesc.pOwner = (CGameObject*)this;
	SampleDesc.vCenter = { 0.f, 0.f, 0.f };
	SampleDesc.vSize = { 1.f, 1.f, 1.f };
	SampleDesc.vRotation = { 0.f, 0.f, 0.f };
	SampleDesc.bActive = true;
	SampleDesc.strCollisionLayer = "Radius Sample";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Radius Sample"), (CComponent**)&m_pSampleCollider, &SampleDesc)))
		return E_FAIL;

	return S_OK;
}

CToolMapObj* CToolMapObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolMapObj* pInstance = new CToolMapObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolMapObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolMapObj::Clone(void* pArg)
{
	CToolMapObj* pInstance = new CToolMapObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolMapObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolMapObj::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
	Safe_Release(m_pSampleCollider);
}
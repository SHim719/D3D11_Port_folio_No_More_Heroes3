#include "ToolAnimObj.h"
#include "Tool_Weapon.h"


CToolAnimObj::CToolAnimObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CToolAnimObj::CToolAnimObj(const CToolAnimObj& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolAnimObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolAnimObj::Initialize(void* pArg)
{
	PrototypeCharacter* pType = (PrototypeCharacter*)pArg;
	m_eCharacter_Type = *pType;

	switch (*pType)
	{
		case TRAVIS:
		{
			if (FAILED(Ready_Travis()))
				return E_FAIL;
			
			break;
		}
		case TRAVIS_SPACE:
		{
			if (FAILED(Ready_Travis_Space()))
				return E_FAIL;
			break;
		}
		case TRAVIS_WORLD:
		{
			if (FAILED(Ready_Travis_World()))
				return E_FAIL;
			break;
		}
		case TRAVIS_ARMOR:
		{
			if (FAILED(Ready_Travis_Armor()))
				return E_FAIL;
			break;
		}
		case MBONE:
		{
			if (FAILED(Ready_Mbone()))
				return E_FAIL;
			break;
		}
		case MRBLACKHOLE_ARML:
		{
			if (FAILED(Ready_MrBlackhole_ArmL()))
				return E_FAIL;
			break;
		}
		case MRBLACKHOLE_ARMR:
		{
			if (FAILED(Ready_MrBlackhole_ArmR()))
				return E_FAIL;
			break;
		}
		case MRBLACKHOLE:
		{
			if (FAILED(Ready_MrBlackhole()))
				return E_FAIL;
			break;
		}
		case MRBLACKHOLE_SPACE:
		{
			if (FAILED(Ready_MrBlackhole_Space()))
				return E_FAIL;
			break;
		}
		case TREATMENT:
		{
			if (FAILED(Ready_Treatment()))
				return E_FAIL;
			break;
		}
		case FULLFACE:
		{
			if (FAILED(Ready_Fullface()))
				return E_FAIL;
			break;
		}
		case TRIPLEPUT:
		{
			if (FAILED(Ready_Tripleput()))
				return E_FAIL;
			break;
		}
		case SONICJUICE:
		{
			if (FAILED(Ready_SonicJuice()))
				return E_FAIL;
			break;
		}
		case TRAVIS_BIKE:
		{
			if (FAILED(Ready_TravisBike()))
				return E_FAIL;
			break;
		}
		case DAMON:
		{
			if (FAILED(Ready_Damon()))
				return E_FAIL;
			break;
		}
		case LEOPARDON:
		{
			if (FAILED(Ready_Leopardon()))
				return E_FAIL;
			break;
		}
		case DESTROYMANTF:
		{
			if (FAILED(Ready_DestroymanTF()))
				return E_FAIL;
			break;
		}
		case JEANE:
		{
			if (FAILED(Ready_Jeane()))
				return E_FAIL;
			break;
		}
	}

	m_pModel->Set_Preview(true);
	return S_OK;
}

void CToolAnimObj::Tick(_float fTimeDelta)
{
}

void CToolAnimObj::LateTick(_float fTimeDelta)
{
	m_pModel->Play_Animation(fTimeDelta);

	m_pModel->Update_BoneMatrices();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CToolAnimObj::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture");

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Travis()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Travis", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	CTool_Weapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Travis_BeamKatana";
	CGameObject* pWeapon = m_pGameInstance->Add_Clone(LEVEL_ANIMTOOL, L"Weapon", L"Prototype_ToolWeapon", &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Travis_Space()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_Space", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Travis_World()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_World", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Travis_Armor()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_Armor", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	CTool_Weapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Travis_Armor_Weapon";
	CGameObject* pWeapon = m_pGameInstance->Add_Clone(LEVEL_ANIMTOOL, L"Weapon", L"Prototype_ToolWeapon", &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Mbone()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Mbone", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	//CTool_Weapon::WEAPONDESC WeaponDesc;
	//WeaponDesc.pParentTransform = m_pTransform;
	//WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	//WeaponDesc.wstrModelTag = L"Prototype_Model_Mbone_BoneSpear";
	//CGameObject* pWeapon = m_pGameInstance->Add_Clone(LEVEL_ANIMTOOL, L"Weapon", L"Prototype_ToolWeapon", &WeaponDesc);
	//if (nullptr == pWeapon)
	//	return E_FAIL;


	return S_OK;
}

HRESULT CToolAnimObj::Ready_MBone_Weapon()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Mbone_BoneSpear", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_MrBlackhole()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_MrBlackhole_ArmL()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole_ArmL", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_MrBlackhole_ArmR()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole_ArmR", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_MrBlackhole_Space()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole_Space", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Treatment()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Treatment", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	CTool_Weapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Treatment_Tomahawk";
	CGameObject* pWeapon = m_pGameInstance->Add_Clone(LEVEL_ANIMTOOL, L"Weapon", L"Prototype_ToolWeapon", &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Fullface()
{
	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Fullface", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	CTool_Weapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Fullface_Sword";
	CGameObject* pWeapon = m_pGameInstance->Add_Clone(LEVEL_ANIMTOOL, L"Weapon", L"Prototype_ToolWeapon", &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Fullface_Sheath";
	pWeapon = m_pGameInstance->Add_Clone(LEVEL_ANIMTOOL, L"Weapon", L"Prototype_ToolWeapon", &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Tripleput()
{
	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Tripleput", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_SonicJuice()
{
	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_SonicJuice", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_TravisBike()
{

	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_Bike", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Damon()
{
	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_SmashBros_Damon", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolAnimObj::Ready_Leopardon()
{
	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_Leopardon", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;


	return S_OK;
}

HRESULT CToolAnimObj::Ready_DestroymanTF()
{
	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_DestroymanTF", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;


	return S_OK;
}

HRESULT CToolAnimObj::Ready_Jeane()
{
	CTransform::TRANSFORMDESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, L"Prototype_Model_NPC_Jeane", L"Model", (CComponent**)&m_pModel)))
		return E_FAIL;


	return S_OK;
}


CToolAnimObj* CToolAnimObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolAnimObj* pInstance = new CToolAnimObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CToolAnimObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolAnimObj::Clone(void* pArg)
{
	CToolAnimObj* pInstance = new CToolAnimObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CToolAnimObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolAnimObj::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
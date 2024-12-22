#include "Enemy_AW.h"

#include "AW_Manager.h"

#include "HP_Number.h"

#include "PlayerBullet_AW.h"
#include "Chest_AW.h"

#include "Bike_AW.h"
#include "Effect_Manager.h"


CEnemy_AW::CEnemy_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CEnemy_AW::CEnemy_AW(const CEnemy_AW& rhs)
	: CCharacter(rhs)
{
}


HRESULT CEnemy_AW::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;

	return S_OK;
}

HRESULT CEnemy_AW::Initialize(void* pArg)
{
	ENEMY_AW_DESC* pDesc = (ENEMY_AW_DESC*)pArg;

	if (FAILED(Ready_Components(pDesc->wstrModelTag)))
		return E_FAIL;

	/* Effect */
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	m_pTransform->Rotation(YAXIS, XM_PI);

	m_pHpNumber = static_cast<CHP_Number*>(m_pGameInstance->Clone_GameObject(L"Prototype_HP_Number"));
	if (nullptr == m_pHpNumber)
		return E_FAIL;

	m_pHpNumber->Set_Offset(XMVectorSet(0.f, 1.f, -1.f, 0.f));

	m_pChest = static_cast<CChest_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_Chest_AW"));
	if (nullptr == m_pChest)
		return E_FAIL;

	//m_tRimLightDesc.vRimColor = _float4(0.78125f, 0.78125f, 0.625f, 1.f);

	return S_OK;
}


void CEnemy_AW::PriorityTick(_float fTimeDelta)
{
	Hit_Scaling(fTimeDelta);
}

void CEnemy_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;

	m_pTransform->Add_Position(XMVectorSet(0.f, 0.f, -AWMGR->Get_MoveSpeed() * fTimeDelta, 0.f), false);

	if (m_fCurHp <= 0.f)
		return;

	m_pHpNumber->Update_Position(m_pTransform->Get_Position());
	m_pHpNumber->Update_RenderHp((_int)m_fCurHp);

	m_pModel->Play_Animation(fTimeDelta);

	m_pRigidbody->Update(fTimeDelta);
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CEnemy_AW::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), 1.f) && m_pCollider->Is_Active())
	{
		m_pModel->Update_BoneMatrices();
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_ShadowObject(this);

		m_pHpNumber->LateTick(fTimeDelta);

#ifdef _DEBUG
		m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
	}
}

HRESULT CEnemy_AW::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_int iTextureFlag = 0;

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::NORMALS);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_MRAO, "g_MRAOTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::MRAO);

		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_EMISSIVE, "g_EmissiveTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::EMISSIVE);

		if (FAILED(m_pShader->Bind_RawValue("g_iTextureFlag", &iTextureFlag, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, m_iPassIdx)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnemy_AW::OnEnter_Layer(void* pArg)
{
	__super::OnEnter_Layer(nullptr);

	m_bInLayer = true;

	m_pCollider->Set_Active(true);
	m_pRigidbody->Set_Velocity(XMVectorZero());
	m_pRigidbody->Set_AngularVelocity(0.f);

	m_pTransform->LookTo(XMVectorSet(0.f, 0.f, -1.f, 0.f));

	if (m_pChest)
		m_pChest->Set_ReturnToPool(true);

	return S_OK;
}

void CEnemy_AW::OnExit_Layer()
{
	__super::OnExit_Layer();

	if (XMVectorGetY(m_pRigidbody->Get_Velocity()) != 0.f)
		EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_Blood_Throwing", m_iEffect_Index);


	m_pCollider->Set_Active(false);
	if (m_pChest)
		m_pChest->Set_ReturnToPool(true);
}


void CEnemy_AW::HitScaling_On()
{
	m_bHitScaling = true;
	m_bScaleUp = true;
	m_fScalingTimeAcc = 0.f;
}

void CEnemy_AW::Hit_Scaling(_float fTimeDelta)
{
	if (false == m_bHitScaling)
		return;


	if (m_bScaleUp)
	{
		m_fScalingTimeAcc += fTimeDelta;
		if (m_fScalingTimeAcc >= m_fScalingTime * 0.5f)
		{
			m_fScalingTimeAcc = m_fScalingTime * 0.5f;
			m_bScaleUp = false;
		}
	}
	else
	{
		m_fScalingTimeAcc -= fTimeDelta;

		if (m_fScalingTimeAcc <= 0.f)
		{
			m_fScalingTimeAcc = 0.f;
			m_bHitScaling = false;

		}

	}
	_float fScalingRatio = m_fScalingTimeAcc / (m_fScalingTime * 0.5f);
	_float fHitScale = JoMath::Lerp(m_fOriginScale, m_fOriginScale * 1.2f, fScalingRatio);

	m_pTransform->Set_Scale(fHitScale);
}

void CEnemy_AW::Spawn_Chest()
{
	m_pChest->OnEnter_Layer(nullptr);
	ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"Layer_Chest", m_pChest));
	Safe_AddRef(m_pChest);
	m_pChest->Get_Transform()->Set_Position(m_pTransform->Get_Position());
	m_pChest->Get_Transform()->Set_PositionY(1.f);
}

void CEnemy_AW::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{
	if (TAG_PLAYER == pOther->Get_Tag() && m_fCurHp > 0.f)
	{
		CBike_AW* pPlayer = static_cast<CBike_AW*>(pOther);
		if (pPlayer->Is_SuperMode()) {

			_float fRandomX = JoRandom::Random_Int(0, 1) == 1 ? -1.f : 1.f;
			_vector vForce = XMVectorSet(-1.f, 1.f, 0.f, 0.f) * 20.f;
			m_pRigidbody->Add_Force(vForce, CRigidbody::Impulse);
			m_pRigidbody->Add_Torque(XMVector3Normalize(XMLoadFloat3(&JoRandom::Random_Float3({ -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f }))), 2000.f, CRigidbody::Impulse);

			EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Blood_Throwing", &m_Effect_Desc, &m_iEffect_Index);
			return;
		}

		ATTACKDESC AttackDesc{};
		AttackDesc.iDamage = (_int)m_fCurHp;

		pPlayer->Hit(AttackDesc);

		Hit(AttackDesc);
	}
}

void CEnemy_AW::Hit(const ATTACKDESC& AttackDesc)
{
	if (m_fCurHp <= 0.f)
		return;

	HitScaling_On();

	m_fCurHp -= (_float)AttackDesc.iDamage;

	if (m_fCurHp <= 0.f) {
		m_pCollider->Set_Active(false);
		Spawn_Chest();
	}

	_int iRand = JoRandom::Random_Int(0, 1);

	iRand == 0 ? EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Blood_Red0", &m_Effect_Desc)
		: EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Blood_Red1", &m_Effect_Desc);
}


HRESULT CEnemy_AW::Ready_Components(const wstring& wstrModelTag)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, wstrModelTag, TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDERDESC HitDesc;
	HitDesc.eType = CCollider::SPHERE;
	HitDesc.pOwner = this;
	HitDesc.vCenter = { 0.f, 1.f, 0.f };
	HitDesc.vSize = { 2.f, 0.f, 0.f };
	HitDesc.vRotation = { 0.f, 0.f, 0.f };
	HitDesc.bActive = false;
	HitDesc.strCollisionLayer = "Monster_HitBox";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("HitBox"), (CComponent**)&m_pCollider, &HitDesc)))
		return E_FAIL;

	CRigidbody::RIGIDBODY_DESC RigidDesc{};
	RigidDesc.fAngularFrictionScale = 0.f;
	RigidDesc.fFrictionScale = 0.f;
	RigidDesc.fMass = 1.f;
	RigidDesc.fMaxAngularVelocity = 3000.f;
	RigidDesc.vMaxVelocity = { 100.f, 100.f, 100.f };
	RigidDesc.pOwnerTransform = m_pTransform;
	RigidDesc.bMoveLocal = false;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Rigidbody"), TEXT("Rigidbody"), (CComponent**)&m_pRigidbody, &RigidDesc)))
		return E_FAIL;

	return S_OK;
}

CEnemy_AW* CEnemy_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_AW* pInstance = new CEnemy_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEnemy_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_AW::Clone(void* pArg)
{
	CEnemy_AW* pInstance = new CEnemy_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEnemy_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_AW::Free()
{
	__super::Free();

	Safe_Release(m_pHpNumber);
	Safe_Release(m_pChest);
	Safe_Release(m_pRigidbody);
}
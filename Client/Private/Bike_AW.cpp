#include "Bike_AW.h"

#include "Travis_AW.h"

#include "PlayerBullet_AW.h"
#include "Hp_Number.h"

#include "Bone.h"

#include "Jeane_AW.h"

#include "AW_Manager.h"
#include "Effect_Manager.h"

#include "Cinematic_Manager.h"

CBike_AW::CBike_AW(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CBike_AW::CBike_AW(const CBike_AW& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CBike_AW::Initialize_Prototype()
{
	m_iTag = TAG_PLAYER;

	return S_OK;
}

HRESULT CBike_AW::Initialize(void* _pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Bullets()))
		return E_FAIL;

	if (FAILED(Ready_TireBones()))
		return E_FAIL;


	m_pTravis = CTravis_AW::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTravis)
		return E_FAIL;

	m_pHpNumber = static_cast<CHP_Number*>(m_pGameInstance->Clone_GameObject(L"Prototype_HP_Number"));
	if (nullptr == m_pHpNumber)
		return E_FAIL;

	m_pHpNumber->Set_Offset(XMVectorSet(-0.1f, 3.f, 0.f, 0.f));

	Bind_KeyFrames();
	Bind_KeyFrameEffects();
	Bind_KeyFrameSounds();

	m_pTransform->Set_Scale(0.7f);
	m_pTransform->Set_Speed(2.f);

	m_strCinemaActorTag = "Bike_AW";
	CINEMAMGR->Add_CinemaObject(this);

	return S_OK;
}

void CBike_AW::PriorityTick(_float _fTimeDelta)
{
	
}

void CBike_AW::Tick(_float _fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;
	
	if (m_bCinematic)
	{
		Play_Cinematic(CINEMAMGR->Get_PlayTimeAcc());
	}
	else
	{
		Key_Input(_fTimeDelta);

		Check_Fire(_fTimeDelta);

		m_pHpNumber->Update_Position(m_pTransform->Get_Position());
		m_pHpNumber->Update_RenderHp((_int)m_fCurHp);
	}
	
	m_pModel->Play_Animation(_fTimeDelta);

	if (m_pTravis)
		m_pTravis->Tick(_fTimeDelta);

	Rotate_Tires(_fTimeDelta);
		
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CBike_AW::LateTick(_float _fTimeDelta)
{
	if (false == m_bCinematic)
		m_pHpNumber->LateTick(_fTimeDelta);

	m_pModel->Update_BoneMatrices();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_ShadowObject(this);

	if (m_pTravis)
	{
		m_pTravis->LateTick(_fTimeDelta);
		m_pTravis->Get_Transform()->Attach_To_Bone(m_pModel->Get_Bone("pirot_attach"), m_pTransform, XMMatrixRotationY(XM_PI), false, true);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif

}

HRESULT CBike_AW::Render()
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


void CBike_AW::Bind_KeyFrames()
{
}

void CBike_AW::Bind_KeyFrameEffects()
{
}

void CBike_AW::Bind_KeyFrameSounds()
{
}

void CBike_AW::Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs)
{
	__super::Start_Cinematic(CinematicDescs);

	m_bCinematic = true;
	
}

void CBike_AW::End_Cinematic()
{
	m_bCinematic = false;

}

void CBike_AW::OnCollisionEnter(CGameObject* _pOther, CCollider* pCollider)
{
	
}

void CBike_AW::OnCollisionExit(CGameObject* _pOther, CCollider* pCollider)
{

}

void CBike_AW::Hit(const ATTACKDESC& AttackDesc)
{
	m_fCurHp -= (_int)AttackDesc.iDamage;
	if (m_fCurHp <= 0.f)
	{
		m_fCurHp = 0.f;
	}
}


void CBike_AW::Adjust_Ability(ABILITY_TYPE eAbility, _float fValue)
{
	switch (eAbility)
	{
	case BULLETATTACK:
		CPlayerBullet_AW::Add_Attack(fValue);
		break;
	case SHOTSPEED:
		CPlayerBullet_AW::Add_ShotSpeed(fValue);
		break;
	case SHOT_INTERVAL:
		m_fFireInterval = max(0.2f, m_fFireInterval - fValue);
		break;  
	case HORIZONTAL_SPEED_UP:
		m_pTransform->Set_Speed(m_pTransform->Get_Speed() + fValue);
		break;
	case VERTICAL_SPEED_UP:
		AWMGR->Add_MoveSpeed(0.5f);
		break;
	case HEALING:
		m_fCurHp += m_fMaxHp * m_fHealingRatio / 100.f;
		m_fCurHp = m_fCurHp > m_fMaxHp ? m_fMaxHp : m_fCurHp;
		EFFECTMGR->Active_Effect(LEVEL_STATIC, "Travis_Healing", Get_EffectDescPtr());
		break;
	case ADD_HP:
		Add_MaxHp(100.f);
		break;
	}

	
}

void CBike_AW::Active_SuperMode()
{
	m_bSuperMode = true;
	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Travis_SuperMode", dynamic_cast<CCharacter*>(m_pTravis)->Get_EffectDescPtr());
	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Bike_SuprMode", &m_Effect_Desc);
	// EFFECT
}

void CBike_AW::Inactive_SuperMode()
{
	m_bSuperMode = false;
	EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_Travis_SuperMode");
	EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_Bike_SuprMode");
}


void CBike_AW::Key_Input(_float fTimeDelta)
{
	if (KEY_PUSHING(eKeyCode::A))
		m_pTransform->Go_Left(fTimeDelta);

	if (KEY_PUSHING(eKeyCode::D))
		m_pTransform->Go_Right(fTimeDelta);
}

void CBike_AW::Rotate_Tires(_float fTimeDelta)
{
	m_fTireRotation += 720.f * fTimeDelta;
	if (m_fTireRotation >= 360.f)
		m_fTireRotation = 0.f;

	m_pBone_Tire_R->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation)));
	m_pBone_Tire_L->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation)));
	m_pBone_Tire_F->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation)));
	
}

void CBike_AW::Check_Fire(_float fTimeDelta)
{
	m_fFireAcc += fTimeDelta;
	if (m_fFireAcc >= m_fFireInterval)
	{
		ADD_EVENT(bind(&CBike_AW::Spawn_Bullet, this));
		m_fFireAcc = 0.f;

		if (m_pGameInstance->Is_Playing("8bit_Gun_Shot"))
			return;
		else
			PLAY_SOUND("8bit_Gun_Shot", false, 0.3f);
	}

}

void CBike_AW::Spawn_Bullet()
{
	size_t iBulletStrength = CPlayerBullet_AW::Get_BulletStrength();

	size_t iBulletCnt = iBulletStrength * 2 - 1;

	_float fSpawnGap = 0.3f;
	_float fSpawnStartX = iBulletCnt % 2 == 0 ? -fSpawnGap * 0.5f - fSpawnGap * (_float)(iBulletCnt / 2 - 1) : -fSpawnGap * (_float)(iBulletCnt / 2);

	_float fSpawnX = fSpawnStartX;
	for (size_t i = 0; i < iBulletCnt; ++i)
	{
		m_Bullets[m_iBulletIdx]->OnEnter_Layer(nullptr);
		CALC_TF->Set_WorldMatrix(m_pTransform->Get_WorldMatrix());

		CALC_TF->Add_Position(XMVectorSet(fSpawnX, 1.f, 1.f, 0.f), true);

		m_Bullets[m_iBulletIdx]->Get_Transform()->Set_Position(CALC_TF->Get_Position());

		ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"Layer_Bullet", m_Bullets[m_iBulletIdx]));
		Safe_AddRef(m_Bullets[m_iBulletIdx]);

		m_iBulletIdx = (m_iBulletIdx + 1) % m_Bullets.size();

		fSpawnX += fSpawnGap;
	}

	
}


HRESULT CBike_AW::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Travis_Bike"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType = CCollider::AABB;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 1.f, 0.f };
	ColliderDesc.vSize = { 1.f, 2.f, 5.f };
	ColliderDesc.bActive = true;
	ColliderDesc.strCollisionLayer = "Travis_HitBox";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_AABB"), TEXT("Collider"), (CComponent**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	//ColliderDesc.eType = CCollider::SPHERE;
	//ColliderDesc.pOwner = this;
	//ColliderDesc.vCenter = { 1.f, 1.f, 0.f };
	//ColliderDesc.vSize = { 1.f, 0.f, 0.f };
	//ColliderDesc.bActive = true;
	//ColliderDesc.strCollisionLayer = "Bike_AW";
	//ColliderDesc.strColliderTag = "Riding_Bike_Collider";
	//
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Riding_Trigger"), (CComponent**)&m_, &ColliderDesc)))
	//	return E_FAIL;

	m_pModel->Set_AnimPlay();

	return S_OK;
}

HRESULT CBike_AW::Ready_Bullets()
{
	m_Bullets.resize(30);

	for (size_t i = 0; i < m_Bullets.size(); ++i)
		m_Bullets[i] = CPlayerBullet_AW::Create(m_pDevice, m_pContext);


	return S_OK;
}

HRESULT CBike_AW::Ready_TireBones()
{
	m_pBone_Tire_R = m_pModel->Get_Bone("rtire_r");
	if (nullptr == m_pBone_Tire_R)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_R);

	m_pBone_Tire_L = m_pModel->Get_Bone("rtire_l");
	if (nullptr == m_pBone_Tire_L)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_L);

	m_pBone_Tire_F = m_pModel->Get_Bone("fronttire");
	if (nullptr == m_pBone_Tire_F)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_F);

	return S_OK;
}


CBike_AW* CBike_AW::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBike_AW* pInstance = new CBike_AW(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CBike_AW");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBike_AW::Clone(void* _pArg)
{
	CBike_AW* pInstance = new CBike_AW(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBike_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBike_AW::Free()
{
	__super::Free();

	Safe_Release(m_pTravis);

	for (auto& pBullet : m_Bullets)
		Safe_Release(pBullet);

	m_Bullets.clear();

	Safe_Release(m_pBone_Tire_F);
	Safe_Release(m_pBone_Tire_L);
	Safe_Release(m_pBone_Tire_R);

	Safe_Release(m_pHpNumber);
}

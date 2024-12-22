#include "Enemy_Bullet_AW.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"

#include "GameEffect.h"

#include "Character.h"


CEnemy_Bullet_AW::CEnemy_Bullet_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemy_Bullet_AW::CEnemy_Bullet_AW(const CEnemy_Bullet_AW& rhs)
	: CGameObject(rhs)
{
}


HRESULT CEnemy_Bullet_AW::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_BulletDesc = *(ENEMY_BULLET_DESC*)pArg;

	m_iTag = (_uint)TAG_PLAYER_WEAPON;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(m_BulletDesc.fBulletSpeed);

	m_Effect_Desc.pParentTransform = m_pTransform;

	return S_OK;
}


void CEnemy_Bullet_AW::PriorityTick(_float fTimeDelta)
{
}

void CEnemy_Bullet_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;

	m_pTransform->Go_Straight(fTimeDelta);

	if (m_pTransform->Get_Position().m128_f32[2] < -2.f)
		m_bReturnToPool = true;

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CEnemy_Bullet_AW::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
}

HRESULT CEnemy_Bullet_AW::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;
	m_bInLayer = true;

	ADD_EVENT(bind(&CCollider::Set_Active, m_pCollider, true));

	m_fMoveDistance = 0.f;

	CGameEffect::EFFECT_DESC Desc{};
	Desc.pParentTransform = m_pTransform;

	EFFECTMGR->Active_Effect(GET_CURLEVEL, m_BulletDesc.strBulletEffectTag, &Desc, &m_iEffectIdx);

	return S_OK;
}

void CEnemy_Bullet_AW::OnExit_Layer()
{
	__super::OnExit_Layer();

	m_pCollider->Set_Active(false);

	if (m_iEffectIdx != -1)
	{
		EFFECTMGR->End_Effect(GET_CURLEVEL,  m_BulletDesc.strBulletEffectTag, (size_t)m_iEffectIdx);
	}
}


void CEnemy_Bullet_AW::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{
	m_bReturnToPool = true;

	ATTACKDESC AttackDesc{};
	AttackDesc.iDamage = (_int)m_BulletDesc.fBulletAttack;

	static_cast<CCharacter*>(pOther)->Hit(AttackDesc);

	EFFECTMGR->End_Effect(GET_CURLEVEL, m_BulletDesc.strBulletEffectTag, (size_t)m_iEffectIdx);
	m_iEffectIdx = -1;
}


HRESULT CEnemy_Bullet_AW::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.f, 0.f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "Monster_Weapon";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CEnemy_Bullet_AW* CEnemy_Bullet_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy_Bullet_AW* pInstance = new CEnemy_Bullet_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEnemy_Bullet_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnemy_Bullet_AW::Clone(void* pArg)
{
	CEnemy_Bullet_AW* pInstance = new CEnemy_Bullet_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEnemy_Bullet_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Bullet_AW::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
}

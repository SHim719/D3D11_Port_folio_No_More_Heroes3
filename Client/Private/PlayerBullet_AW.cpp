#include "PlayerBullet_AW.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"

#include "CustomFont.h"

#include "Camera.h"

#include "Enemy_AW.h"

 _float	CPlayerBullet_AW::s_fMaxDist = { 20.f };
 _float	CPlayerBullet_AW::s_fBulletAttack = { 1.f };
 _float	CPlayerBullet_AW::s_fBulletSpeed = { 8.f };
 size_t CPlayerBullet_AW::s_iStrength = { 1 };

 vector<string> CPlayerBullet_AW::s_strEffectTag {
	"AA_Bullet_Sapphire",
	"AA_Bullet_Purple",
	"AA_Bullet_Red",
 };


CPlayerBullet_AW::CPlayerBullet_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerBullet_AW::CPlayerBullet_AW(const CPlayerBullet_AW& rhs)
	: CGameObject(rhs)
{
}


HRESULT CPlayerBullet_AW::Initialize(void* pArg)
{
	m_iTag = (_uint)TAG_PLAYER_WEAPON;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(5.f);

	return S_OK;
}


void CPlayerBullet_AW::PriorityTick(_float fTimeDelta)
{
}

void CPlayerBullet_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;

	m_pTransform->Add_Position(XMVectorSet(0.f, 0.f, s_fBulletSpeed * fTimeDelta, 0.f), false);

	m_fMoveDistance += s_fBulletSpeed * fTimeDelta;
	if (m_fMoveDistance >= s_fMaxDist)
		m_bReturnToPool = true;

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CPlayerBullet_AW::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	//	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
}

HRESULT CPlayerBullet_AW::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;
	m_bInLayer = true;

	ADD_EVENT(bind(&CCollider::Set_Active, m_pCollider, true));

	m_fMoveDistance = 0.f;

	CGameEffect::EFFECT_DESC Desc{};
	Desc.pParentTransform = m_pTransform;


	m_strEffectTag = s_strEffectTag[s_iStrength - 1];

	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, m_strEffectTag, &Desc, &m_iEffectIdx);


	return S_OK;
}

void CPlayerBullet_AW::OnExit_Layer()
{
	__super::OnExit_Layer();

	m_pCollider->Set_Active(false);

	if (m_iEffectIdx != -1)
		EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, m_strEffectTag, (size_t)m_iEffectIdx);
	
}


void CPlayerBullet_AW::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{
	m_bReturnToPool = true;

	ATTACKDESC AttackDesc{};
	AttackDesc.iDamage = (_int)s_fBulletAttack;

	static_cast<CEnemy_AW*>(pOther)->Hit(AttackDesc);

	EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, m_strEffectTag, (size_t)m_iEffectIdx);
	m_iEffectIdx = -1;

	if (m_pGameInstance->Is_Playing("Bullet_hit"))
		return;
	else
		PLAY_SOUND("Bullet_hit", false, 0.3f);
}


HRESULT CPlayerBullet_AW::Ready_Components()
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
	ColliderDesc.strCollisionLayer = "Travis_Weapon";
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CPlayerBullet_AW* CPlayerBullet_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerBullet_AW* pInstance = new CPlayerBullet_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerBullet_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerBullet_AW::Clone(void* pArg)
{
	CPlayerBullet_AW* pInstance = new CPlayerBullet_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayerBullet_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerBullet_AW::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
}

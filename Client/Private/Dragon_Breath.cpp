#include "Dragon_Breath.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"

#include "CustomFont.h"

#include "Camera.h"

#include "Enemy_AW.h"

_float	CDragon_Breath::s_fMaxDist = { 20.f };
_float	CDragon_Breath::s_fBreathAttack = { 20.f };
_float	CDragon_Breath::s_fBreathSpeed = { 10.f };
_uint	CDragon_Breath::s_iBreathStrength = { 0 };


CDragon_Breath::CDragon_Breath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDragon_Breath::CDragon_Breath(const CDragon_Breath& rhs)
	: CGameObject(rhs)
{
}


HRESULT CDragon_Breath::Initialize(void* pArg)
{
	m_iTag = (_uint)TAG_PLAYER_WEAPON;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_Speed(s_fBreathSpeed);

	m_Effect_Desc.pParentTransform = m_pTransform;

	return S_OK;
}


void CDragon_Breath::PriorityTick(_float fTimeDelta)
{
}

void CDragon_Breath::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;

	m_pTransform->Go_Straight(fTimeDelta);

	m_fMoveDistance += m_pTransform->Get_Speed() * fTimeDelta;
	if (m_fMoveDistance >= s_fMaxDist)
		m_bReturnToPool = true;

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CDragon_Breath::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
}

HRESULT CDragon_Breath::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;
	m_bInLayer = true;

	m_pCollider->Set_Active(true);

	m_fMoveDistance = 0.f;

	CGameEffect::EFFECT_DESC Desc{};
	Desc.pParentTransform = m_pTransform;

	_vector vLook = m_pTransform->Get_GroundLook();
	vLook.m128_f32[1] -= 0.1f;
	vLook = XMVector3Normalize(vLook);

	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);

	string strEffectTag = "AA_Dragon_FireBall_Level" + to_string(s_iBreathStrength + 1);

	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, strEffectTag, &Desc, &m_iEffectIdx);

	m_strEffectTag = strEffectTag;

	return S_OK;
}

void CDragon_Breath::OnExit_Layer()
{
	__super::OnExit_Layer();

	m_pCollider->Set_Active(false);

	if (m_iEffectIdx != -1)
		End_Effect();
	
}


void CDragon_Breath::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{
	PLAY_SOUND("Fireball_Hit", false, 1.f);

	m_bReturnToPool = true;

	ATTACKDESC AttackDesc{};
	AttackDesc.iDamage = (_int)s_fBreathAttack;

	static_cast<CEnemy_AW*>(pOther)->Hit(AttackDesc);

	End_Effect();
}

void CDragon_Breath::End_Effect()
{
	EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, m_strEffectTag, m_iEffectIdx);
	m_iEffectIdx = -1;
}


HRESULT CDragon_Breath::Ready_Components()
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

CDragon_Breath* CDragon_Breath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDragon_Breath* pInstance = new CDragon_Breath(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CDragon_Breath"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDragon_Breath::Clone(void* pArg)
{
	CDragon_Breath* pInstance = new CDragon_Breath(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDragon_Breath"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragon_Breath::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
}

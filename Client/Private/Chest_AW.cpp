#include "Chest_AW.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"
#include "CustomFont.h"

#include "Chest_Up.h"
#include "Chest_Down.h"
#include "Chest_Ability_Font.h"

#include "Bike_AW.h"

CChest_AW::CChest_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CChest_AW::CChest_AW(const CChest_AW& rhs)
	: CGameObject(rhs)
{
}


HRESULT CChest_AW::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CChest_AW::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pChestUp = CChest_Up::Create(m_pDevice, m_pContext);
	m_pChestDown = CChest_Down::Create(m_pDevice, m_pContext);
	m_pChestFont = CChest_Ability_Font::Create(m_pDevice, m_pContext);

	m_Effect_Desc.pParentTransform = m_pTransform;
	return S_OK;
}


void CChest_AW::PriorityTick(_float fTimeDelta)
{
}

void CChest_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;

	m_pTransform->Add_Position(XMVectorSet(0.f, 0.f, -AWMGR->Get_MoveSpeed() * fTimeDelta, 0.f), false);

	Update_ChestParts();

	m_pChestUp->Tick(fTimeDelta);

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CChest_AW::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), 1.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pChestFont->LateTick(fTimeDelta);

#ifdef _DEBUG
		m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
	}
}

HRESULT CChest_AW::Render()
{
	m_pChestUp->Render();
	m_pChestDown->Render();

	return S_OK;
}

HRESULT CChest_AW::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;
	m_bInLayer = true;

	ADD_EVENT(bind(&CCollider::Set_Active, m_pCollider, true));

	Set_RandomReward();

	m_pChestUp->Init_Spawn();

	CGameEffect::EFFECT_DESC Desc{};
	Desc.pParentTransform = m_pTransform;

	return S_OK;
}

void CChest_AW::OnExit_Layer()
{
	__super::OnExit_Layer();

	m_pCollider->Set_Active(false);

}


void CChest_AW::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{
	if (TAG_PLAYER == pOther->Get_Tag())
	{
		m_pCollider->Set_Active(false);
		
		AWMGR->Adjust_Chest_Reward(m_eRewardType);

		m_pChestUp->Set_State_Opening();

		EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Travis_GetChest", dynamic_cast<CCharacter*>(pOther)->Get_EffectDescPtr());
	}

}

void CChest_AW::Set_RandomReward()
{
	_uint iDragonCount = AWMGR->Get_Player()->Get_DragonCount();
	_uint iJeanCount = AWMGR->Get_Player()->Get_JeanCount();

	if (iDragonCount == 0 && iJeanCount == 0)
	{
		m_eRewardType = (CHEST_REWARD_TYPE)JoRandom::Random_Int(ADD_DOG, ADD_DRAGON);
	}
	else if (iDragonCount == 0)
	{
		m_eRewardType = ADD_DRAGON;
	}
	else if (iJeanCount == 0)
	{
		m_eRewardType = ADD_DOG;
	}
	else if (iJeanCount >= 4 && iDragonCount < 4)
	{
		m_eRewardType = ADD_DRAGON;
		if (JoRandom::Random_Int(0, 1))
			m_eRewardType = (CHEST_REWARD_TYPE)JoRandom::Random_Int(ADD_DOG_ATTACK, REDUCE_DRAGON_ATTACK_INTERVAL);
	}
	else if (iJeanCount < 4 && iDragonCount >= 4)
	{
		m_eRewardType = ADD_DOG;
		if (JoRandom::Random_Int(0, 1))
			(CHEST_REWARD_TYPE)JoRandom::Random_Int(ADD_DOG_ATTACK, REDUCE_DRAGON_ATTACK_INTERVAL);
	}
	else if (iJeanCount >= 4 && iDragonCount >= 4)
	{
		m_eRewardType = (CHEST_REWARD_TYPE)JoRandom::Random_Int(ADD_DOG_ATTACK, REDUCE_DRAGON_ATTACK_INTERVAL);
	}
	else
	{
		m_eRewardType = (CHEST_REWARD_TYPE)JoRandom::Random_Int(ADD_DOG, REDUCE_DRAGON_ATTACK_INTERVAL);
	}

	wstring wstrFont = L"";

	switch (m_eRewardType)
	{
	case ADD_DOG:
		wstrFont = L"진 소환";
		break;
	case ADD_DRAGON:
		wstrFont = L"용 소환";
		break;
	case ADD_DOG_ATTACK:
		wstrFont = L"진 공격력\n증가";
		break;
	case ADD_DRAGON_ATTACK:
		wstrFont = L"용 공격력\n증가";
		break;
	case REDUCE_DOG_ATTACK_INTERVAL:
		wstrFont = L"진 공격빈도\n증가";
		break;
	case REDUCE_DRAGON_ATTACK_INTERVAL:
		wstrFont = L"용 연사\n증가";
		break;
	}

	m_pChestFont->Set_RenderText(wstrFont);
}

void CChest_AW::Update_ChestParts()
{
	_vector vChestPos = m_pTransform->Get_Position();

	m_pChestUp->Get_Transform()->Set_Position(vChestPos);
	m_pChestDown->Get_Transform()->Set_Position(vChestPos);

	_vector vFontPos = vChestPos + XMVectorSet(0.f, 1.f, -1.f, 0.f);

	m_pChestFont->Get_Transform()->Set_Position(vFontPos);

}


HRESULT CChest_AW::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 2.f, 0.f, 0.f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "NPC_HitBox";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CChest_AW* CChest_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChest_AW* pInstance = new CChest_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChest_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChest_AW::Clone(void* pArg)
{
	CChest_AW* pInstance = new CChest_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChest_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest_AW::Free()
{
	__super::Free();

	Safe_Release(m_pChestUp);
	Safe_Release(m_pChestDown);
	Safe_Release(m_pChestFont);

	Safe_Release(m_pCollider);
}

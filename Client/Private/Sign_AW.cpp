#include "Sign_AW.h"

#include "Effect_Manager.h"
#include "AW_Manager.h"
#include "CustomFont.h"
#include "Camera.h"
#include "Sign_AW.h"

#include "Bike_AW.h"

CSign_AW::CSign_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSign_AW::CSign_AW(const CSign_AW& rhs)
	: CGameObject(rhs)
{
}


HRESULT CSign_AW::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;

	return S_OK;
}

HRESULT CSign_AW::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransform->Set_PositionY(2.f);
	m_pTransform->Set_Scale({ 4.f, 4.f, 1.f });

	return S_OK;
}


void CSign_AW::PriorityTick(_float fTimeDelta)
{
}

void CSign_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;

	m_pTransform->Add_Position(XMVectorSet(0.f, 0.f, -AWMGR->Get_MoveSpeed() * fTimeDelta, 0.f), false);

	m_pRigidbody->Update(fTimeDelta);
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CSign_AW::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), 1.f) && (m_pCollider->Is_Active() || true == m_bFlying))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_AFTER_POSTPROCESS, this);


#ifdef _DEBUG
		m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
	}
}

HRESULT CSign_AW::Render()
{
	m_pGameInstance->Render_TextToTarget(L"Font_AW", m_wstrText, { 200.f, 200.f }, XMVectorSet(0.f, 0.f, 0.7f, 1.f), CCustomFont::FontAlignCenter);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Text", m_pShader, "g_FontTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CSign_AW::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;
	m_bInLayer = true;
	m_bFlying = false;

	m_pRigidbody->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pRigidbody->Set_AngularVelocity(0.f);

	m_pCollider->Set_Active(true);

	Select_RandAbility();

	CGameEffect::EFFECT_DESC Desc{};
	Desc.pParentTransform = m_pTransform;

	EFFECTMGR->Active_Effect(GET_CURLEVEL, "Effect_Pannel", &Desc, &m_iIdx);
	return S_OK;
}

void CSign_AW::OnExit_Layer()
{                                                                                                                
	__super::OnExit_Layer();

	m_pCollider->Set_Active(false);

	if (m_iIdx != -1)
	{
		EFFECTMGR->End_Effect(GET_CURLEVEL, "Effect_Pannel", (size_t)m_iIdx);
		m_iIdx = -1;
	}
		
	
}


void CSign_AW::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{
	if (TAG_PLAYER == pOther->Get_Tag())
	{
		CBike_AW* pPlayer = static_cast<CBike_AW*>(pOther);

		m_pCollider->Set_Active(false);

		pPlayer->Adjust_Ability(m_eAbilityType, m_fAbilityVal);
		AWMGR->Active_Ability_Text(m_wstrText);


		if (pPlayer->Is_SuperMode()) {

			m_bFlying = true;
			_float fRandomX = JoRandom::Random_Int(0, 1) == 1 ? -1.f : 1.f;
			_vector vForce = XMVectorSet(-1.f, 1.f, 0.f, 0.f) * 20.f;
			m_pRigidbody->Add_Force(vForce, CRigidbody::Impulse);
			m_pRigidbody->Add_Torque(XMVector3Normalize(XMLoadFloat3(&JoRandom::Random_Float3({ -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f }))), 2000.f, CRigidbody::Impulse);
		}
		else
		{
			EFFECTMGR->End_Effect(GET_CURLEVEL, "Effect_Pannel", (size_t)m_iIdx);
			m_iIdx = -1;
		}

		EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Travis_GetPannel", dynamic_cast<CCharacter*>(pOther)->Get_EffectDescPtr());
	}

}

void CSign_AW::Select_RandAbility()
{
	m_eAbilityType = (ABILITY_TYPE)JoRandom::Random_Int(0, ABILITY_TYPE_END - 1);
	
	switch (m_eAbilityType)
	{
	case BULLETATTACK:
	{
		m_fAbilityVal = 10.f;
		m_wstrText = L"총알 공격력\n증가";
		break;
	}
	case SHOTSPEED:
	{
		m_fAbilityVal = 1.f;
		m_wstrText = L"총알 속도\n증가";
		break;
	}
		
	case SHOT_INTERVAL:
	{
		m_fAbilityVal = 0.1f;
		m_wstrText = L"총알 딜레이\n감소";
		break;
	}

	case HORIZONTAL_SPEED_UP:
	{
		m_fAbilityVal = 1.f;
		m_wstrText = L"좌우 속도\n증가";
		break;
	}
	case VERTICAL_SPEED_UP:
	{
		m_fAbilityVal = 1.f;
		m_wstrText = L"세로 속도\n증가";
		break;
	}

	case HEALING:
	{
		m_fAbilityVal = 1.f;
		m_wstrText = L"체력 회복";
		break;
	}

	case ADD_HP:
	{
		m_fAbilityVal = 50.f;
		m_wstrText = L"체력 증가";
		break;
	}
	}

}


HRESULT CSign_AW::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxQuad"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 0.f, 0.f };
	ColliderDesc.vSize = { 0.8f, 0.5f, 0.5f };
	ColliderDesc.vRotation = { 0.f, 0.f, 0.f };
	ColliderDesc.bActive = false;
	ColliderDesc.strCollisionLayer = "NPC_HitBox";
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_AABB"), TEXT("Collider"), (CComponent**)&m_pCollider, &ColliderDesc)))
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

CSign_AW* CSign_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSign_AW* pInstance = new CSign_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSign_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSign_AW::Clone(void* pArg)
{
	CSign_AW* pInstance = new CSign_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSign_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSign_AW::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRigidbody);
}

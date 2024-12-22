#include "Jeane_AW.h"

#include "AW_Manager.h"

#include "Jeane_Enums.h"

#include "Enemy_AW.h"
#include "Bike_AW.h"
#include "Effect_Manager.h"

_float		CJeane_AW::s_fAttackInterval = 0.5f;
_float		CJeane_AW::s_fAttack = 5.f;
_int		CJeane_AW::s_iSpawnSequence = 0;


CJeane_AW::CJeane_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CJeane_AW::CJeane_AW(const CJeane_AW& rhs)
	: CCharacter(rhs)
{
}


HRESULT CJeane_AW::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJeane_AW::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* Effect */
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	_float fRandomX = JoRandom::Random_Float(-2.5f, -1.f);
	fRandomX = s_iSpawnSequence == 1 ? fRandomX : JoRandom::Random_Float(1.f, 2.5f);

	s_iSpawnSequence = (s_iSpawnSequence + 1) % 2;

	_float fRandomZ = JoRandom::Random_Float(1.f, 3.f);

	m_vOffset = { fRandomX, 0.f, fRandomZ, 0.f };

	m_pTransform->Set_Scale(2.f);

	Change_State(JEANE_MOVE);

	m_pModel->Set_AnimPlay();

	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Jean_Spawn", &m_Effect_Desc);
	PLAY_SOUND("Jeane_Meow", false, 1.f);

	return S_OK;
}


void CJeane_AW::PriorityTick(_float fTimeDelta)
{

}

void CJeane_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Cinematic())
	{
		m_bDestroyed = true;
		return;
	}

	if (AWMGR->Is_Stop())
		return;

	if (Check_Release_Target(m_pAttackTarget))
	{
		Safe_Release(m_pAttackTarget);
		Change_State(JEANE_MOVE);
	}

	if (false == m_bEnd_SpawnEffect)
	{
		m_fSpawnEffectTimeAcc += fTimeDelta;
	}

	if (m_fSpawnEffectTimeAcc >= 0.5f)
	{
		m_bEnd_SpawnEffect = true;
	}
	switch (m_eState)
	{
	case JEANE_MOVE:
		Move(fTimeDelta);
		break;
	case JEANE_RUSH:
		if (true == m_bEnd_SpawnEffect) {
			Rush(fTimeDelta);
		}
		break;
	case JEANE_ATTACK:
		if (true == m_bEnd_SpawnEffect) {
			Attack(fTimeDelta);
		}
		break;
	}

	m_pModel->Play_Animation(fTimeDelta);
}

void CJeane_AW::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), 1.f))
	{
		m_pModel->Update_BoneMatrices();
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_ShadowObject(this);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif

}

HRESULT CJeane_AW::Render()
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

HRESULT CJeane_AW::OnEnter_Layer(void* pArg)
{
	__super::OnEnter_Layer(nullptr);

	m_bInLayer = true;

	m_vOffset = JoRandom::Random_Float4({ -1.5f, 0.f, 1.f, 0.f }, { 1.5f, 0.f, 1.5f, 0.f });


	return S_OK;
}

void CJeane_AW::OnExit_Layer()
{
	__super::OnExit_Layer();

}


void CJeane_AW::Move(_float fTimeDelta)
{
	_vector vPlayerPos = AWMGR->Get_Player()->Get_Transform()->Get_Position();

	m_pTransform->Set_Position(vPlayerPos + XMLoadFloat4(&m_vOffset));

	_float fTargetDist = 0.f;
	CEnemy_AW* pEnemy = AWMGR->Find_NearestEnemy(fTargetDist);
	if (true == Check_Release_Target(pEnemy))
		return;

	if (fTargetDist < m_fRushDist)
	{
		Safe_Release(m_pAttackTarget);
		m_pAttackTarget = pEnemy;
		Safe_AddRef(m_pAttackTarget);
		Change_State(JEANE_RUSH);
		if (m_pGameInstance->Is_Playing("Jeane_Aggressive"))
			PLAY_SOUND("Jeane_Aggressive", false, 0.1f);
		else
			PLAY_SOUND("Jeane_Aggressive", false, 0.6f);
	}


}

void CJeane_AW::Rush(_float fTimeDelta)
{
	m_pTransform->LookAt(m_pAttackTarget->Get_Transform()->Get_Position());

	m_pTransform->Move_Root(m_pModel->Get_DeltaRootPos() * 6.5f);

	_float fDist = XMVector3Length(m_pTransform->Get_Position() - m_pAttackTarget->Get_Transform()->Get_Position()).m128_f32[0];
	if (fDist < m_fRushMargin)
	{
		Change_State(JEANE_ATTACK);
		if (m_pGameInstance->Is_Playing("Jeane_DoubleScratch"))
			PLAY_SOUND("Jeane_DoubleScratch", false, 0.1f);
		else 
			PLAY_SOUND("Jeane_DoubleScratch", false, 1.f);
	}


}

void CJeane_AW::Attack(_float fTimeDelta)
{
	m_pTransform->LookAt(m_pAttackTarget->Get_Transform()->Get_Position());

	if (!AWMGR->Is_Boss_Stage())
		m_pTransform->Add_Position(XMVectorSet(0.f, 0.f, -AWMGR->Get_MoveSpeed() * fTimeDelta, 0.f), false);

	m_fIntervalAcc += fTimeDelta;
	if (m_fIntervalAcc >= s_fAttackInterval)
	{
		ATTACKDESC AttackDesc{};
		AttackDesc.iDamage = (_int)s_fAttack;

		m_pAttackTarget->Hit(AttackDesc);

		m_fIntervalAcc = 0.f;

		EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Jean_Attack", &m_Effect_Desc);
	}
}


void CJeane_AW::Change_State(JEANE_STATE eState)
{
	switch (eState)
	{
	case JEANE_MOVE:
		m_pModel->Change_Animation((_uint)JEANE_ANIMATIONS::ANI_CH_Jeane_MOV_Run);
		m_pTransform->LookTo(ZAXIS);
		break;
	case JEANE_RUSH:
	case JEANE_ATTACK:
		m_pModel->Change_Animation((_uint)JEANE_ANIMATIONS::ANI_CH_Jeane_MOV_Avoid_F);
		m_fIntervalAcc = s_fAttackInterval;
		break;
	}

	m_eState = eState;

}

_bool CJeane_AW::Check_Release_Target(CEnemy_AW* pEnemy)
{
	return pEnemy && (pEnemy->Get_Hp() <= 0.f || pEnemy->Get_Transform()->Get_Position().m128_f32[2] < 1.f
		|| pEnemy->Get_Transform()->Get_Position().m128_f32[1] != 0.f);
}

void CJeane_AW::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{

}

HRESULT CJeane_AW::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Jeane"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CJeane_AW* CJeane_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJeane_AW* pInstance = new CJeane_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CJeane_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJeane_AW::Clone(void* pArg)
{
	CJeane_AW* pInstance = new CJeane_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CJeane_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJeane_AW::Free()
{
	__super::Free();

	Safe_Release(m_pAttackTarget);
}
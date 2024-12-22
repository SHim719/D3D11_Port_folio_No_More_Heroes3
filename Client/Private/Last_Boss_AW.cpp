#include "Last_Boss_AW.h"

#include "AW_Manager.h"

#include "HP_Number.h"

#include "Bike_AW.h"
#include "Enemy_Bullet_AW.h"

#include "Bone.h"
#include "Effect_Manager.h"
#include "Various_Camera.h"
#include "Travis.h"
#include "Cinematic_Manager.h"


CLast_Boss_AW::CLast_Boss_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy_AW(pDevice, pContext)
{
}

CLast_Boss_AW::CLast_Boss_AW(const CLast_Boss_AW& rhs)
	: CEnemy_AW(rhs)
{
}

HRESULT CLast_Boss_AW::Initialize_Prototype()
{
	m_iTag = (_uint)TAG_ENEMY;

	return S_OK;
}

HRESULT CLast_Boss_AW::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	if (FAILED(Ready_Bullets()))
		return E_FAIL;

	m_pModel->Set_AnimPlay();

	m_eCurState = LAST_BOSS_STATE::STATE_IDLE;
	m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_IDL_CombatNeutral);

	m_pTransform->Set_Scale(3.f);

	m_pTransform->Rotation(YAXIS, XM_PI);
	m_pTransform->Set_Speed(2.f);

	m_pHpNumber = static_cast<CHP_Number*>(m_pGameInstance->Clone_GameObject(L"Prototype_HP_Number"));
	if (nullptr == m_pHpNumber)
		return E_FAIL;

	m_pHpNumber->Set_Offset(XMVectorSet(0.f, 7.f, -1.f, 0.f));

	/* StartPos */
	m_pTransform->Set_Position(XMVectorSet(0.f, 0.f, 20.f, 1.f));
	/* HP */
	m_fCurHp = 20000.f;

	m_fOriginScale = 3.f;

	Bind_KeyFrames();

	m_Effect_Desc.pParentModel = m_pModel;
	m_Effect_Desc.pParentTransform = m_pTransform;

	m_strCinemaActorTag = "DestroyMan";
	CINEMAMGR->Add_CinemaObject(this);

	CINE_EVENT CineEvent{};
	CineEvent.eEventType = ETC;
	CineEvent.pFunc = bind(&CLast_Boss_AW::Fly_High, this);
		
	CINEMAMGR->Bind_ActorEvent("SonicJuice_Enter", m_strCinemaActorTag, 3.25f, CineEvent);

	return S_OK;
}

void CLast_Boss_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Stop())
		return;

	if (false == m_bCinematic)
	{
		if (m_bAppearing)
		{
			if ((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_Loop == m_pModel->Get_CurrentAnimIndex())
			{
				m_pTransform->Go_Down(fTimeDelta);
				EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Shot_Loop", &m_Effect_Desc);
				EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Appear_Thunder", &m_Effect_Desc);

				if (m_pTransform->Get_Position().m128_f32[1] <= 0.f)
				{
					EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Shot_Loop");
					m_pTransform->Set_PositionY(0.f);
					m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_Out);
				}
			}
			else
			{
				if (m_pModel->Is_AnimComplete())
				{
					m_bAppearing = false;
					m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_IDL_CombatNeutral);
					m_pTransform->Set_Speed(30.f);
				}

			}
		}
		else
		{
			Patten_Root(fTimeDelta);

			if (m_bSpark == true)
				Patten_Spark(fTimeDelta);
		}

		m_pHpNumber->Update_Position(m_pTransform->Get_Position());
		m_pHpNumber->Update_RenderHp((_int)m_fCurHp);
	}

	else
	{
		Play_Cinematic(CINEMAMGR->Get_PlayTimeAcc());

		m_pRigidbody->Update(fTimeDelta);
	}

	m_pModel->Play_Animation(fTimeDelta);

	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
}

void CLast_Boss_AW::Hit(const ATTACKDESC& AttackDesc)
{
	if (m_fCurHp <= 0.f || true == m_bAppearing)
		return;

	HitScaling_On();

	m_fCurHp -= (_float)AttackDesc.iDamage;

	if (m_fCurHp <= 0.f)
	{
		EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Surface_L");
		EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Surface_R");
		EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Foot_Particle");

		m_pCollider->Set_Active(false);
		AWMGR->Change_GameState(CAW_Manager::CINEMATIC);
	}
		
}

HRESULT CLast_Boss_AW::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;

	m_bInLayer = true;

	m_pCollider->Set_Active(true);

	m_bAppearing = true;

	m_pTransform->Set_PositionY(10.f);
	m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_Loop);

	return S_OK;
}

void CLast_Boss_AW::OnExit_Layer()
{
	//__super::OnExit_Layer();

	//m_pCollider->Set_Active(false);
}

void CLast_Boss_AW::Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs)
{
	__super::Start_Cinematic(CinematicDescs);

	m_bCinematic = true;

	EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_Boss_Landing_Dust");
	EFFECTMGR->End_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Foot_Particle");
}

void CLast_Boss_AW::End_Cinematic()
{
	if ((_uint)LAST_BOSS_ANI::ANI_CH_Destroyman_DMG_Stun_Loop != m_pModel->Get_CurrentAnimIndex())
		m_bCinematic = false;
}

void CLast_Boss_AW::Bind_KeyFrames()
{
	m_pModel->Bind_Func("Fire", bind(&CLast_Boss_AW::Fire, this));
	m_pModel->Bind_Func("Spark", bind(&CLast_Boss_AW::Spark, this));
	m_pModel->Bind_Func("Landing_Shaking", bind(&CVarious_Camera::Start_VerticalShaking, static_cast<CVarious_Camera*>(m_pGameInstance->Get_MainCamera()),
		1.f, 0.5f, 10));

	m_pModel->Bind_Func("Effect_Landing", bind(&CLast_Boss_AW::Landing_Effect, this));
	m_pModel->Bind_Func("Effect_Punch_Start", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, LEVEL_ARROW_A_ROW, "AA_BOSS_Punch", &m_Effect_Desc, nullptr));
	m_pModel->Bind_Func("Effect_Punch_End", bind(&CEffect_Manager::End_Effect, EFFECTMGR, LEVEL_ARROW_A_ROW, "AA_BOSS_Punch",0));
	m_pModel->Bind_Func("Effect_Spark_Ready0", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, LEVEL_ARROW_A_ROW, "AA_BOSS_Spark_Ready", &m_Effect_Desc, nullptr));
	m_pModel->Bind_Func("Effect_Spark_Ready1", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, LEVEL_ARROW_A_ROW, "AA_BOSS_Spark_Ready1", &m_Effect_Desc, nullptr));
	m_pModel->Bind_Func("Effect_Spark_Smash", bind(&CEffect_Manager::Active_Effect, EFFECTMGR, LEVEL_ARROW_A_ROW, "AA_BOSS_Spark_Smash", &m_Effect_Desc, nullptr));
	m_pModel->Bind_Func("Effect_Foot_Particle_End", bind(&CEffect_Manager::End_Effect, EFFECTMGR, LEVEL_ARROW_A_ROW, "AA_BOSS_Foot_Particle",0));
}

void CLast_Boss_AW::OnCollisionEnter(CGameObject* pOther, CCollider* pCollider)
{
	if ((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyPunch_Loop == m_pModel->Get_CurrentAnimIndex() && TAG_PLAYER == pOther->Get_Tag())
	{
		ATTACKDESC AttackDesc{};
		AttackDesc.iDamage = 50;

		static_cast<CBike_AW*>(pOther)->Hit(AttackDesc);
	}


}

void CLast_Boss_AW::Fire()
{
	if (m_fCurHp <= 0.f)
		return;

	_matrix SocketMatrix = m_pModel->Get_Bone("weapon_r")->Get_CombinedTransformation();
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = SocketMatrix * m_pTransform->Get_WorldMatrix();

	_vector vPlayerPos = AWMGR->Get_Player()->Get_Transform()->Get_Position();

	_vector vLook = vPlayerPos - SocketMatrix.r[3];

	_uint iNumBullets = 5;

	_float fSpreadAngle = 10.f;
	_float fHalfAngle = fSpreadAngle * 0.5f;
	_float fDeltaAngle = fSpreadAngle / (iNumBullets - 1);
	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Cannon_Particle", &m_Effect_Desc);

	for (_uint i = 0; i < iNumBullets; ++i)
	{
		_float fAngle = -fHalfAngle + fDeltaAngle * i;
		_matrix RotationMatrix = XMMatrixRotationAxis(YAXIS, To_Radian(fAngle));

		_vector vBulletDir = XMVector3TransformNormal(vLook, RotationMatrix);
		m_Bullets[m_iBulletIdx]->Get_Transform()->LookTo(vBulletDir);

		m_Bullets[m_iBulletIdx]->Set_BulletEffectTag("AA_BOSS_Cannon");
		m_Bullets[m_iBulletIdx]->Get_Transform()->Set_Position(SocketMatrix.r[3] + XMLoadFloat4(&m_vShotOffset));
		m_Bullets[m_iBulletIdx]->OnEnter_Layer(nullptr);
		Safe_AddRef(m_Bullets[m_iBulletIdx]);
		ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"Layer_Bullet", m_Bullets[m_iBulletIdx]));
		m_iBulletIdx = (m_iBulletIdx + 1) % m_Bullets.size();
	}
}

void CLast_Boss_AW::Spark()
{
	if (m_fCurHp <= 0.f)
		return;

	m_bSpark = true;
}

void CLast_Boss_AW::Fly_High()
{
	m_pRigidbody->Add_Force(XMVectorSet(-0.2f, 10.f, 80.f, 0.f), CRigidbody::VelocityChange);
	m_pRigidbody->Add_Torque(m_pTransform->Get_Look(), 1200.f, CRigidbody::VelocityChange);

	static_cast<CVarious_Camera*>(GET_CAMERA)->Start_VerticalShaking(5.f, 0.2f, 20);

}

HRESULT CLast_Boss_AW::Ready_Components()
{
	CTransform::TRANSFORMDESC tTransformDesc;
	tTransformDesc.fSpeedPerSec = 30.f;
	tTransformDesc.fRotationPerSec = 90.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &tTransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_DestroymanTF"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	// =============================================Collider================================================================

	CCollider::COLLIDERDESC HitDesc;
	HitDesc.eType				= CCollider::SPHERE;
	HitDesc.pOwner				= this;
	HitDesc.vCenter				= { 0.f, 1.f, 0.f };
	HitDesc.vSize				= { 2.f, 0.f, 0.f };
	HitDesc.vRotation			= { 0.f, 0.f, 0.f };
	HitDesc.bActive				= false;
	HitDesc.strCollisionLayer	= "Monster_HitBox";

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

HRESULT CLast_Boss_AW::Ready_Bullets()
{
	CEnemy_Bullet_AW::ENEMY_BULLET_DESC Desc;
	Desc.fBulletAttack		= 10.f;
	Desc.fBulletSpeed		= 15.f;
	Desc.fMaxDist			= 15.f;
	Desc.strBulletEffectTag = "AA_BOSS_Trap";

	m_Bullets.resize(30);
	for (auto& pBullet : m_Bullets)
		pBullet = static_cast<CEnemy_Bullet_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_Enemy_Bullet_AW", &Desc));


	return S_OK;
}

void CLast_Boss_AW::Patten_Root(_float _fTimeDelta)
{
	if (m_pModel->Get_CurrentAnimIndex() == (_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyPunch_Loop) {
		m_pTransform->Go_Straight(_fTimeDelta);

		if (m_pTransform->Get_Position().m128_f32[2] < -1.f)
			m_bPunch = true;
	}
	else if (m_pModel->Get_CurrentAnimIndex() == (_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_Out && m_bDown == false) {
		m_pTransform->Go_Down(_fTimeDelta);

		if (m_pTransform->Get_Position().m128_f32[1] < 0.f) {
			m_pTransform->Set_PositionY(0.f);
			m_bDown = true;
		}
	}

	if (m_pModel->Is_AnimComplete() == true) {
		if (m_eCurState == LAST_BOSS_STATE::STATE_IDLE) {
			m_eCurState = LAST_BOSS_STATE::STATE_CANNON;
			m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyCannon, 0.1f, false);
		}
		else if (m_eCurState == LAST_BOSS_STATE::STATE_CANNON) {
			m_eCurState = LAST_BOSS_STATE::STATE_PUNCH;
			m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyPunch_In, 0.1f, false);
		}
		else if (m_eCurState == LAST_BOSS_STATE::STATE_PUNCH) {
			if (m_pModel->Get_CurrentAnimIndex() == (_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyPunch_In) {
				m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyPunch_Loop);
			}
			else if (m_bPunch == true) {
				m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyPunch_Out, 0.1f, false);
				m_bPunch = false;
			}
			else if (m_pModel->Get_CurrentAnimIndex() == (_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyPunch_Out) {
				m_pTransform->Set_Position(XMVectorSet(0.f, 10.f, 20.f, 1.f));
				m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_Out, 0.1f, false);
			}
			else if (m_bDown == true) {
				m_eCurState = LAST_BOSS_STATE::STATE_SPARK;
				m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_ATK_DestroySpark, 0.1f, false);
				m_bDown = false;
			}
		}
		else if (m_eCurState == LAST_BOSS_STATE::STATE_SPARK) {
			m_eCurState = LAST_BOSS_STATE::STATE_IDLE;
			m_pModel->Change_Animation((_uint)LAST_BOSS_ANI::ANI_CH_DestroymanTF_IDL_CombatNeutral, 0.1f, false);
			m_pTransform->Set_PositionY(0.f);
			m_bSpark = false;
		}
	}
}

void CLast_Boss_AW::Patten_Spark(_float _fTimeDelta)
{
	m_fShootTime += _fTimeDelta;

	if (m_fShootTime > 0.2f) {
		_float4 TargetPos = { 0.f, 0.f, 0.f, 1.f };

		_vector PRight	= XMLoadFloat4(&TargetPos);
		_vector PLeft	= XMLoadFloat4(&TargetPos);

		_vector vMonster = m_pTransform->Get_Position();

		_vector MRight	= vMonster;
		_vector MLeft	= vMonster;

		PRight.m128_f32[0] += 2.f;
		PLeft.m128_f32[0] -= 2.f;

		MRight.m128_f32[0] += 2.f;
		MLeft.m128_f32[0] -= 2.f;

		_int iRandom = JoRandom::Random_Int(0, 2);

		//if (iRandom == 0 && m_iPrevRandom == 1)
		//	iRandom = 4;
		//else if (iRandom == 0 && m_iPrevRandom == 2)
		//	iRandom = 4;

		_vector vLook = {};
		if (iRandom == 0) {
			vLook = JoMath::Calc_GroundLook(XMLoadFloat4(&TargetPos), vMonster);
			m_Bullets[m_iBulletIdx]->Get_Transform()->Set_Position(vMonster);
		}
		else if (iRandom == 1) {
			vLook = JoMath::Calc_GroundLook(PRight, MRight);
			m_Bullets[m_iBulletIdx]->Get_Transform()->Set_Position(MRight);
		}
		else if (iRandom == 2) {
			vLook = JoMath::Calc_GroundLook(PLeft, MLeft);
			m_Bullets[m_iBulletIdx]->Get_Transform()->Set_Position(MLeft);
		}

		//m_iPrevRandom = iRandom;

		m_Bullets[m_iBulletIdx]->Get_Transform()->LookTo(vLook);

		m_Bullets[m_iBulletIdx]->Set_BulletEffectTag("AA_BOSS_Trap");
		m_Bullets[m_iBulletIdx]->OnEnter_Layer(nullptr);
		Safe_AddRef(m_Bullets[m_iBulletIdx]);
		ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"Layer_Bullet", m_Bullets[m_iBulletIdx]));
		m_iBulletIdx = (m_iBulletIdx + 1) % m_Bullets.size();

		m_fShootTime = 0.f;
	}
}

void CLast_Boss_AW::Landing_Effect()
{
	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Boss_Landing_Dust", &m_Effect_Desc);
	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Foot_Particle", &m_Effect_Desc);
}


CLast_Boss_AW* CLast_Boss_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLast_Boss_AW* pInstance = new CLast_Boss_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLast_Boss_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLast_Boss_AW::Clone(void* pArg)
{
	CLast_Boss_AW* pInstance = new CLast_Boss_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLast_Boss_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLast_Boss_AW::Free()
{
	__super::Free();

	for (auto& pBullet : m_Bullets)
		Safe_Release(pBullet);

	Safe_Release(m_pRigidbody);
}

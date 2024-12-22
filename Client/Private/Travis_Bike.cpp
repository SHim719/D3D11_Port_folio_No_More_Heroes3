#include "Travis_Bike.h"

#include "Bone.h"

#include "Travis_Bike_States.h"
#include "Trigger_Manager.h"
#include "Trigger.h"

#include "Travis_World.h"

#include "UI_Manager.h"
#include "UI_World_City.h"

CTravis_Bike::CTravis_Bike(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CTravis_Bike::CTravis_Bike(const CTravis_Bike& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CTravis_Bike::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTravis_Bike::Initialize(void* _pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	Bind_KeyFrames();
	Bind_KeyFrameEffects();
	Bind_KeyFrameSounds();

	m_pTransform->Set_Position(XMVectorSet(-8.81f, 0.218f, 7.669f, 1.f));

	m_pNavigation->Set_CurrentIdx(m_pTransform->Get_Position());

	Change_State((_uint)Travis_Bike_States::State_NonRide);

	Update_NonRide_Colliders();

	return S_OK;
}

void CTravis_Bike::PriorityTick(_float _fTimeDelta)
{
}

void CTravis_Bike::Tick(_float _fTimeDelta)
{
	if (m_States[m_iState])
		m_States[m_iState]->Update(_fTimeDelta);

	m_pModel->Play_Animation(_fTimeDelta);

	m_pRigidbody->Update(_fTimeDelta);

	Compute_YPos();

	if ((_uint)Travis_Bike_States::State_Ride == m_iState)
	{
		EFFECTMGR->Active_Effect(LEVEL_STATIC, "Travis_Vehicle_Trail", &m_Effect_Desc);
		GET_PLAYER_WORLD->Get_Transform()->Attach_To_Bone(m_pModel->Get_Bone("pirot_attach"), m_pTransform, XMMatrixRotationY(XM_PI));
	}

	if ((_uint)Travis_Bike_States::State_Ride == m_iState)
	{
		m_pBikeCollider->Update(m_pTransform->Get_WorldMatrix());
	}
}

void CTravis_Bike::LateTick(_float _fTimeDelta)
{
	if (m_States[m_iState])
		m_States[m_iState]->Late_Update(_fTimeDelta);

	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), 5.f))
	{
		m_pModel->Update_BoneMatrices();

		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_ShadowObject(this);

	}
	
#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
	m_pGameInstance->Add_RenderComponent(m_pHitBoxCollider);
	m_pGameInstance->Add_RenderComponent(m_pRidingCollider);
	m_pGameInstance->Add_RenderComponent(m_pBikeCollider);

#endif
}

HRESULT CTravis_Bike::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{ 
		if (i == 1)
			continue;

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


HRESULT CTravis_Bike::Render_ShadowDepth()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 3)))
			return E_FAIL;
	}
	return S_OK;
}

void CTravis_Bike::Bind_KeyFrames()
{
}

void CTravis_Bike::Bind_KeyFrameEffects()
{
}

void CTravis_Bike::Bind_KeyFrameSounds()
{
}

void CTravis_Bike::OnCollisionEnter(CGameObject* _pOther, CCollider* pCollider)
{
	if (TAG_PLAYER == _pOther->Get_Tag())
		static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->Set_BikeUI(true, m_pTransform->Get_Position());

	if (pCollider->Get_ColliderTag() == "NPC_CarHitBox") {
		if (m_pRigidbody->Get_VelocityLength() >= 20.f) {
			ATTACKDESC tAttackDesc = {};
			tAttackDesc.iDamage = 10;
			static_cast<CCharacter*>(_pOther)->Hit(tAttackDesc);
		}
	}

	if (_pOther->Get_Tag() == TAG_TRIGGER) {
		wstring strTriggerName = static_cast<CTrigger*>(_pOther)->Get_Name();

		if (strTriggerName == L"Trigger_Portal_WC_to_AAR") {
			CFadeScreen::FADEDESC FD = {};
			FD.eFadeColor = CFadeScreen::BLACK;
			FD.fFadeOutSpeed = 0.5f;
			FD.fFadeInSpeed = 144.f;
			FD.pCallback_FadeOutEnd = []()->void {
				CTrigger_Manager::Get_Instance()->Exec_Trigger(L"Trigger_Portal_WC_to_AAR");
				};

			static_cast<CTravis_World*>(GET_PLAYER_WORLD)->Apply_FadeIO(FD);
		}
	}
}

void CTravis_Bike::OnCollisionExit(CGameObject* _pOther, CCollider* pCollider)
{
	if (TAG_PLAYER == _pOther->Get_Tag())
		static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->Set_BikeUI(false);
}

HRESULT CTravis_Bike::OnEnter_Layer(void* pArg)
{
	Update_NonRide_Colliders();
	
	return S_OK;
}

void CTravis_Bike::Get_Off()
{
	Change_State((_uint)Travis_Bike_States::State_NonRide);

	GET_PLAYER_WORLD->GetOff_Bike(m_pNavigation->Get_CurrentIdx());

	EFFECTMGR->Inactive_Effect(GET_CURLEVEL, "Travis_Vehicle_Trail");

}

void CTravis_Bike::Set_Active_Colliders_By_RidingState(_bool bRide)
{
	m_pBikeCollider->Set_Active(bRide);

	m_pCollider->Set_Active(!bRide);
	m_pHitBoxCollider->Set_Active(!bRide);
	m_pRidingCollider->Set_Active(!bRide);

	if (false == bRide)
	{
		Update_NonRide_Colliders();
	}

}

void CTravis_Bike::Update_NonRide_Colliders()
{
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());
	m_pRidingCollider->Update(m_pTransform->Get_WorldMatrix());
}



void CTravis_Bike::Compute_YPos()
{
	if (nullptr == m_pNavigation)
		return;

	_vector vCurPos = m_pTransform->Get_Position();
	_float fCurHeight = m_pNavigation->Decide_YPos(vCurPos);
	vCurPos.m128_f32[1] = fCurHeight;
	m_pTransform->Set_Position(vCurPos);

}


HRESULT CTravis_Bike::Bind_ShaderResources()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	return S_OK;
}

HRESULT CTravis_Bike::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = 360.0f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	CRigidbody::RIGIDBODY_DESC RigidDesc{};
	RigidDesc.pOwnerTransform = m_pTransform;
	RigidDesc.fMass = 1.f;
	RigidDesc.fFrictionScale = 1.f;
	RigidDesc.fAngularFrictionScale = 60.f;
	RigidDesc.fMaxAngularVelocity = 60.f;
	RigidDesc.vMaxVelocity = { 30.f, 30.f, 30.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Rigidbody"), TEXT("Rigidbody"), (CComponent**)&m_pRigidbody, &RigidDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Travis_Bike"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc = {};
	ColliderDesc.eType		= CCollider::SPHERE;
	ColliderDesc.pOwner		= this;
	ColliderDesc.vCenter	= { 0.f, 1.f, -1.25f };
	ColliderDesc.vSize		= { 2.5f, 2.5f, 2.5f };
	ColliderDesc.bActive	= true;
	ColliderDesc.strCollisionLayer = "Monster_HitBox";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider1"), (CComponent**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 0.f, 1.f, 1.25f };
	ColliderDesc.vSize = { 2.5f, 2.5f, 2.5f };
	ColliderDesc.bActive = true;
	ColliderDesc.strCollisionLayer = "Monster_HitBox";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider2"), (CComponent**)&m_pHitBoxCollider, &ColliderDesc)))
		return E_FAIL;


	CCollider::COLLIDERDESC BikeColliderDesc = {};
	BikeColliderDesc.eType				= CCollider::SPHERE;
	BikeColliderDesc.pOwner				= this;
	BikeColliderDesc.vCenter			= { 0.f, 1.f, 0.f };
	BikeColliderDesc.vSize				= { 2.f, 0.f, 0.f };
	BikeColliderDesc.bActive			= false;
	BikeColliderDesc.strCollisionLayer	= "Travis_Bike";
	BikeColliderDesc.strColliderTag		= "Travis_Bike_Collider";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("BikeCollider"), (CComponent**)&m_pBikeCollider, &BikeColliderDesc)))
		return E_FAIL;

	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = this;
	ColliderDesc.vCenter = { 1.f, 1.f, 0.f };
	ColliderDesc.vSize = { 1.f, 0.f, 0.f };
	ColliderDesc.bActive = true;
	ColliderDesc.strCollisionLayer = "Travis_Bike";
	ColliderDesc.strColliderTag = "Riding_Bike_Collider";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Riding_Trigger"), (CComponent**)&m_pRidingCollider, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Navigation"), TEXT("Navigation"), (CComponent**)&m_pNavigation)))
		return E_FAIL;

	for (auto& pState : m_States) {
		pState->Set_Navigation(m_pNavigation);
	}

	m_pRigidbody->Change_Navigation(m_pNavigation);

	m_pModel->Set_AnimPlay();

	return S_OK;
}

HRESULT CTravis_Bike::Ready_States()
{
	m_States.resize((_uint)Travis_Bike_States::State_End);

	m_States[(_uint)Travis_Bike_States::State_NonRide] = CTravis_Bike_State_NonRide::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)Travis_Bike_States::State_Ride] = CTravis_Bike_State_Ride::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

CTravis_Bike* CTravis_Bike::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTravis_Bike* pInstance = new CTravis_Bike(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CTravis_Bike");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTravis_Bike::Clone(void* _pArg)
{
	CTravis_Bike* pInstance = new CTravis_Bike(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTravis_Bike"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_Bike::Free()
{
	__super::Free();

	Safe_Release(m_pRigidbody);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pRidingCollider);
	Safe_Release(m_pBikeCollider);
}

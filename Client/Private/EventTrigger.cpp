#include "EventTrigger.h"

#include "Cutscene_Manager.h"
#include "Player.h"

CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (m_pCollider)
		m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	return S_OK;
}


void CEventTrigger::Tick(_float fTimeDelta)
{

}

void CEventTrigger::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
}

void CEventTrigger::OnCollisionEnter(CGameObject* pOther)
{
	switch (m_eTriggerEvent)
	{
	case START_ODUR_CUTSCENE:
		CUTSCENEMGR->OnEnter_Cutscene(ENCOUNTER_ODUR);
		ADD_EVENT(bind(&CGameObject::Set_Destroy, this, true));
		break;
	case START_URD_CUTSCENE:
		CUTSCENEMGR->OnEnter_Cutscene(ENCOUNTER_URD);
		ADD_EVENT(bind(&CGameObject::Set_Destroy, this, true));
		break;
	case CHANGE_FOOTSTEP_SOUND:
	{
		CPlayer* pPlayer = GET_PLAYER;
		wstring FootStepTag = pPlayer->Get_FootStepTag();
		if (L'W' == FootStepTag[9])
			FootStepTag = L"FootStep_Grass";
		else
			FootStepTag = L"FootStep_Wood";
		pPlayer->Set_FootStepTag(FootStepTag);
		break;
	}
	}
	

}


HRESULT CEventTrigger::Ready_Components(void* pArg)
{
	LOADOBJDESC* pLoadDesc = (LOADOBJDESC*)pArg;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	CCollider::COLLIDERDESC Desc;
	Desc.eType = CCollider::AABB;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.f, 0.f };
	Desc.vSize = pLoadDesc->vColliderSize;
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.bActive = true;
	Desc.strCollisionLayer = "EventTrigger";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_AABB"), TEXT("Collider"), (CComponent**)&m_pCollider, &Desc)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	m_eTriggerEvent = (TRIGGEREVENTS)pLoadDesc->iTriggerIdx;

	return S_OK;
}


CEventTrigger* CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEventTrigger* pInstance = new CEventTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEventTrigger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
	CEventTrigger* pInstance = new CEventTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEventTrigger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
}

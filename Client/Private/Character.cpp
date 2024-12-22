#include "Character.h"
#include "State_Base.h"

#include "Weapon.h"


CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
{
}


HRESULT CCharacter::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_Effect_Desc.pParentModel = m_pModel;
	m_Effect_Desc.pParentTransform = m_pTransform;

	if (nullptr != m_Effect_Desc.pParentModel)
		Safe_AddRef(m_Effect_Desc.pParentModel);
	if (nullptr != m_Effect_Desc.pParentTransform)
		Safe_AddRef(m_Effect_Desc.pParentTransform);

	return S_OK;
}

HRESULT CCharacter::Render_ShadowDepth()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 3)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCharacter::Render_Cascade()
{
	
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CCharacter::Bind_KeyFrames()
{
}

void CCharacter::Bind_KeyFrameEffects()
{
}

void CCharacter::Bind_KeyFrameSounds()
{
}

void CCharacter::Decide_PassIdx()
{
	if (m_bDissolve)
		m_iPassIdx = 1;
	else
		m_iPassIdx = 0;
}

HRESULT CCharacter::Bind_DissolveDescs()
{
	if (FAILED(m_pDissolveTexture->Set_SRV(m_pShader, "g_DissolveTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
		return E_FAIL;

	_float4 vDissolveDirection;
	XMStoreFloat4(&vDissolveDirection, m_pTransform->Get_Up() * -1.f);

	_float4 vDissolveOrigin;
	XMStoreFloat4(&vDissolveOrigin, m_pTransform->Get_Position() + m_pTransform->Get_Up() * 2.f);

	if (FAILED(m_pShader->Bind_RawValue("g_vDissolveDirection", &vDissolveDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vDissolveOrigin", &vDissolveOrigin, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vDissolveDiffuse", &m_vDissolveDiffuse, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Change_State(_uint eState, void* pArg)
{
	m_iPrevState = m_iState;

	m_States[m_iPrevState]->OnState_End();

	m_iState = eState;

	m_States[m_iState]->OnState_Start(pArg);
}


void CCharacter::Hit(const ATTACKDESC& AttackDesc)
{
 	m_States[m_iState]->OnHit(AttackDesc);
}

_int CCharacter::Take_Damage(const ATTACKDESC& AttackDesc)
{
	return 0;
}

void CCharacter::OnDeath()
{
}

void CCharacter::Tick_Weapons(_float fTimeDelta)
{
	for (auto& pWeapon : m_Weapons)
		pWeapon->Tick(fTimeDelta);
}

void CCharacter::LateTick_Weapons(_float fTimeDelta)
{
	for (auto& pWeapon : m_Weapons)
		pWeapon->LateTick(fTimeDelta);
}

void CCharacter::Update_Colliders()
{
	if (m_pCollider)
		m_pCollider->Update(m_pTransform->Get_WorldMatrix());

	if (m_pHitBoxCollider)
		m_pHitBoxCollider->Update(m_pTransform->Get_WorldMatrix());
}



HRESULT CCharacter::Bind_RimLightDescs()
{
	if (FAILED(m_pShader->Bind_RawValue("g_vRimColor", &m_tRimLightDesc.vRimColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fRimPower", &m_tRimLightDesc.fRimPower, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_States.size(); ++i)
		Safe_Release(m_States[i]);

	for (auto& pWeapon : m_Weapons)
		Safe_Release(pWeapon);
	m_Weapons.clear();

	Safe_Release(m_Effect_Desc.pParentTransform);
	Safe_Release(m_Effect_Desc.pParentModel);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
	Safe_Release(m_pHitBoxCollider);	// BH - 20240901 : Add Releasing Hit Box Collider
}
#include "Travis.h"

#include "Virtual_Bone.h"
#include "Various_Camera.h"
#include "Travis_Utility_LockOn_Remastered.h"

_bool CTravis::s_bInBattle = { false };

CTravis::CTravis(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter{ _pDevice,_pContext }
{
}

CTravis::CTravis(const CTravis& _rhs)
	: CCharacter{ _rhs }
{
}

HRESULT CTravis::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iTag = (_uint)TAG_PLAYER;

	return S_OK;
}

HRESULT CTravis::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Utilities()))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;

	m_pFadeScreen = static_cast<CFadeScreen*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_FadeScreen"));
	if (nullptr == m_pFadeScreen)
		return E_FAIL;

	return S_OK;
}

void CTravis::PriorityTick(_float _fTimeDelta)
{
}

void CTravis::Tick(_float _fTimeDelta)
{
}

void CTravis::LateTick(_float _fTimeDelta)
{
}

HRESULT CTravis::Render()
{
	return S_OK;
}

HRESULT CTravis::Render_Cascade()
{
	return S_OK;
}

HRESULT CTravis::Render_ShadowDepth()
{
	return S_OK;
}

void CTravis::Bind_KeyFrames()
{
}

void CTravis::Bind_KeyFrameEffects()
{
}

void CTravis::Bind_KeyFrameSounds()
{
}

void CTravis::OnCollisionEnter(CGameObject* _pOther, CCollider* _pCollider)
{
}

void CTravis::OnCollisionStay(CGameObject* _pOther, CCollider* _pCollider)
{
}

void CTravis::OnCollisionExit(CGameObject* _pOther, CCollider* _pCollider)
{
}

HRESULT CTravis::OnEnter_Layer(void* pArg)
{
	if(FAILED(__super::OnEnter_Layer(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CTravis::OnExit_Layer()
{
}

_vector CTravis::Get_Center() const
{
	return _vector();
}

_vector CTravis::Get_TopPos() const
{
	return _vector();
}

_int CTravis::Take_Damage(const ATTACKDESC& _AttackDesc)
{
	return _int();
}

HRESULT CTravis::Bind_ShaderResources()
{
	return S_OK;
}

CGameObject* CTravis::Get_CurLockOnTarget()
{
	return m_pUtility_LockOn_Remastered->Get_CurLockOnTarget();
}

_bool CTravis::Get_HasTarget() const
{
	return (m_pUtility_LockOn_Remastered->Get_CurLockOnTarget() != nullptr) ? true : false;
}

_bool CTravis::Get_IsLockOn() const
{
	if ((m_pUtility_LockOn_Remastered->Get_CurLockOnTarget() != nullptr) && m_pUtility_LockOn_Remastered->Get_IsLockOn())
		return true;
	else
		return false;
}

_vector CTravis::Get_LockOnTargetCenterPos() const
{
	if (!Get_HasTarget())
		return XMVectorSet(0.f, 0.f, 0.f, 1.f);

	return m_pUtility_LockOn_Remastered->Get_CurLockOnTarget()->Get_Center();
}

_vector CTravis::Get_LockOnTargetDir() const
{
	if (!Get_HasTarget())
		return m_pTransform->Get_GroundLook();

	return XMVector4Normalize(XMVectorSetY(m_pUtility_LockOn_Remastered->Get_CurLockOnTarget()->Get_Center() - m_pTransform->Get_Position(), 0.f));
}

_vector CTravis::Get_VirtualBonePos() const
{
	return m_pVirtualBone->Get_VirtualBonePos();
}

void CTravis::Start_Battle()
{
	s_bInBattle = true;
}

HRESULT CTravis::Ready_Camera()
{
	CVarious_Camera::VARCAM_DESC PlayerCameraDesc = {};
	PlayerCameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	PlayerCameraDesc.fNear = 0.1f;
	PlayerCameraDesc.fFar = 1000.f;
	PlayerCameraDesc.fFovy = 60.f;
	PlayerCameraDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	PlayerCameraDesc.vEye = { 0.f, 2.f, -2.f, 1.f };
	PlayerCameraDesc.pOwner = this;

	m_pPlayerCamera = static_cast<CVarious_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_VariousCamera", &PlayerCameraDesc));
	if (nullptr == m_pPlayerCamera)
		return E_FAIL;

	return S_OK;
}

HRESULT CTravis::Ready_Utilities()
{
	Safe_Release(m_pUtility_LockOn_Remastered);

	m_pUtility_LockOn_Remastered = CTravis_Utility_LockOn_Remastered::Create(this);
	if (nullptr == m_pUtility_LockOn_Remastered)
		return E_FAIL;

	return S_OK;
}

void CTravis::Free()
{
	__super::Free();

	Safe_Release(m_pModel_Camera);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pVirtualBone);

	Safe_Release(m_pPlayerCamera);
	Safe_Release(m_pFadeScreen);
	Safe_Release(m_pUtility_LockOn_Remastered);
}

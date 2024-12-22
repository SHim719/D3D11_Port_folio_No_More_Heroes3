#include "Various_Camera.h"

#include "GameInstance.h"

#include "Travis.h"
#include "GameEffect.h"

CVarious_Camera::CVarious_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera{ _pDevice,_pContext }
{
}

CVarious_Camera::CVarious_Camera(const CVarious_Camera& _rhs, CTransform::TRANSFORMDESC* _pArg)
	: CCamera{ _rhs,_pArg }
{
}

HRESULT CVarious_Camera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVarious_Camera::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	VARCAM_DESC* pDesc = static_cast<VARCAM_DESC*>(_pArg);
	m_pOwner = pDesc->pOwner;

	if (FAILED(Ready_States()))
		return E_FAIL;

	return S_OK;
}

void CVarious_Camera::Tick(_float _fTimeDelta)
{
	//RECT rc = {};
	//POINT p1 = {}, p2 = {};
	//GetClientRect(g_hWnd, &rc);
	//p1.x = rc.left;
	//p1.y = rc.top;
	//p2.x = rc.right;
	//p2.y = rc.bottom;
	//ClientToScreen(g_hWnd, &p1);
	//ClientToScreen(g_hWnd, &p2);
	//rc.left = p1.x+100;
	//rc.top = p1.y+100;
	//rc.right = p2.x-100;
	//rc.bottom = p2.y-100;
	//ClipCursor(&rc);


	m_States[m_iCurStateIndex]->Update(_fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix_Origin, m_pTransform->Get_WorldMatrix());

	Rolling(_fTimeDelta);
	VerticalShaking(_fTimeDelta);
	ZoomInOut(_fTimeDelta);
}

void CVarious_Camera::LateTick(_float _fTimeDelta)
{
	m_States[m_iCurStateIndex]->Late_Update(_fTimeDelta);

	//Set_CursorToCenter(g_hWnd, g_iWinSizeX, g_iWinSizeY);
}

HRESULT CVarious_Camera::Render()
{
	return S_OK;
}

void CVarious_Camera::Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs)
{
	__super::Start_Cinematic(CinematicDescs);

	Change_State((_uint)VARCAM_STATES::STATE_CINEMATIC_JO);
}

void CVarious_Camera::Update_ViewProj()
{
	if (m_bRolling||m_bShaking) {
		//_matrix WorldMatrixOrigin = m_pTransform->Get_WorldMatrix();

		//m_pTransform->LookAt(GET_PLAYER->Get_LockOnCenter());

		__super::Update_ViewProj();

		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix_Origin));
	}
	else {
		__super::Update_ViewProj();
	}
}

void CVarious_Camera::OnActive()
{
	//m_iCurStateIndex = (_uint)VARCAM_STATES::STATE_BACKVIEW;
	//m_States[m_iCurStateIndex]->EnterState(nullptr);
}

void CVarious_Camera::Initial_Setting(_uint _iStateIndex, void* _pArg)
{
	m_iCurStateIndex = _iStateIndex;
	m_States[m_iCurStateIndex]->EnterState(_pArg);
}

void CVarious_Camera::Set_Navigation(_uint _iLevelIndex, const wstring& _strNavigationTag)
{
	Safe_Release(m_pNavigation);

	auto iter = m_Components.find(L"Navigation");
	if (iter != m_Components.end()) {
		Safe_Release(iter->second);
		m_Components.erase(iter);
	}

	if (SUCCEEDED(__super::Add_Component(_iLevelIndex, _strNavigationTag, L"Navigation", reinterpret_cast<CComponent**>(&m_pNavigation)))) {
		m_pNavigation->Set_CurrentIdx(m_pTransform->Get_Position());

		for (auto& iter : m_States) {
			iter->Set_Navigation(m_pNavigation);
		}
	}
	else {
		for (auto& iter : m_States) {
			iter->Set_Navigation(nullptr);
		}
	}
}

void CVarious_Camera::Set_NoNavigation()
{
	auto iter = m_Components.find(L"Navigation");
	if (iter != m_Components.end()) {
		Safe_Release(iter->second);
		m_Components.erase(iter);
	}

	m_pNavigation = nullptr;

	for (auto& iter : m_States) {
		iter->Set_Navigation(nullptr);
	}
}

HRESULT CVarious_Camera::Ready_States()
{
	// Common
	m_States[(_uint)VARCAM_STATES::STATE_ATTACHED_TO_BONE]					= CVarious_Camera_State_AttachedToBone::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_BACKVIEW] = CVarious_Camera_State_BackView::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_CINEMATIC] = CVarious_Camera_State_Cinematic::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_LERPING_BEFORE_ATTACHED_TO_BONE]	= CVarious_Camera_State_LerpingBeforeAttachedToBone::Create(m_pDevice, m_pContext, this);

	// Ground
	m_States[(_uint)VARCAM_STATES::STATE_ARMOR_MULTIMISSILE]				= CVarious_Camera_State_Armor_MultiMissile::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_ARMOR_BACKVIEW]					= CVarious_Camera_State_Armor_BackView::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_LERPING]							= CVarious_Camera_State_Lerping::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_LOCKON]							= CVarious_Camera_State_LockOn::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_PLAYER_APPEARING] = CVarious_Camera_State_Player_Appearing::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_SWORD_TO_SWORD]					= CVarious_Camera_State_SwordToSword::Create(m_pDevice, m_pContext, this);

	// Space
	m_States[(_uint)VARCAM_STATES::STATE_SPACEAPPEARING]					= CVarious_Camera_State_Space_Appearing::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_SPACEAIMING]						= CVarious_Camera_State_Space_RailGun_Light::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_SPACELERPING]						= CVarious_Camera_State_SpaceLerping::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_SPACEVIEW]							= CVarious_Camera_State_SpaceView::Create(m_pDevice, m_pContext, this);

	// World
	m_States[(_uint)VARCAM_STATES::STATE_INTRO]								= CVarious_Camera_State_Intro::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_TOILET]							= CVarious_Camera_State_Toilet::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_TRASHCOLLECTOR]					= CVarious_Camera_State_TrashCollector::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_WORLD_CCTV]						= CVarious_Camera_State_WorldCCTV::Create(m_pDevice, m_pContext, this);

	// Mario & SmashBros
	m_States[(_uint)VARCAM_STATES::STATE_MARIO]								= CVarious_Camera_State_Mario::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_SMASHBROS]							= CVarious_Camera_State_SmashBros::Create(m_pDevice, m_pContext, this);

	// Arrow A Row
	m_States[(_uint)VARCAM_STATES::STATE_AW_DEFAULT]						= CVarious_Camera_State_AW_Default::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)VARCAM_STATES::STATE_AW_BOSSVIEW]						= CVarious_Camera_State_AW_BossView::Create(m_pDevice, m_pContext, this);

	// Cinematic
	m_States[(_uint)VARCAM_STATES::STATE_CINEMATIC_JO] = CVarious_Camera_State_Cinematic_Jo::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

void CVarious_Camera::Rolling(_float _fTimeDelta)
{
	if (m_bRolling) {
		m_fTimeAcc_Roll += _fTimeDelta;

		if (m_fTimeAcc_Roll <= m_fRollingDuration) {
			_matrix RollingMatrix = XMMatrixRotationAxis(m_pTransform->Get_Look(), To_Radian(sinf(m_fTimeAcc_Roll * XM_2PI * m_iRollingCount / m_fRollingDuration) * m_fRollingPower * (1.f - (m_fTimeAcc_Roll / m_fRollingDuration))));
			m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix_Origin) * RollingMatrix);
		}
		else
			Finish_Rolling();
	}
}

void CVarious_Camera::VerticalShaking(_float _fTimeDelta)
{
	if (m_bShaking) {
		m_fTimeAcc_Shake += _fTimeDelta;

		if (m_fTimeAcc_Shake <= m_fShakingDuration) {
			_matrix ShakingMatrix = XMMatrixRotationAxis(m_pTransform->Get_Right(), To_Radian(sinf(m_fTimeAcc_Shake * XM_2PI * m_iShakingCount / m_fShakingDuration) * m_fShakingPower * (1.f - (m_fTimeAcc_Shake / m_fShakingDuration))));
			m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix_Origin) * ShakingMatrix);

		}
		else
			Finish_VerticalShaking();
	}
}

void CVarious_Camera::ZoomInOut(_float _fTimeDelta)
{
	if (m_bZoomIO) {
		m_fTimeAcc_ZoomIO += _fTimeDelta;

		if (m_fTimeAcc_ZoomIO <= m_fZoomIODuration*0.5f) {
			_float fCurFov = JoMath::Lerp(m_fFov_Origin, m_fFov_Dest, m_fTimeAcc_ZoomIO / (m_fZoomIODuration * 0.5f));
			Set_FovY(fCurFov);
		}
		else if (m_fZoomIODuration * 0.5f < m_fTimeAcc_ZoomIO && m_fTimeAcc_ZoomIO <= m_fZoomIODuration) {
			_float fCurFov = JoMath::Lerp(m_fFov_Dest, m_fFov_Origin, (m_fTimeAcc_ZoomIO - (m_fZoomIODuration * 0.5f)) / (m_fZoomIODuration * 0.5f));
			Set_FovY(fCurFov);
		}
		else
			Finish_ZoomIO();
	}
}

void CVarious_Camera::Change_State(_uint _iStateIndex, void* _pArg)
{
	m_States[m_iCurStateIndex]->ExitState();

	m_iCurStateIndex = _iStateIndex;

	m_States[m_iCurStateIndex]->EnterState(_pArg);
}

void CVarious_Camera::Start_Rolling(_float _fRollingPower, _float _fRollingDuration, _uint _iRollingCount)
{
	XMStoreFloat4x4(&m_WorldMatrix_Origin, m_pTransform->Get_WorldMatrix());
	m_fRollingPower = _fRollingPower;
	m_fRollingDuration = _fRollingDuration;
	m_fTimeAcc_Roll = 0.f;
	m_iRollingCount = _iRollingCount;
	m_bRolling = true;
}

void CVarious_Camera::Start_VerticalShaking(_float _fShakingPower, _float _fShakingDuration, _uint _iShakingCount)
{
	XMStoreFloat4x4(&m_WorldMatrix_Origin, m_pTransform->Get_WorldMatrix());
	m_fShakingPower = _fShakingPower;
	m_fShakingDuration = _fShakingDuration;
	m_fTimeAcc_Shake = 0.f;
	m_iShakingCount = _iShakingCount;
	m_bShaking = true;
}

void CVarious_Camera::Start_ZoomIO(_float _fStartFov, _float _fEndFov, _float _fIODuration)
{
	m_fFov_Origin = _fStartFov;
	m_fFov_Dest = _fEndFov;
	m_fZoomIODuration = _fIODuration;
	m_fTimeAcc_ZoomIO = 0.f;
	m_bZoomIO = true;
}

void CVarious_Camera::Finish_Rolling()
{
	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix_Origin));
	m_bRolling = false;
}

void CVarious_Camera::Finish_VerticalShaking()
{
	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix_Origin));
	m_bShaking = false;
}

void CVarious_Camera::Finish_ZoomIO()
{
	Set_FovY(m_fFov_Origin);
	m_bZoomIO = false;
}

void CVarious_Camera::Bind_Triggers(_uint _iLevelIndex)
{
	switch (_iLevelIndex) {
	case (_uint)LEVEL_WORLD_HOME: 
	{
		static_cast<CVarious_Camera_State_WorldCCTV*>(m_States[(_uint)VARCAM_STATES::STATE_WORLD_CCTV])->Trigger_Batch_CCTVs();
	}
	break;
	}
}

CVarious_Camera* CVarious_Camera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVarious_Camera* pPrototype = new CVarious_Camera(_pDevice, _pContext);

	if (FAILED(pPrototype->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CVarious_Camera"));
		Safe_Release(pPrototype);
	}

	return pPrototype;
}

CGameObject* CVarious_Camera::Clone(void* _pArg)
{
	CTransform::TRANSFORMDESC* pTransformDesc = (CTransform::TRANSFORMDESC*)_pArg;

	CGameObject* pClone = new CVarious_Camera(*this, pTransformDesc);

	if (FAILED(pClone->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CVarious_Camera"));
		Safe_Release(pClone);
	}

	return pClone;
}

void CVarious_Camera::Free()
{
	__super::Free();

	Safe_Release(m_pNavigation);

	for (_uint i = 0; i < (_uint)VARCAM_STATES::STATES_END; ++i)
		Safe_Release(m_States[i]);
}

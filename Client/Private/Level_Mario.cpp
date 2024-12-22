#include "Level_Mario.h"

#include "GameInstance.h"

#include "Free_Camera.h"
#include "MapLoader.h"
#include "Travis_Manager.h"

#include "SmashBros_Terrain.h"
#include "Gimmick_Mario_Background.h"

/* Monster */
#include "Killer.h"
#include "Goomba.h"
#include "Togezo.h"
/* SmashBros_Travis */
#include "SmashBros_Travis.h"

#include "Effect_Manager.h"
#include "UI_Manager.h"

#include "Trigger_Manager.h"
#include "Levels_Header.h"


CLevel_Mario::CLevel_Mario(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Mario::Initialize()
{
	PLAY_SOUND("BGM - Overworld", true, 0.5f);

	m_iLevelID = (_uint)LEVEL_MARIO;
	SET_CURLEVEL(m_iLevelID);

	m_pTriggerManager = CTrigger_Manager::Create();
	if (nullptr == m_pTriggerManager)
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Ready_Camera()))
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(Ready_Map()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_Monster()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	if (FAILED(Ready_Background()))
		return E_FAIL;

	Bind_Trigger_Functions();

	if (nullptr == m_pGameInstance->Add_Clone(LEVEL_MARIO, L"Layer_EndPrtal", L"Prototype_Mario_EndPortal"))
		return E_FAIL;

	// TODO : Add Additional Code Here.
	m_pGameInstance->Set_Active_Shadow(false);

	m_pGameInstance->Set_Active_Sky(false);

	return S_OK;
}

void CLevel_Mario::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}
#endif // _DEBUG

	// TODO : Add Additional Code Here.

	if (m_bCreKillerEnd == false)
		Killer_Shot(fTimeDelta);

	if (m_bCreTogezoEnd == false)
		Togezo_WaveGame(fTimeDelta);

	if (!m_bLevelClear && GET_PLAYER_SMASH->Get_Transform()->Get_Position().m128_f32[0] > -21.5f)
	{
		STOP_SOUND("BGM - PowerfulMario");
		PLAY_SOUND("Retro-Level-Clear", false, 1.f);
		m_bLevelClear = true;
	}

	/* Effect */

	if (-1 == m_iEffectIndex)
	{
		EFFECTMGR->Active_Effect(LEVEL_MARIO, "Mario_BackGroundEffect", nullptr, &m_iEffectIndex);
	}

	Check_Level_Change();
}


HRESULT CLevel_Mario::Render()
{
	return S_OK;
}

void CLevel_Mario::OnExitLevel()
{
	m_pGameInstance->Set_Active_Sky(false);

	// TODO : Add Additional Code Here.
}

#ifdef _DEBUG

HRESULT CLevel_Mario::Ready_Camera()
{
	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 1000.f;
	camDesc.fFovy = 60.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	m_pDebugFreeCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));
	if (nullptr == m_pDebugFreeCamera)
		return E_FAIL;

	return S_OK;
}

#endif // _DEBUG

HRESULT CLevel_Mario::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Level_Mario.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(CMapLoader::Get_Instance()->Load_Terrain(L"../../Resources/DataFiles/TriggerData/PlatformColliders_Mario.dat", m_iLevelID)))
		return E_FAIL;

	if (FAILED(CMapLoader::Get_Instance()->Load_Trigger(L"../../Resources/DataFiles/TriggerData/Trigger_Mario.dat", m_iLevelID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Mario::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::SMASHBROS_TRAVIS);

	return S_OK;
}

HRESULT CLevel_Mario::Ready_Monster()
{
	m_vecKiller.resize(5);
	m_vecGoomba.resize(4);
	m_vecTogezo.resize(50);

	/* Killer */
	for (_int i = 0; i < 5; i++)
		m_vecKiller[i] = static_cast<CKiller*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Killer"));

	/* Goomba */
	for (_int i = 0; i < 4; i++) {
		m_vecGoomba[i] = static_cast<CGoomba*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Goomba"));
		m_vecGoomba[m_iGoomba]->OnEnter_Layer(nullptr);
		m_pGameInstance->Insert_GameObject(LEVEL_MARIO, L"Layer_Enemy", m_vecGoomba[m_iGoomba]);
		Safe_AddRef(m_vecGoomba[m_iGoomba++]);
	}
	m_iGoomba = 0;

	/* Togezo */
	for (_int i = 0; i < 50; i++)
		m_vecTogezo[i] = static_cast<CTogezo*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_Togezo"));

	return S_OK;
}

HRESULT CLevel_Mario::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Mario", L"Prototype_UI_Mario"))
		return E_FAIL;

	UIMGR->GetScene(L"SCENE_Mario")->SceneInit();

	return S_OK;
}

HRESULT CLevel_Mario::Ready_Background()
{
	pair<wstring, _bool> BackgroundTex[] =
	{
		{ L"Sky", true },
		{ L"Cloud", true },
		{ L"PillarBack", false },
		{ L"PillarFront", false },
		{ L"Bush", false },
	};

	_int iTexCount = sizeof(BackgroundTex) / sizeof(BackgroundTex[0]);

	for (_int i = 0; i < iTexCount; ++i)
	{
		wstring wstrLayerTag = L"Mario_" + BackgroundTex[i].first;
		CGimmick_Mario_Background::MARIO_BGDESC     Mario_BgDesc{};
		Mario_BgDesc.fScale = (iTexCount - i) * 100.f;
		Mario_BgDesc.wstrTag = BackgroundTex[i].first;
		Mario_BgDesc.IsMoving = BackgroundTex[i].second;
		Mario_BgDesc.fSpeedPerSec = 10.f;
		Mario_BgDesc.fRotationPerSec = 0.f;

		for (_int j = 0; j < 3; ++j)
		{
			CGameObject* pObj = m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Mario_Background", L"Prototype_Mario_Background", &Mario_BgDesc);
			pObj->Get_Transform()->Set_Position(XMVectorSet(Mario_BgDesc.fScale * (j - 1), 10.f, Mario_BgDesc.fScale / 2.f, 1.f));
		}
	}

	return S_OK;
}

void CLevel_Mario::Check_Level_Change()
{
	if (m_bGoLevel_SmashBros) {
		UIMGR->ChangeLevel();
		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_SMASHBROS_BOSS_DAMON, m_pDevice, m_pContext));
	}
}

void CLevel_Mario::Bind_Trigger_Functions()
{
	//m_pTriggerManager->Bind_Func(L"Trigger_Portal_Mario_to_SmashBros",
	//	bind(&CLevel_Mario::Go_Level_SmashBros, this));

	m_pTriggerManager->Bind_Func(L"Trigger_Portal_Mario_to_SmashBros",
		bind(&CLevel_Mario::Start_EndCutScene, this));
	
}

void CLevel_Mario::Killer_Shot(_float _fTimeDelta)
{
	/* Killer */
	m_fCreKillerTime += _fTimeDelta;

	if (m_fCreKillerTime > 5.f) {
		m_vecKiller[m_iKillerIndex]->OnEnter_Layer(nullptr);
		m_pGameInstance->Insert_GameObject(LEVEL_MARIO, L"Layer_Enemy", m_vecKiller[m_iKillerIndex]);
		Safe_AddRef(m_vecKiller[m_iKillerIndex++]);

		m_fCreKillerTime = 0.f;

		if (m_iKillerIndex >= 5)
			m_iKillerIndex = 0;
	}

	if (GET_PLAYER_SMASH->Get_Transform()->Get_Position().m128_f32[0] > -67.f)
		m_bCreKillerEnd = true;
}

void CLevel_Mario::Togezo_WaveGame(_float _fTimeDelta)
{
	/* Togezo */
	if (m_bWave == false && GET_PLAYER_SMASH->Get_Transform()->Get_Position().m128_f32[0] > -52.f)
		m_bWave = true;

	if (m_bWave == true) {
		m_fCreTogezoTime += _fTimeDelta;

		if (m_fCreTogezoTime > 0.1f) {
			m_vecTogezo[m_iTogezoIndex]->OnEnter_Layer(nullptr);
			m_pGameInstance->Insert_GameObject(LEVEL_MARIO, L"Layer_Enemy", m_vecTogezo[m_iTogezoIndex]);
			Safe_AddRef(m_vecTogezo[m_iTogezoIndex++]);

			m_fCreTogezoTime = 0.f;

			if (m_iTogezoIndex >= 50)
				m_iTogezoIndex = 0;
		}
	}

	if (GET_PLAYER_SMASH->Get_Transform()->Get_Position().m128_f32[0] > -28.f)
		m_bCreTogezoEnd = true;
}

void CLevel_Mario::Start_EndCutScene()
{
	GET_PLAYER_SMASH->Change_State(_uint(SMASHTRAVIS_STATES::STATE_END_PORTAL), nullptr);
}

CLevel_Mario* CLevel_Mario::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Mario* pInstance = new CLevel_Mario(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Mario"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Mario::Free()
{
	__super::Free();

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif 

	Safe_Release(m_pTriggerManager);

	for (auto& pKiller : m_vecKiller)
		Safe_Release(pKiller);
	m_vecKiller.clear();

	for (auto& pGoomba : m_vecGoomba)
		Safe_Release(pGoomba);
	m_vecGoomba.clear();

	for (auto& pTogezo : m_vecTogezo)
		Safe_Release(pTogezo);
	m_vecTogezo.clear();
}

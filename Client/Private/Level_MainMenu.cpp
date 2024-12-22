#include "Level_MainMenu.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Travis_Manager.h"

#include "Monster_Manager.h"

#include "Levels_Header.h"

#include "UI_MainMenu.h"

CLevel_MainMenu::CLevel_MainMenu(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_MainMenu::Initialize()
{
	m_iLevelID = (_uint)LEVEL_MAINMENU;
	SET_CURLEVEL(m_iLevelID);

#ifdef _DEBUG

	if (FAILED(Ready_Camera()))
		return E_FAIL;

#endif // _DEBUG

	if (FAILED(Ready_Map()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_Fog()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	m_pGameInstance->Set_Active_Sky(true);
	// TODO : Add Additional Code Here.

	return S_OK;
}

void CLevel_MainMenu::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG

	if (KEY_DOWN(eKeyCode::Enter)) {

		INS_MAIN eCurState = static_cast<CUI_MainMenu*>(UIMGR->GetScene(L"SCENE_MainMenu"))->GetCurIdx();

		if (INS_MAIN_GAMESTART == eCurState)
		{
			PLAY_SOUND("MainMenu_Disappear", false, 0.5f);
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_HOME, m_pDevice, m_pContext));
			m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_HOME_LAB, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_UFO, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_MOTEL_FRONT, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY_TRASHCOLLECTOR, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_UFO_OUTSIDE, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY_TOILET, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_UFO_TOILET, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_UFO_INSIDE, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BOSS_BLACKHOLE_GROUND, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BOSS_BLACKHOLE_SPACE, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BOSS_SONICJUICE, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_SMASHBROS_BOSS_DAMON, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_ARROW_A_ROW, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_MARIO, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_ISLAND, m_pDevice, m_pContext));
			//m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_ENDING, m_pDevice, m_pContext));
		}

		else if (INS_MAIN_EXIT == eCurState)
			DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_MainMenu::Render()
{
	return S_OK;
}

void CLevel_MainMenu::OnExitLevel()
{
	STOP_SOUND("BGM - Intro");
	m_pGameInstance->Set_TimeScale(1.f);
	m_pGameInstance->Inactive_RadialBlur(0.f);
	m_pGameInstance->Set_Active_Fog(false);
	m_pGameInstance->Set_Active_Sky(false);
}

#ifdef _DEBUG

HRESULT CLevel_MainMenu::Ready_Camera()
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

HRESULT CLevel_MainMenu::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Level_MainMenu.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MainMenu::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);


	return S_OK;
}

HRESULT CLevel_MainMenu::Ready_Fog()
{
	FOG_DESCS FogDesc{};

	FogDesc.fFogDensity = 1.f;
	FogDesc.fFogDistStart = 400.f;
	FogDesc.fFogDistEnd = 600.f;
	XMStoreFloat4(&FogDesc.vFogColor, XMVectorSet(0.13f, 0.43f, 1.f, 1.f) * 1.1f);

	m_pGameInstance->Set_Active_Fog(true, &FogDesc);
	return S_OK;
}

HRESULT CLevel_MainMenu::Ready_UI()
{
	// 이후 현 레벨 아닌 다른 레벨 이동 가능성 있음
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_Intro", L"Prototype_UI_Intro_Logo"))
		return E_FAIL;
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_MainMenu", L"Prototype_UI_MainMenu"))
		return E_FAIL;

	return S_OK;
}

CLevel_MainMenu* CLevel_MainMenu::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_MainMenu* pInstance = new CLevel_MainMenu(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_MainMenu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MainMenu::Free()
{
	__super::Free();

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG

	CMapLoader::Destroy_Instance();
}

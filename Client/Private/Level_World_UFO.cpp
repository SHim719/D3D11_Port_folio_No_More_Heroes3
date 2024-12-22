#include "Level_World_UFO.h"

#include "GameInstance.h"
#include "MapLoader.h"
#include "Free_Camera.h"
#include "Travis_Manager.h"
#include "Trigger_Manager.h"
#include "Levels_Header.h"
#include "UI_Manager.h"
#include "UI_CCTV.h"

CLevel_World_UFO::CLevel_World_UFO(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_World_UFO::Initialize()
{
	m_iLevelID = (_uint)LEVEL_WORLD_UFO;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - UFO Passage", true, 0.5f);

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

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	// TODO : Add Additional Code Here.

	if (FAILED(Ready_UI()))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_NPC", L"Prototype_GameObject_NPC_Bugjiro"))
		return E_FAIL;

	Bind_Trigger_Functions();

	m_pGameInstance->Set_Active_Shadow(true);
	m_pGameInstance->Set_Active_SSR(true);

	return S_OK;
}

void CLevel_World_UFO::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG

	Check_Level_Change();
}

HRESULT CLevel_World_UFO::Render()
{
	return S_OK;
}
void CLevel_World_UFO::OnExitLevel()
{
	m_pGameInstance->Set_Active_SSR(false);
}
#ifdef _DEBUG


HRESULT CLevel_World_UFO::Ready_Camera()
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

HRESULT CLevel_World_UFO::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/World_UFO.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(CMapLoader::Get_Instance()->Load_Trigger(L"../../Resources/DataFiles/TriggerData/Trigger_World_UFO.dat", m_iLevelID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World_UFO::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);

	return S_OK;
}

HRESULT CLevel_World_UFO::Ready_Effects()
{
	return S_OK;
}

HRESULT CLevel_World_UFO::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Home_ButtonMove", L"Prototype_UI_MoveButton"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_CCTV", L"Prototype_UI_CCTV"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_SushiShop", L"Prototype_UI_SushiShop"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
		return E_FAIL;

	return S_OK;
}

void CLevel_World_UFO::Check_Level_Change()
{
	if (m_bGoLevelBattleUFOInside) {
		STOP_SOUND("BGM - UFO Passage");
		UIMGR->ChangeLevel();
		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_UFO_INSIDE, m_pDevice, m_pContext));
	}
	else if (m_bGoLevelBossBlackHoleGround) {
		STOP_SOUND("BGM - UFO Passage");
		UIMGR->ChangeLevel();
		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BOSS_BLACKHOLE_GROUND, m_pDevice, m_pContext));
	}
}

void CLevel_World_UFO::Bind_Trigger_Functions()
{
	/* 
	1. Trigger_Portal_WUFO_to_BUFOI
	2. Trigger_Portal_WUFO_to_BBHG
	3. Trigger_Interaction_WUFO_Sushi
	*/
	m_pTriggerManager->Bind_Func(L"Trigger_Portal_WUFO_to_BUFOI",
		bind(&CLevel_World_UFO::Go_Level_Battle_UFO_Inside, this));
	m_pTriggerManager->Bind_Func(L"Trigger_Portal_WUFO_to_BBHG",
		bind(&CLevel_World_UFO::Go_Level_Boss_BlackHole_Ground, this));
	m_pTriggerManager->Bind_Func(L"Trigger_Interaction_WUFO_Sushi",
		bind(&CLevel_World_UFO::Open_Wesn_UI, this, L"Trigger_Interaction_WUFO_Sushi"));
}

void CLevel_World_UFO::Open_Wesn_UI(wstring wstrTriggerName)
{
	UIMGR->SceneInit(L"SCENE_CCTV");
	static_cast<CUI_CCTV*>(UIMGR->GetScene(L"SCENE_CCTV"))->Set_UserType(wstrTriggerName);
}

CLevel_World_UFO* CLevel_World_UFO::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_World_UFO* pInstance = new CLevel_World_UFO(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_World_UFO"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_World_UFO::Free()
{
	__super::Free();
	CMapLoader::Destroy_Instance();

	Safe_Release(m_pTriggerManager);
#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG
}

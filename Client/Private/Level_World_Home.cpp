#include "Level_World_Home.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Travis_World.h"
#include "Trigger_Manager.h"
#include "Levels_Header.h"
#include "Travis_Manager.h"
#include "UI_Manager.h"

CLevel_World_Home::CLevel_World_Home(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_World_Home::Initialize()
{
	m_iLevelID = (_uint)LEVEL_WORLD_HOME;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - Motel", true, 1.f);

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

	if (FAILED(Ready_UI()))
		return E_FAIL;

	if (FAILED(Ready_NPC()))
		return E_FAIL;

	// TODO : Add Additional Code Here.
	Bind_Trigger_Functions();

	return S_OK;
}

void CLevel_World_Home::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	//if (KEY_DOWN(eKeyCode::L))
	//	UIMGR->ShowSubTitle(L"name");
	//if (KEY_DOWN(eKeyCode::P))
	//	UIMGR->ShowSubTitle(L"name2");

	//if (KEY_DOWN(eKeyCode::Zero))
	//	ADD_EVENT(bind(&CUI_Manager::NextSubTitle, Get_Inst(CUI_Manager)));

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

	if (KEY_DOWN(eKeyCode::F3)) {
		_vector vCurCamPos = m_pDebugFreeCamera->Get_Transform()->Get_Position();
		_vector vCurCamLook = m_pDebugFreeCamera->Get_Transform()->Get_Look();
		cout << "CamPos : " << vCurCamPos.m128_f32[0] << " " << vCurCamPos.m128_f32[1] << " " << vCurCamPos.m128_f32[2] << endl;
		cout << "CamLook : " << vCurCamLook.m128_f32[0] << " " << vCurCamLook.m128_f32[1] << " " << vCurCamLook.m128_f32[2] << endl;
	}

#endif // _DEBUG

	Check_Level_Change();
}

HRESULT CLevel_World_Home::Render()
{
	return S_OK;
}

#ifdef _DEBUG

HRESULT CLevel_World_Home::Ready_Camera()
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

HRESULT CLevel_World_Home::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/TravisHouse.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(CMapLoader::Get_Instance()->Load_Trigger(L"../../Resources/DataFiles/TriggerData/Trigger_TravisHouse.dat", m_iLevelID)))
		return E_FAIL;

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Default_Terrain", L"Prototype_Default_Terrain");

	return S_OK;
}

HRESULT CLevel_World_Home::Ready_Player()
{
	//PLAYER_INITIALIZE_DESC PlayerInitializeDesc = {};
	//PlayerInitializeDesc.strModelTag = L"Prototype_Model_Travis_World";
	//PlayerInitializeDesc.vInitialPos = _float4(3.5f, 0.f, 3.5f, 1.f);
	////XMStoreFloat4(&PlayerInitializeDesc.vInitialPos, m_pTriggerManager->Find_TriggerPos(L"Trigger_Portal_Home_to_Motel_Front"));
	//PlayerInitializeDesc.vInitialPos.y = 0.f;

	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);

	return S_OK;
}

HRESULT CLevel_World_Home::Ready_Effects()
{
	return S_OK;
}

HRESULT CLevel_World_Home::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_SubTitle", L"Prototype_UI_SubTitle"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_Home", L"Prototype_UI_Home"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Home_ButtonMove", L"Prototype_UI_MoveButton"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World_Home::Ready_NPC()
{
	if (!GAME_FLOW_CHECK(FLOW_Jean_dissapeared)) {
		if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Npc", L"Prototype_GameObject_NPC_Jeane"))
			return E_FAIL;
	}

	return S_OK;
}

void CLevel_World_Home::Check_Level_Change()
{
	if (m_bGo_Level_Battle_Motel_Front) {
		STOP_SOUND("BGM - Motel");
		UIMGR->ChangeLevel();
		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_MOTEL_FRONT, m_pDevice, m_pContext));
	}
	else if (m_bGo_Level_World_City) {
		STOP_SOUND("BGM - Motel");
		UIMGR->ChangeLevel();
		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext));
	}
	else if (m_bGo_Level_World_Home_Lab) {
		STOP_SOUND("BGM - Motel");
		UIMGR->ChangeLevel();
		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_HOME_LAB, m_pDevice, m_pContext));
	}
}

void CLevel_World_Home::Bind_Trigger_Functions()
{
	m_pTriggerManager->Bind_Func(L"Trigger_Portal_Home_to_Motel_Front",
		bind(&CLevel_World_Home::Go_Level_Battle_Motel_Front, this));
	m_pTriggerManager->Bind_Func(L"Trigger_Portal_Home_to_Motel_Front_2F",
		bind(&CLevel_World_Home::Go_Level_World_City, this));

	m_pTriggerManager->Bind_Func(L"Trigger_Portal_Home_to_Home_Lab",
		bind(&CLevel_World_Home::Go_Level_World_Home_Lab, this));

	//m_pTriggerManager->Bind_Func(L"Trigger_Interaction_Home_Closet",
	//	bind(&CLevel_World_Home::Go_Level_Battle_Motel_Front, this));

}

void CLevel_World_Home::Go_Level_Battle_Motel_Front()
{
	if (!GAME_FLOW_CHECK(FLOW_Battle_Motel_Front_Cleared))
		m_bGo_Level_Battle_Motel_Front = true;
	else {
		m_bGo_Level_World_City = true;

		PLAYER_INITIALIZE_DESC PD = {};
		PD.vInitialPos = _float4(3.80605674f, 0.268703014f, 12.5759706f, 1.00000000f);
		PD.vInitialLookAt = _float4(PD.vInitialPos.x - 0.999649525f, 0.f, PD.vInitialPos.z - 0.0264745969f, 1.f);
		PD.iNavIndex = 33;
		CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PD);
	}
}

void CLevel_World_Home::Go_Level_World_City()
{
	if (!GAME_FLOW_CHECK(FLOW_Battle_Motel_Front_Cleared))
		m_bGo_Level_Battle_Motel_Front = true;
	else {
		m_bGo_Level_World_City = true;

		PLAYER_INITIALIZE_DESC PD = {};
		PD.vInitialPos = _float4(4.08901596f, 3.60985613f, 12.6197834f, 1.00000000f);
		PD.vInitialLookAt = _float4(PD.vInitialPos.x - 0.0174989980f, 0.f, PD.vInitialPos.z - -0.999846935f, 1.f);
		PD.iNavIndex = 167;
		CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PD);
	}
}

void CLevel_World_Home::Go_Level_World_Home_Lab()
{
	m_bGo_Level_World_Home_Lab = true;

	PLAYER_INITIALIZE_DESC PD = {};
	PD.vInitialPos = _float4(0.5f, 1.0f, -8.5f, 1.f);
	PD.vInitialLookAt = _float4(0.f, 0.f, 1.f, 1.f);

	CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_HOME_LAB, PD);
}

CLevel_World_Home* CLevel_World_Home::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_World_Home* pInstance = new CLevel_World_Home(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_World_Home"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_World_Home::Free()
{
	__super::Free();

	Safe_Release(m_pTriggerManager);

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG

	CMapLoader::Destroy_Instance();
}

#include "Level_World_City.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Travis_Manager.h"
#include "NPC_Manager.h"
#include "Trigger_Manager.h"
#include "Levels_Header.h"
#include "UI_Manager.h"
#include "NPC_Quest.h"
#include "UI_CCTV.h"
#include "Travis_World.h"
#include "UI_World_City.h"


CLevel_World_City::CLevel_World_City(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_World_City::Initialize()
{
    m_iLevelID = (_uint)LEVEL_WORLD_CITY;
    SET_CURLEVEL(m_iLevelID);
    PLAY_SOUND("BGM - Santa Destroy 2021", true, 0.5f);

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

    if (FAILED(Ready_NPC()))
        return E_FAIL;

    if (FAILED(Ready_Effects()))
        return E_FAIL;

    // TODO : Add Additional Code Here.

    if (FAILED(Ready_UI()))
        return E_FAIL;

    // ALERT : Only For Test.
    //GAME_FLOW_SET(FLOW_First_PhoneCall_End, true);
    //GAME_FLOW_SET(FLOW_Quest_BattleIsland_Cleared, true);
    //GAME_FLOW_SET(FLOW_Quest_ToiletMinigame_Cleared, true);
    //GAME_FLOW_SET(FLOW_Quest_TrashCollector_Cleared, true);
    //GAME_FLOW_SET(FLOW_Silvia_Conversation_End, true);
    //GAME_FLOW_SET(FLOW_Boss_Blackhole_Cleared, true);
    //GAME_FLOW_SET(FLOW_Boss_SonicJuice_Cleared, true);

    if (GAME_FLOW_CHECK(FLOW_Quest_BattleIsland_Cleared) &&
        GAME_FLOW_CHECK(FLOW_Quest_ToiletMinigame_Cleared) &&
        GAME_FLOW_CHECK(FLOW_Quest_TrashCollector_Cleared) &&
        !GAME_FLOW_CHECK(FLOW_Silvia_Conversation_End)) {
        if (nullptr == m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_NPC", L"Prototype_GameObject_NPC_Silvia"))
            return E_FAIL;
    }

    NPC_MANAGER_INITIALIZER NPCManagerInitializer = {};
    NPCManagerInitializer.iLevelIndex = m_iLevelID;

    m_pNPC_Manager = CNPC_Manager::Create(m_pDevice, m_pContext, &NPCManagerInitializer);
    if (nullptr == m_pNPC_Manager)
        return E_FAIL;
    
    Bind_Trigger_Functions();

    m_pGameInstance->Set_Active_Shadow(true);
    m_pGameInstance->Set_Active_Sky(true);
    m_pGameInstance->Set_Active_Cascade(true);




    return S_OK;
}

void CLevel_World_City::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

    if (KEY_DOWN(eKeyCode::F2)) {
        m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
    }

#endif // _DEBUG

    m_pNPC_Manager->Manage_Wandering();

    if (!GAME_FLOW_CHECK(FLOW_First_PhoneCall_End)&&!m_bFlowControl) {
        if (!m_bPhoneCall)
        {
            PLAY_SOUND("SE_sys_TelCall", false, 1.f);

            m_bPhoneCall = true;
        }

        m_fTimeAcc += _fTimeDelta;

        if (m_fTimeAcc > 5.f) {
            STOP_SOUND("SE_sys_TelCall");

            GET_PLAYER_WORLD->Change_State((_uint)WORLDTRAVIS_STATES::STATE_EVE_PHONECALL);
            m_bFlowControl = true;
        }
    }

    Check_Level_Change();
}

HRESULT CLevel_World_City::Render()
{
    return S_OK;
}

void CLevel_World_City::OnExitLevel()
{
    STOP_SOUND("BGM - Santa Destroy 2021");
    STOP_SOUND("SE_enCar_drivin_01");

    m_pGameInstance->Set_Active_Sky(false);
    m_pGameInstance->Set_Active_Cascade(false);
}

#ifdef _DEBUG

HRESULT CLevel_World_City::Ready_Camera()
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

HRESULT CLevel_World_City::Ready_Map()
{
    if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/TravisMotel.dat", m_iLevelID, m_pDevice, m_pContext)))
        return E_FAIL;

    if (FAILED(CMapLoader::Get_Instance()->Load_Trigger(L"../../Resources/DataFiles/TriggerData/Trigger_MotelFront.dat", m_iLevelID)))
        return E_FAIL;

    CGameObject* pEntrance_R = { nullptr };
    CGameObject* pEntrance_L = { nullptr };
    
    pEntrance_R = m_pGameInstance->Add_Clone(LEVEL_WORLD_CITY, L"Layer_Environment", L"Prototype_Highway_Entrance");
    pEntrance_L = m_pGameInstance->Add_Clone(LEVEL_WORLD_CITY, L"Layer_Environment", L"Prototype_Highway_Entrance");


    pEntrance_R->Get_Transform()->Set_Position(XMVectorSet(28.831f, 4.f, -147.848f, 1.f));
    pEntrance_L->Get_Transform()->Set_Position(XMVectorSet(39.611f, 4.f, -147.848f, 1.f));
    return S_OK;
}

HRESULT CLevel_World_City::Ready_Player()
{
    CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);

    CGameObject* pTravisBike = m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Player", L"Prototype_Travis_Bike");
    if (nullptr == pTravisBike)
        assert(false);
    
    return S_OK;
}

HRESULT CLevel_World_City::Ready_NPC()
{
    //NPC_QUEST_DESC tNpcQuestDesc;
    //tNpcQuestDesc.t_Pos = { -16.4f, 0.1f, -59.1f, 1.f };
    //tNpcQuestDesc.t_Rotation = { -90.f };
    //tNpcQuestDesc.t_Npc = NPC_CLASS::NPC_B;

    ///* NPC - B : Toilet */
    //if (nullptr == m_pGameInstance->Add_Clone(LEVEL_WORLD_CITY, L"Layer_NPC", L"Prototype_GameObject_NPC_Quest", &tNpcQuestDesc))
    //    return E_FAIL;

    //tNpcQuestDesc.t_Pos = { -55.f, 0.2f, 104.8f, 1.f };
    //tNpcQuestDesc.t_Rotation = { -90.f };
    //tNpcQuestDesc.t_Npc = NPC_CLASS::NPC_C;

    ///* NPC - A : Benz */
    //if (nullptr == m_pGameInstance->Add_Clone(LEVEL_WORLD_CITY, L"Layer_NPC", L"Prototype_GameObject_NPC_Quest", &tNpcQuestDesc))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_World_City::Ready_Effects()
{
    return S_OK;
}

HRESULT CLevel_World_City::Ready_UI()
{
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_SubTitle", L"Prototype_UI_SubTitle"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_World_City", L"Prototype_UI_World_City"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_CCTV", L"Prototype_UI_CCTV"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_Notice", L"Prototype_UI_Notice"))
        return E_FAIL;

    return S_OK;
}

void CLevel_World_City::Check_Level_Change()
{
    if (m_bGoLevelBattelUFOOutside) {
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_UFO_OUTSIDE, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelWorldLabB1F) {
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_HOME_LAB, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelWorldTravisHouse1F) {
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_HOME, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelWorldTravisHouse2F) {
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_HOME, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelWorldToilet) {
        static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->SetVisible(UI_VISIBLE_FALSE);
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY_TOILET, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelBattleIsland) {
        static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->SetVisible(UI_VISIBLE_FALSE);
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_ISLAND, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelWorldTrashCollector) {
        static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->SetVisible(UI_VISIBLE_FALSE);
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY_TRASHCOLLECTOR, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelArrowARow) {
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_ARROW_A_ROW, m_pDevice, m_pContext));
    }
    else if (m_bGoLevelMario) {
        UIMGR->ChangeLevel();
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_MARIO, m_pDevice, m_pContext));
    }
}

void CLevel_World_City::Bind_Trigger_Functions()
{
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_MF_to_BUFOO",
      bind(&CLevel_World_City::Go_Level_Battle_UFO_Outside, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_MF_to_TH_1F",
          bind(&CLevel_World_City::Go_Level_World_TravisHouse_1F, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_MF_to_TH_2F",
          bind(&CLevel_World_City::Go_Level_World_TravisHouse_2F, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_MF_to_Lab_B1F",
          bind(&CLevel_World_City::Go_Level_World_Lab_B1F, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_WC_to_TR",
          bind(&CLevel_World_City::Go_Level_World_City_Toilet, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_WC_to_BI",
          bind(&CLevel_World_City::Go_Level_Battle_Island, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_WC_to_TC",
          bind(&CLevel_World_City::Go_Level_Wolrd_TrashCollector, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_WC_to_AAR",
          bind(&CLevel_World_City::Go_Level_Arrow_A_Row, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Portal_WC_to_Mario",
          bind(&CLevel_World_City::Go_Level_Mario, this));
      m_pTriggerManager->Bind_Func(L"Trigger_Talk_QuestNPC_Toilet",
          bind(&CLevel_World_City::Activate_Interaction_With_ToiletNPC, this, L"Trigger_Talk_QuestNPC_Toilet"));
      m_pTriggerManager->Bind_Func(L"Trigger_Talk_QuestNPC_BI",
          bind(&CLevel_World_City::Activate_Interaction_With_BattleQuestNPC, this, L"Trigger_Talk_QuestNPC_BI"));
      m_pTriggerManager->Bind_Func(L"Trigger_Talk_QuestNPC_TC",
          bind(&CLevel_World_City::Activate_Interaction_With_TrashQuestNPC, this, L"Trigger_Talk_QuestNPC_TC"));
}

void CLevel_World_City::Go_Level_World_Lab_B1F()
{
    m_bGoLevelWorldLabB1F = true;

    PLAYER_INITIALIZE_DESC PD = {};
    PD.vInitialPos = _float4(5.85460377f, 3.79853821f, 14.2597523f, 1.00000000f);
    PD.vInitialLookAt = _float4(PD.vInitialPos.x - 0.967474401f, 0.f, PD.vInitialPos.z + 0.252968967f, 1.f);
    PD.iNavIndex = 1;

    CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_HOME_LAB, PD);
}

void CLevel_World_City::Go_Level_World_TravisHouse_1F()
{
    m_bGoLevelWorldTravisHouse1F = true;

    PLAYER_INITIALIZE_DESC PD = {};
    PD.vInitialPos = _float4(3.05278945f, 0.0105142230f, 3.27268529f, 1.00000000f);
    PD.vInitialLookAt = _float4(PD.vInitialPos.x + 0.445690125f, 0.f, PD.vInitialPos.z + 0.895187318f, 1.f);
    PD.iNavIndex = 1;
    CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_HOME, PD);
}

void CLevel_World_City::Go_Level_World_TravisHouse_2F()
{
    m_bGoLevelWorldTravisHouse2F = true;

    PLAYER_INITIALIZE_DESC PD = {};
    PD.vInitialPos = _float4(3.10713720f, 3.83337212f, 3.66532850f, 1.00000000f);
    PD.vInitialLookAt = _float4(PD.vInitialPos.x + 0.00647723628f, 0.f, PD.vInitialPos.z + 0.999979019f, 1.f);
    PD.iNavIndex = 126;
    CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_HOME, PD);
}

void CLevel_World_City::Activate_Interaction_With_ToiletNPC(wstring wstrTriggerName)
{
    static_cast<CNPC_Quest*>(m_pNPC_Manager->Get_QuestNPC((_uint)CNPC_Manager::QUEST_NPC::QUEST_TOILET))->Activate_Interaction();

    UIMGR->SceneInit(L"SCENE_CCTV");
    static_cast<CUI_CCTV*>(UIMGR->GetScene(L"SCENE_CCTV"))->Set_UserType(wstrTriggerName);
}

void CLevel_World_City::Activate_Interaction_With_BattleQuestNPC(wstring wstrTriggerName)
{
    static_cast<CNPC_Quest*>(m_pNPC_Manager->Get_QuestNPC((_uint)CNPC_Manager::QUEST_NPC::QUEST_BATTLE_ISLAND))->Activate_Interaction();

    UIMGR->SceneInit(L"SCENE_CCTV");
    static_cast<CUI_CCTV*>(UIMGR->GetScene(L"SCENE_CCTV"))->Set_UserType(wstrTriggerName);
}

void CLevel_World_City::Activate_Interaction_With_TrashQuestNPC(wstring wstrTriggerName)
{
    static_cast<CNPC_Quest*>(m_pNPC_Manager->Get_QuestNPC((_uint)CNPC_Manager::QUEST_NPC::QUEST_TRASHCOLLECTOR))->Activate_Interaction();

    UIMGR->SceneInit(L"SCENE_CCTV");
    static_cast<CUI_CCTV*>(UIMGR->GetScene(L"SCENE_CCTV"))->Set_UserType(wstrTriggerName);
}

CLevel_World_City* CLevel_World_City::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_World_City* pInstance = new CLevel_World_City(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CLevel_World_City"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_World_City::Free()
{
    __super::Free();

    Safe_Release(m_pNPC_Manager);
    Safe_Release(m_pTriggerManager);

#ifdef _DEBUG

    Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG

    CMapLoader::Destroy_Instance();
}

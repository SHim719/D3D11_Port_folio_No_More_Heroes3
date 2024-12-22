#pragma once


#define KEY_DOWN(KEY) m_pGameInstance->GetKeyDown(KEY)
#define KEY_PUSHING(KEY) m_pGameInstance->GetKey(KEY)
#define KEY_UP(KEY) m_pGameInstance->GetKeyUp(KEY)
#define KEY_NONE(KEY) m_pGameInstance->GetKeyNone(KEY)


#define GET_CURLEVEL m_pGameInstance->Get_CurrentLevelID()
#define SET_CURLEVEL(LEVEL) m_pGameInstance->Set_CurrentLevelID(LEVEL)
#define ADD_EVENT(FUNC) m_pGameInstance->Add_Event(FUNC)
#define GET_PLAYER			static_cast<CTravis*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Layer_Player"))
#define GET_PLAYER_SPACE	static_cast<CTravis_Space*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Layer_Player"))
#define GET_PLAYER_WORLD	static_cast<CTravis_World*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Layer_Player"))
#define GET_PLAYER_SMASH	static_cast<CSmashBros_Travis*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Layer_Player"))
#define GET_SONICJUICE		static_cast<CSonicJuice*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Layer_Enemy"))
#define UIMGR_TURNRPG		static_cast<CUI_TurnRPG*>(UIMGR->GetScene(L"SCENE_TurnRPG"))
#define GET_CAMERA m_pGameInstance->Get_MainCamera()
#define PLAY_SOUND(SOUND, LOOP, VOLUME) m_pGameInstance->Play(SOUND, LOOP, VOLUME)
#define STOP_SOUND(SOUND) m_pGameInstance->Stop(SOUND)

#define GET_LAYER(LEVEL, str) m_pGameInstance->Find_GameObject(LEVEL, str)

#define Get_Inst(TYPE) TYPE::Get_Instance()

#define UIMGR Get_Inst(CUI_Manager)
#define CUTSCENEMGR Get_Inst(CCutscene_Manager)
#define EFFECTMGR Get_Inst(CEffect_Manager)
#define MONSTERMGR Get_Inst(CMonster_Manager)
#define TURNMGR Get_Inst(CTurn_Manager)
#define AWMGR Get_Inst(CAW_Manager)
#define CINEMAMGR Get_Inst(CCinematic_Manager)

#define CALC_TF m_pGameInstance->Get_CalculateTF()


#define YAXIS XMVectorSet(0.f, 1.f, 0.f, 0.f)
#define ZAXIS XMVectorSet(0.f, 0.f, 1.f, 0.f)

#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!

#define LOCK_PLAYER_KEY_IO static_cast<CTravis_World*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Layer_Player"))->Set_ControlLock(true);
#define UNLOCK_PLAYER_KEY_IO static_cast<CTravis_World*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Layer_Player"))->Set_ControlLock(false);
 
#define GAME_FLOW_CHECK(index) CTravis_Manager::Get_Instance()->Get_FlowCheck(index)
#define GAME_FLOW_SET(index, isclear) CTravis_Manager::Get_Instance()->Set_FlowCheck(index, isclear)
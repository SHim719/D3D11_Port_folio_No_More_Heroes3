#include "Level_Boss_SonicJuice.h"

#include "GameInstance.h"

#include "Free_Camera.h"
#include "MapLoader.h"
#include "Travis_Manager.h"
#include "Travis_Basic.h"

#include "Levels_Header.h"

#include "Turn_Manager.h"

#include "Cinematic_Manager.h"
#include "Cinema_Travis_Bike.h"
#include "Cinema_Travis_World.h"

#include "UI_Manager.h"

_bool CLevel_Boss_SonicJuice::IS_BOSS_DEAD = { false };

CLevel_Boss_SonicJuice::CLevel_Boss_SonicJuice(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Boss_SonicJuice::Initialize()
{
    PLAY_SOUND("BGM - Sonic Juice JRPG Battle", true, 0.5f);
    m_iLevelID = (_uint)LEVEL_BOSS_SONICJUICE;
    SET_CURLEVEL(m_iLevelID);

#ifdef _DEBUG
    if (FAILED(Ready_Camera()))
        return E_FAIL;
#endif // _DEBUG

    if (FAILED(Ready_Map()))
        return E_FAIL;

    if (FAILED(Ready_Monster()))
        return E_FAIL;

    if (FAILED(Ready_UI()))
        return E_FAIL;

    m_pGameInstance->Set_Render_SonicJuice(true);

  // Start_Game();

    if (FAILED(Ready_Cinematic()))
        return E_FAIL;
    
    CINEMAMGR->Start_Cinematic("SonicJuice_Cinematic");
    
    CINEMAMGR->Bind_CinemaEndFunc(
        [&]()->void {
            UIMGR->ShowSubTitle_Timer(L"SonicJuice_Intro", 0.f, 18.f / 6.f);
            UIMGR->Insert_ExitFunc(
                [&]()->void {
                    CFadeScreen::FADEDESC FD = {};
                    FD.fFadeOutSpeed = 1.f;
                    FD.fExtraTime = 1.f;
                    FD.fFadeInSpeed = 1.f;
                    FD.pCallback_FadeOutEnd = [&]()->void {CINEMAMGR->Clear_CinemaObjects(); };
                    FD.pCallback_FadeInStart = [&]()->void {Start_Game(); };
    
                    CGameObject* pFadeScreen = m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_FadeScreen");
                    pFadeScreen->OnEnter_Layer(&FD);
                    m_pGameInstance->Insert_GameObject(m_iLevelID, L"Layer_UI", pFadeScreen);
                }
            );
        }
    );
    
    m_pGameInstance->Set_Active_Sky(true);
    

    return S_OK;
}

void CLevel_Boss_SonicJuice::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (KEY_DOWN(eKeyCode::F2)) {
        m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
    }
#endif // _DEBUG

    __super::Tick(fTimeDelta);

    // TODO : Add Additional Code Here.
    TURNMGR->Tick(fTimeDelta);

    if (IS_BOSS_DEAD && !m_bLevelCleard) {
        m_fTimeAcc_FinalDelay += fTimeDelta;

        if (m_fTimeAcc_FinalDelay > 0.25f) {
            //m_pGameInstance->Set_Active_FinalSlash_RedTone(true);

            RADIALBLUR_DESCS RD = {};
            RD.fBlurRadius = 10.f;
            RD.fBlurStrength = 10.f;
            RD.pActor = GET_PLAYER;
            m_pGameInstance->Active_RadialBlur(RD);

            m_pGameInstance->Set_TimeScaleWithRealTime(0.001f, 7.f);
            m_bLevelCleard = true;
            UIMGR->CommonBattleEnd(INS_LETTER_JUICE);
        }
    }

    if (m_bLevelCleard) {
        m_fTimeAcc_LevelClear += fTimeDelta;
        if (m_fTimeAcc_LevelClear > 5.5f)
            UIMGR->CommonBattleEndLetterDisappear(INS_LETTER_JUICE);

        if (m_fTimeAcc_LevelClear > 7.f) {
            if (!GAME_FLOW_CHECK(FLOW_Boss_SonicJuice_Cleared)) {
                PLAYER_INITIALIZE_DESC PD = {};
                PD.vInitialPos = _float4(-13.f, 0.3f, -1.f, 1.f);
                PD.vInitialLookAt = _float4(-13.f, 0.3f, 0.f, 1.f);
                CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PD);

                GAME_FLOW_SET(FLOW_Boss_SonicJuice_Cleared, true);

                m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext));
            }
        }
    }
}

HRESULT CLevel_Boss_SonicJuice::Render()
{
    return S_OK;
}

void CLevel_Boss_SonicJuice::OnExitLevel()
{
    STOP_SOUND("BGM - Sonic Juice Boss Battle");

    // TODO : Add Additional Code Here.
    m_pGameInstance->Set_Active_Sky(false);
    m_pGameInstance->Set_Active_SSR(false);
    m_pGameInstance->Inactive_RadialBlur(0.1f);
    m_pGameInstance->Set_TimeScale(1.f);

    m_pGameInstance->Set_Render_SonicJuice(false);
}

HRESULT CLevel_Boss_SonicJuice::Start_Game()
{
    if (FAILED(Ready_Player()))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Player"))
        return E_FAIL;
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_TurnRPGScene", L"Prototype_UI_TurnRPG"))
        return E_FAIL;

    if (FAILED(TURNMGR->Initialize()))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG

HRESULT CLevel_Boss_SonicJuice::Ready_Camera()
{
    CCamera::CAMERADESC camDesc{};
    camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    camDesc.fNear   = 0.1f;
    camDesc.fFar    = 1000.f;
    camDesc.fFovy   = 60.f;
    camDesc.vAt     = { 0.f, 0.f, 1.f, 1.f };
    camDesc.vEye    = { 0.f, 2.f, -2.f, 1.f };

    m_pDebugFreeCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));
    if (nullptr == m_pDebugFreeCamera)
        return E_FAIL;

    return S_OK;
}

#endif // _DEBUG

HRESULT CLevel_Boss_SonicJuice::Ready_Map()
{
    if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Level_SonicJuice.dat", m_iLevelID, m_pDevice, m_pContext)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss_SonicJuice::Ready_Player()
{
    CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_BATTLE_GROUND);

    return S_OK;
}

HRESULT CLevel_Boss_SonicJuice::Ready_Monster()
{
    /* Boss_SonicJuice */
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Enemy", L"Prototype_GameObject_SonicJuice"))
        return E_FAIL;

    /* Fish */
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Etc", L"Prototype_GameObject_Fish"))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss_SonicJuice::Ready_UI()
{
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_SubTitle", L"Prototype_UI_SubTitle"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle", L"Prototype_UI_InBattle_Letter"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Named"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Mechanic_Battle", L"Prototype_UI_InBattle_Mechanic"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_Battle_Result"))
        return E_FAIL;



    return S_OK;
}

HRESULT CLevel_Boss_SonicJuice::Ready_Cinematic()
{
    CINEMAMGR->Clear_CinemaObjects();

    CCinema_Travis_World* pCinemaTravis = static_cast<CCinema_Travis_World*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Travis_World"));
    if (nullptr == pCinemaTravis)
        return E_FAIL;

    CCinema_Travis_Bike* pCinemaBike = static_cast<CCinema_Travis_Bike*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Travis_Bike"));
    if (nullptr == pCinemaBike)
        return E_FAIL;

    pCinemaBike->Set_CinemaTravis(pCinemaTravis);

    CINEMAMGR->Bind_DefaultEvents();


    return S_OK;
}

CLevel_Boss_SonicJuice* CLevel_Boss_SonicJuice::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_Boss_SonicJuice* pInstance = new CLevel_Boss_SonicJuice(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CLevel_Boss_SonicJuice"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Boss_SonicJuice::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pDebugFreeCamera);
#endif // _DEBUG

    CTurn_Manager::Destroy_Instance();
}

#include "Level_World_City_TrashCollector.h"

#include "GameInstance.h"

#include "Travis_Manager.h"
#include "Travis_World.h"
#include "Free_Camera.h"

#include "TrashCollectorObject.h"
#include "UI_MiniGame_Trash.h"
#include "Levels_Header.h"
#include "UI_Manager.h"

CLevel_World_City_TrashCollector::CLevel_World_City_TrashCollector(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_World_City_TrashCollector::Initialize()
{
    PLAY_SOUND("BGM - Garbage Collecting", true, 0.5f);

    m_iLevelID = (_uint)LEVEL_WORLD_CITY_TRASHCOLLECTOR;
    SET_CURLEVEL(m_iLevelID);

#ifdef _DEBUG

    if (FAILED(Ready_Camera()))
        return E_FAIL;

#endif // _DEBUG
    if (FAILED(Ready_Map()))
        return E_FAIL;

    if (FAILED(Ready_Player()))
        return E_FAIL;

    //if (FAILED(Ready_Monster()))
    //    return E_FAIL;

    //if (FAILED(Ready_Effects()))
    //    return E_FAIL;

    PLAYER_TRASHCOLLECTOR_INFO* pPTI = CTravis_Manager::Get_Instance()->Get_TrashCollectorInfo();
    pPTI->SetUp_Timer(10.f);

    if (FAILED(Ready_UI()))
        return E_FAIL;

    for (_uint i = 0; i < 50; ++i) {
        m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Enemy", L"Prototype_GameObject_TrashCollectorObject");
    }

    for (_uint i = 0; i < 10; ++i) {
        m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Enemy", L"Prototype_GameObject_Croc");
    }
    m_pGameInstance->Set_Active_Sky(true);



    return S_OK;
}

void CLevel_World_City_TrashCollector::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

    if (KEY_DOWN(eKeyCode::F2)) {
        m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
    }

#endif // _DEBUG

    PLAYER_TRASHCOLLECTOR_INFO* pPTI = CTravis_Manager::Get_Instance()->Get_TrashCollectorInfo();

    if (!m_bStart) {
        m_fTimeAcc_StartTimer -= _fTimeDelta;
        if (0.f > m_fTimeAcc_StartTimer) {
            pPTI->bStart = true;
            m_bStart = true;
        }
    }

    pPTI->Update_Info(_fTimeDelta * m_pGameInstance->Get_TimeScale());

    if (GAME_FLOW_CHECK(FLOW_Quest_TrashCollector_Cleared)&&!m_bLevelCleard) {
        CFadeScreen::FADEDESC FD = {};
        FD.eFadeColor = CFadeScreen::BLACK;
        FD.fExtraTime = 0.f;
        FD.fFadeInSpeed = 1.f;
        FD.fFadeOutSpeed = 1.f;
        FD.pCallback_FadeOutEnd = [&]()->void {
            PLAYER_INITIALIZE_DESC PD = {};
            PD.vInitialPos = _float4(-59.5026894f, 0.235670030f, 105.212692f, 1.00000000f);
            PD.vInitialLookAt = _float4(PD.vInitialPos.x - 1.f, 0.f, PD.vInitialPos.z, 1.f);
            PD.iPrevLevel = LEVEL_WORLD_CITY_TRASHCOLLECTOR;
            CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PD);

            ADD_EVENT(bind(&CGameInstance::Change_Level, m_pGameInstance, CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext)));
            };

        GET_PLAYER_WORLD->Apply_FadeIO(FD);
        m_bLevelCleard = true;
    }
}

HRESULT CLevel_World_City_TrashCollector::Render()
{
    return S_OK;
}

void CLevel_World_City_TrashCollector::OnExitLevel()
{
    STOP_SOUND("BGM - Garbage Collecting");

    UIMGR->ChangeLevel();

    m_pGameInstance->Set_Active_Sky(false);
    m_pGameInstance->Inactive_RadialBlur(0.1f);
    m_pGameInstance->Set_TimeScale(1.f);
}

#ifdef _DEBUG

HRESULT CLevel_World_City_TrashCollector::Ready_Camera()
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

HRESULT CLevel_World_City_TrashCollector::Ready_Map()
{
    if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Garbage_Collecting.dat", m_iLevelID, m_pDevice, m_pContext)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_World_City_TrashCollector::Ready_Player()
{
    CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);
    *(CTravis_Manager::Get_Instance()->Get_TrashCollectorInfo()) = {};

    return S_OK;
}

HRESULT CLevel_World_City_TrashCollector::Ready_UI()
{
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_MiniGame_Trash", L"Prototype_UI_MiniGame_Trash"))
        return E_FAIL;

    return S_OK;
}

CLevel_World_City_TrashCollector* CLevel_World_City_TrashCollector::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_World_City_TrashCollector* pInstance = new CLevel_World_City_TrashCollector(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CLevel_World_City_TrashCollector"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_World_City_TrashCollector::Free()
{
    __super::Free();

#ifdef _DEBUG

    Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG
}

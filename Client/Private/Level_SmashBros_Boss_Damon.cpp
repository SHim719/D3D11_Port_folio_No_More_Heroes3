#include "Level_SmashBros_Boss_Damon.h"

#include "GameInstance.h"

#include "Free_Camera.h"
#include "MapLoader.h"
#include "Travis_Manager.h"

#include "SmashBros_Terrain.h"
#include "SmashBros_Wall.h"
#include "SmashBros_Damon.h"

_bool CLevel_SmashBros_Boss_Damon::s_bLevelCleared = { false };

CLevel_SmashBros_Boss_Damon::CLevel_SmashBros_Boss_Damon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CClientLevel{ _pDevice,_pContext } 
{
}

HRESULT CLevel_SmashBros_Boss_Damon::Initialize()
{
    PLAY_SOUND("BGM - Damon Boss Battle", true, 0.5f);

    m_iLevelID = (_uint)LEVEL_SMASHBROS_BOSS_DAMON;
    SET_CURLEVEL(m_iLevelID);

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
    
    CSmashBros_Wall::SMASHBROS_WALL_DESC WallDesc = {};
    WallDesc.vInitialPos = { -9.2f, 2.f ,0.f,1.f };
    m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_SmashBros_Physicals", L"Prototype_SmashBros_Wall", &WallDesc);
    WallDesc.vInitialPos = { 21.4f, 2.f ,0.f,1.f };
    m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_SmashBros_Physicals", L"Prototype_SmashBros_Wall", &WallDesc);

    m_pGameInstance->Set_Active_Sky(true);

    return S_OK;
}

void CLevel_SmashBros_Boss_Damon::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (KEY_DOWN(eKeyCode::F2)) {
        m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
    }
#endif // _DEBUG

    // TODO : Add Additional Code Here.

}


HRESULT CLevel_SmashBros_Boss_Damon::Render()
{
    return S_OK;
}

void CLevel_SmashBros_Boss_Damon::OnExitLevel()
{
    // TODO : Add Additional Code Here.
    m_pGameInstance->Set_Active_Sky(false);
    m_pGameInstance->Inactive_RadialBlur(0.1f);
    m_pGameInstance->Set_TimeScale(1.f);
}

#ifdef _DEBUG

HRESULT CLevel_SmashBros_Boss_Damon::Ready_Camera()
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

HRESULT CLevel_SmashBros_Boss_Damon::Ready_Map()
{
    if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Damon_BossMap.dat", m_iLevelID, m_pDevice, m_pContext)))
        return E_FAIL;

    if (FAILED(CMapLoader::Get_Instance()->Load_Terrain(L"../../Resources/DataFiles/TriggerData/PlatformColliders_DamonBoss.dat", m_iLevelID)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SmashBros_Boss_Damon::Ready_Player()
{
    CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::SMASHBROS_TRAVIS);

    return S_OK;
}

HRESULT CLevel_SmashBros_Boss_Damon::Ready_Monster()
{
    CSmashBros_Damon* pDamon = static_cast<CSmashBros_Damon*>(m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Enemy", L"Prototype_SmashBros_Damon"));
    if (nullptr == pDamon)
        return E_FAIL;
    pDamon->OnEnter_Layer(nullptr);

    return S_OK;
}

HRESULT CLevel_SmashBros_Boss_Damon::Ready_UI()
{
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_SubTitle", L"Prototype_UI_SubTitle"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_SmashScene", L"Prototype_UI_Smash"))
        return E_FAIL;

    return S_OK;
}

CLevel_SmashBros_Boss_Damon* CLevel_SmashBros_Boss_Damon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_SmashBros_Boss_Damon* pInstance = new CLevel_SmashBros_Boss_Damon(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CLevel_SmashBros_Boss_Damon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_SmashBros_Boss_Damon::Free()
{
    __super::Free();

#ifdef _DEBUG

    Safe_Release(m_pDebugFreeCamera);

#endif 
}

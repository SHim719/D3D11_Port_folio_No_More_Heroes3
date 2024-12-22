#include "Level_Arrow_A_Row.h"

#include "GameInstance.h"

#include "Free_Camera.h"
#include "MapLoader.h"
#include "Travis_Manager.h"

#include "Bike_AW.h"

#include "AW_Manager.h"
#include "Cinematic_Manager.h"

#include "Light.h"
#include "FadeScreen.h"
#include "Level_Loading.h"

#include "Various_Camera.h"


CLevel_Arrow_A_Row::CLevel_Arrow_A_Row(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Arrow_A_Row::Initialize()
{
    PLAY_SOUND("BGM - Arms of Immortal DAEMON X MACHINA", true, 0.5f);

    m_iLevelID = (_uint)LEVEL_ARROW_A_ROW;
    SET_CURLEVEL(m_iLevelID);

#ifdef _DEBUG
    if (FAILED(Ready_Camera()))
        return E_FAIL;
#endif // _DEBUG

    if (FAILED(Ready_Light()))
        return E_FAIL;

    if (FAILED(AWMGR->Initialize()))
        return E_FAIL;

    
    m_pGameInstance->Set_Active_Sky(true);
    m_pGameInstance->Set_Active_Shadow(true);
    m_pGameInstance->Set_Active_Cascade(true);

    CINEMAMGR->Bind_DefaultEvents();
  
    CFadeScreen::FADEDESC FadeDesc{};
    FadeDesc.eFadeColor = CFadeScreen::BLACK;
    FadeDesc.fFadeOutSpeed = 1.f;
    FadeDesc.fFadeInSpeed = 5.f;
    FadeDesc.pCallback_FadeOutEnd = bind(&CGameInstance::Change_Level, m_pGameInstance, CLevel_Loading::Create(LEVEL_BOSS_SONICJUICE, m_pDevice, m_pContext));
  
    CGameObject* pFadeScreen = m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_FadeScreen");
    pFadeScreen->OnEnter_Layer(&FadeDesc);
  
    CINE_EVENT CineEvent;
    CineEvent.eEventType = ETC;
    CineEvent.pFunc = bind(&CGameInstance::Insert_GameObject, m_pGameInstance, LEVEL_STATIC, L"Layer_UI", pFadeScreen);
  
    CINEMAMGR->Bind_CamEvent("SonicJuice_Enter", 6.f, CineEvent);

    CINEMAMGR->Start_Cinematic("Arrow_Level_Enter");
   
    return S_OK;
}

void CLevel_Arrow_A_Row::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    if (KEY_DOWN(eKeyCode::F2)) {
        m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
    }
#endif // _DEBUG

    __super::Tick(fTimeDelta);

    AWMGR->Tick(fTimeDelta);
}

HRESULT CLevel_Arrow_A_Row::Render()
{
    return S_OK;
}

void CLevel_Arrow_A_Row::OnExitLevel()
{
    STOP_SOUND("BGM - Arms of Immortal DAEMON X MACHINA");

    m_pGameInstance->Set_Active_Sky(false);
    m_pGameInstance->Set_Active_Cascade(false);

    m_pGameInstance->Inactive_RadialBlur(0.1f);
    m_pGameInstance->Set_TimeScale(1.f);
}

#ifdef _DEBUG

HRESULT CLevel_Arrow_A_Row::Ready_Camera()
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

HRESULT CLevel_Arrow_A_Row::Ready_Map()
{
    if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Level_SonicJuice.dat", m_iLevelID, m_pDevice, m_pContext)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_Arrow_A_Row::Ready_Light()
{
    LIGHT_DESC LightDesc{};
    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
    LightDesc.fLightStrength = 1.f;

    m_pGameInstance->Set_DirectionalLight(LightDesc);

    return S_OK;
}


CLevel_Arrow_A_Row* CLevel_Arrow_A_Row::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_Arrow_A_Row* pInstance = new CLevel_Arrow_A_Row(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CLevel_Arrow_A_Row"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Arrow_A_Row::Free()
{
    __super::Free();

    CAW_Manager::Destroy_Instance();

#ifdef _DEBUG

    //CTurn_Manager::Destroy_Instance();
    Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG
}

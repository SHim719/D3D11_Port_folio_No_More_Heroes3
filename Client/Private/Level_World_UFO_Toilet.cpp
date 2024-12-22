#include "Level_World_UFO_Toilet.h"

#include "GameInstance.h"

#include "Free_Camera.h"
#include "Travis_Manager.h"
#include "Trigger_Manager.h"

CLevel_World_UFO_Toilet::CLevel_World_UFO_Toilet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_World_UFO_Toilet::Initialize()
{
    m_iLevelID = (_uint)LEVEL_WORLD_UFO_TOILET;
    SET_CURLEVEL(m_iLevelID);
    STOP_SOUND("BGM - UFO Passage");

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

    // TODO : Add Additional Code Here.

    return S_OK;
}

void CLevel_World_UFO_Toilet::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

    if (KEY_DOWN(eKeyCode::F2)) {
        m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
    }

#endif // _DEBUG



}

HRESULT CLevel_World_UFO_Toilet::Render()
{
    return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_World_UFO_Toilet::Ready_Camera()
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

HRESULT CLevel_World_UFO_Toilet::Ready_Map()
{
    CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Toilet_Repair.dat", m_iLevelID, m_pDevice, m_pContext);
 
    return S_OK;
}

HRESULT CLevel_World_UFO_Toilet::Ready_Player()
{
    CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);

    return S_OK;
}

HRESULT CLevel_World_UFO_Toilet::Ready_Effects()
{
    return S_OK;
}

HRESULT CLevel_World_UFO_Toilet::Ready_UI()
{
    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_CheckPoint", L"Prototype_UI_ToiletCheckPoint"))
        return E_FAIL;

    if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
        return E_FAIL;

    return S_OK;
}

void CLevel_World_UFO_Toilet::Check_Level_Change()
{
    /*if (m_bGo_Level_Battle_Motel_Front) {
        m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BATTLE_MOTEL_FRONT, m_pDevice, m_pContext));
    }*/
}

void CLevel_World_UFO_Toilet::Bind_Trigger_Functions()
{
    /*  m_pTriggerManager->Bind_Func(L"Trigger_Portal_Home_to_Motel_Front",
      bind(&CLevel_World_Home::Go_Level_Battle_Motel_Front, this));*/
}

CLevel_World_UFO_Toilet* CLevel_World_UFO_Toilet::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_World_UFO_Toilet* pInstance = new CLevel_World_UFO_Toilet(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CLevel_World_UFO_Toilet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_World_UFO_Toilet::Free()
{
    __super::Free();

    Safe_Release(m_pTriggerManager);
#ifdef _DEBUG

    Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG


}

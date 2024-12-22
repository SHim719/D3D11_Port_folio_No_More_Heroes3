#include "Level_World_Home_Lab.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Travis_World.h"
#include "Travis_Manager.h"
#include "Effect_Manager.h"
#include "Trigger_Manager.h"
#include "UI_Manager.h"
#include "UI_CCTV.h"
#include "Levels_Header.h"

_bool CLevel_World_Home_Lab::s_bFirstEnterWesn = { true };

CLevel_World_Home_Lab::CLevel_World_Home_Lab(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_World_Home_Lab::Initialize()
{
	m_iLevelID = (_uint)LEVEL_WORLD_HOME_LAB;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - Naomis Lab", true, 0.5f);

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

	if (FAILED(Ready_UI()))
		return E_FAIL;


	// TODO : Add Additional Code Here.
	Bind_Trigger_Functions();

	m_pGameInstance->Set_Active_SSR(true);

	return S_OK;
}

void CLevel_World_Home_Lab::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG


	if (-1 == m_iEffectIndex)
	{
		EFFECTMGR->Active_Effect(LEVEL_WORLD_HOME_LAB, "Wrist_Cutter", nullptr, &m_iEffectIndex);
	}

	Check_Level_Change();
}

HRESULT CLevel_World_Home_Lab::Render()
{
	return S_OK;
}

void CLevel_World_Home_Lab::OnExitLevel()
{
	STOP_SOUND("BGM - Naomis Lab");
	m_pGameInstance->Set_Active_SSR(false);
}


#ifdef _DEBUG
HRESULT CLevel_World_Home_Lab::Ready_Camera()
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

HRESULT CLevel_World_Home_Lab::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/TravisHouseB1_NaomiLab.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(CMapLoader::Get_Instance()->Load_Trigger(L"../../Resources/DataFiles/TriggerData/Trigger_Lab.dat", m_iLevelID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World_Home_Lab::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);

	return S_OK;
}

HRESULT CLevel_World_Home_Lab::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Home_ButtonMove", L"Prototype_UI_MoveButton"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PowerUp"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_CCTV", L"Prototype_UI_CCTV"))
		return E_FAIL;

	return S_OK;
}

void CLevel_World_Home_Lab::Check_Level_Change()
{
	if (m_bGoLevelWorldCity) {
		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext));
	}
}

void CLevel_World_Home_Lab::Bind_Trigger_Functions()
{
	m_pTriggerManager->Bind_Func(L"Trigger_Open_WESN",
		bind(&CLevel_World_Home_Lab::Open_Wesn_UI, this, L"Trigger_Open_WESN"));
	m_pTriggerManager->Bind_Func(L"Trigger_Portal_Lab_to_MF",
		bind(&CLevel_World_Home_Lab::Go_Level_World_City, this));
}

void CLevel_World_Home_Lab::Open_Wesn_UI(wstring wstrTriggerName)
{
	UIMGR->SceneInit(L"SCENE_CCTV");
	static_cast<CUI_CCTV*>(UIMGR->GetScene(L"SCENE_CCTV"))->Set_UserType(wstrTriggerName);
}

void CLevel_World_Home_Lab::Go_Level_World_City()
{
	m_bGoLevelWorldCity = true;

	PLAYER_INITIALIZE_DESC PD = {};
	PD.vInitialPos = _float4(-16.5306244f, 0.268467456f, -11.6231022f, 1.00000000f);
	PD.vInitialLookAt = _float4(PD.vInitialPos.x -1.f, 0.f, PD.vInitialPos.z, 1.f);

	CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PD);
}

CLevel_World_Home_Lab* CLevel_World_Home_Lab::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_World_Home_Lab* pInstance = new CLevel_World_Home_Lab(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_World_Home_Lab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_World_Home_Lab::Free()
{
	__super::Free();
	Safe_Release(m_pTriggerManager);
#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG
}

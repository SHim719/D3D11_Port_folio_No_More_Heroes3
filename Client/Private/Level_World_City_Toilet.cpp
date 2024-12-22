#include "Level_World_City_Toilet.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Travis_Manager.h"

#include "UI_Manager.h"
#include "Levels_Header.h"

_bool CLevel_World_City_Toilet::IS_TOILET_END = { false };
_bool CLevel_World_City_Toilet::IS_MINIGAME_END = { false };

CLevel_World_City_Toilet::CLevel_World_City_Toilet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice, _pContext }
{
}

HRESULT CLevel_World_City_Toilet::Initialize()
{
	PLAY_SOUND("BGM - Toilet Cleaning", true, 1.f);

	m_iLevelID = (_uint)LEVEL_WORLD_CITY_TOILET;
	SET_CURLEVEL(m_iLevelID);

#ifdef _DEBUG

	if (FAILED(Ready_Camera()))
		return E_FAIL;

#endif // DEBUG

	if (FAILED(Ready_Map()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	m_pMozaic = m_pGameInstance->Add_Clone(LEVEL_WORLD_CITY_TOILET, L"Layer_Temp", L"Prototype_Mozaic_Quad");
	if (nullptr == m_pMozaic)
		return E_FAIL; 

	return S_OK;
}

void CLevel_World_City_Toilet::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG

	if (IS_MINIGAME_END) {
		if (m_pMozaic) {
			m_pMozaic->Set_Destroy(true);
			m_pMozaic = nullptr;
		}
	}

	if (IS_TOILET_END == true) {
		STOP_SOUND("BGM - Toilet Cleaning");

		UIMGR->ChangeLevel();
		
		GAME_FLOW_SET(FLOW_Quest_ToiletMinigame_Cleared, true);

		PLAYER_INITIALIZE_DESC PD = {};
		PD.vInitialPos = _float4(-21.9864063f, 0.127127081f, -57.4814911f, 1.00000000f);
		PD.vInitialLookAt = _float4(PD.vInitialPos.x - 0.962795079f, 0.f, PD.vInitialPos.z - 0.270232618f, 1.f);
		PD.iPrevLevel = m_iLevelID;
		CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PD);

		m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext));
	}
}

HRESULT CLevel_World_City_Toilet::Render()
{
	return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_World_City_Toilet::Ready_Camera()
{
	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect		= (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear		= 0.1f;
	camDesc.fFar		= 1000.f;
	camDesc.fFovy		= 60.f;
	camDesc.vAt			= { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye		= { 0.f, 2.f, -2.f, 1.f };

	m_pDebugFreeCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));
	if (nullptr == m_pDebugFreeCamera)
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

HRESULT CLevel_World_City_Toilet::Ready_Map()
{
	CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Toilet_Repair.dat", m_iLevelID, m_pDevice, m_pContext);

	return S_OK;
}

HRESULT CLevel_World_City_Toilet::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_WORLD);

	return S_OK;
}

HRESULT CLevel_World_City_Toilet::Ready_Effects()
{
	return S_OK;
}

HRESULT CLevel_World_City_Toilet::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_Toilet", L"Prototype_UI_MiniGame_Toilet"))
		return E_FAIL;

	UIMGR->GetScene(L"SCENE_Minigame_Toilet")->SceneInit();

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_CheckPoint", L"Prototype_UI_ToiletCheckPoint"))
		return E_FAIL;

	return S_OK;
}

#pragma region Cre, Free
CLevel_World_City_Toilet* CLevel_World_City_Toilet::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_World_City_Toilet* pInstance = new CLevel_World_City_Toilet(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_World_City_Toilet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_World_City_Toilet::Free()
{
	__super::Free();

#ifdef _DEBUG
	
	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG

}
#pragma endregion
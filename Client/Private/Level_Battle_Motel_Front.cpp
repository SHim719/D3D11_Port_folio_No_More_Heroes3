#include "Level_Battle_Motel_Front.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Travis_Manager.h"

#include "Monster_Manager.h"

#include "Levels_Header.h"

#include "UI_Manager.h"
#include "UI_InBattle_Letter.h"

#include "Travis.h"
#include "Various_Camera.h"

/* Cinema */
#include "Cinematic_Manager.h"
#include "Cinema_Damon.h"
#include "Cinema_Mbone.h"
#include "Cinema_Fullface.h"
#include "Cinema_Treatment.h"
#include "Cinema_Tripleput.h"
#include "Cinema_Jeane.h"
#include "Cinema_Travis_Motel.h"

CLevel_Battle_Motel_Front::CLevel_Battle_Motel_Front(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Battle_Motel_Front::Initialize()
{
	m_iLevelID = (_uint)LEVEL_BATTLE_MOTEL_FRONT;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - Start the game", true, 0.3f);

#ifdef _DEBUG

	if (FAILED(Ready_Camera()))
		return E_FAIL;

#endif // _DEBUG

	if (FAILED(Ready_Map()))
		return E_FAIL;

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	if (FAILED(Ready_Cinematic()))
		return E_FAIL;

	//Start_Game();

	CINEMAMGR->Start_Cinematic("Motel_Front");

	CINEMAMGR->Bind_CinemaEndFunc(
		[&]()->void {
			CFadeScreen::FADEDESC FD = {};
			FD.fFadeOutSpeed= 1.f;
			FD.fExtraTime	= 1.f;
			FD.fFadeInSpeed	= 1.f;
			FD.pCallback_FadeOutEnd = [&]()->void {CINEMAMGR->Clear_CinemaObjects(); };
			FD.pCallback_FadeInStart = [&]()->void {Start_Game(); };

			CGameObject* pFadeScreen = m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_FadeScreen");
			pFadeScreen->OnEnter_Layer(&FD);
			m_pGameInstance->Insert_GameObject(m_iLevelID, L"Layer_UI", pFadeScreen);
		}
	);

	//if (FAILED(Ready_Fog()))
	//	return E_FAIL;

	m_pGameInstance->Set_Active_Sky(true);

	// TODO : Add Additional Code Here.

	return S_OK;
}

void CLevel_Battle_Motel_Front::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG

	__super::Tick(_fTimeDelta);

	/* WAVE Update */
	if (m_bCinemaEnd == true) {
		m_pMonster_Manager->Update(_fTimeDelta);

		if (m_pMonster_Manager->Get_LastEnemyDead() && !m_bLevelCleard) {
			m_fTimeAcc_FinalDelay += _fTimeDelta;

			if (m_fTimeAcc_FinalDelay > 0.25f) {
				STOP_SOUND("BGM - Start the game");
				PLAY_SOUND("BGM - Defeated or Dead", false, 1.f);
				m_pGameInstance->Set_Active_FinalSlash_RedTone(true);
				m_pGameInstance->Set_TimeScaleWithRealTime(0.001f, 7.f);
				m_bLevelCleard = true;

				UIMGR->CommonBattleEnd(INS_LETTER_KILL);
			}
		}
	}

	if (m_bLevelCleard) {
		m_fTimeAcc_LevelClear += _fTimeDelta;
		if (m_fTimeAcc_LevelClear > 5.5f)
			UIMGR->CommonBattleEndLetterDisappear(INS_LETTER_KILL);

		if (m_fTimeAcc_LevelClear > 7.f) {
			UIMGR->ChangeLevel();
			m_pGameInstance->Set_Active_FinalSlash_RedTone(false);

			PLAYER_INITIALIZE_DESC PlayerDesc = {};
			PlayerDesc.strModelTag = L"Prototype_Model_Travis_World";
			XMStoreFloat4(&PlayerDesc.vInitialPos, GET_PLAYER->Get_Pos());
			XMStoreFloat4(&PlayerDesc.vInitialLookAt, GET_PLAYER->Get_Pos() + GET_PLAYER->Get_Look());

			CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PlayerDesc);

			GAME_FLOW_SET(FLOW_Battle_Motel_Front_Cleared, true);

			m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext));
		}
	}
}

HRESULT CLevel_Battle_Motel_Front::Render()
{
	return S_OK;
}

void CLevel_Battle_Motel_Front::OnExitLevel()
{
	STOP_SOUND("BGM - Defeated or Dead");
	m_pGameInstance->Set_TimeScale(1.f);
	m_pGameInstance->Inactive_RadialBlur(0.f);
	m_pGameInstance->Set_Active_Sky(false);
}

HRESULT CLevel_Battle_Motel_Front::Start_Game()
{
	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_Monster()))
		return E_FAIL;
	 
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Monster"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Player"))
		return E_FAIL;

	m_bCinemaEnd = true;

	return S_OK;
}

#ifdef _DEBUG

HRESULT CLevel_Battle_Motel_Front::Ready_Camera()
{
	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect		= (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear		= 0.1f;
	camDesc.fFar		= 1000.f;
	camDesc.fFovy		= 30.f;
	camDesc.vAt			= { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye		= { 0.f, 2.f, -2.f, 1.f };

	m_pDebugFreeCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));
	if (nullptr == m_pDebugFreeCamera)
		return E_FAIL;

	return S_OK;
}

#endif // _DEBUG

HRESULT CLevel_Battle_Motel_Front::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/TravisMotel.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle_Motel_Front::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_BATTLE_GROUND);
	//CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_BATTLE_GROUND_ARMOR);

	return S_OK;
}

HRESULT CLevel_Battle_Motel_Front::Ready_Monster()
{
	/* ===Memory Rick Warning=== */ /* Object pool */
	CMonster_Manager::MOSTER_MANAGER_DESC tManagerDesc = {};
	tManagerDesc.iBoneTotalCount = 2;
	tManagerDesc.iTripleputTotalCount = 1;
	/* Create */
	m_pMonster_Manager = CMonster_Manager::Create(m_pDevice, m_pContext, &tManagerDesc);

	/* WAVE_1 */
	CMonster_Manager::MONSTER_WAVE_DESC tWaveDesc = {};
	tWaveDesc.iNumBone = 2;
	tWaveDesc.f4StartPos[0] = _float4{ -21.368f, 0.f, -3.178f, 1.f };
	tWaveDesc.f4StartPos[1] = _float4{ -21.266f, 0.f, 15.287f, 1.f };
	tWaveDesc.bLast = false;

	m_pMonster_Manager->Wave_Setting(tWaveDesc);

	/* WAVE_2*/
	CMonster_Manager::MONSTER_WAVE_DESC tWaveDesc2 = {};
	tWaveDesc2.iNumBone = 2;
	tWaveDesc2.f4StartPos[0] = _float4{ -19.6f, 0.f, 10.6f, 1.f };
	tWaveDesc2.f4StartPos[1] = _float4{ -19.6f, 0.f, 0.f, 1.f };
	tWaveDesc2.iNumTripleput = 1;
	tWaveDesc2.f4StartPos[2] = _float4{ -19.6f, 0.f, 5.7f, 1.f };
	tWaveDesc2.bLast = true;

	m_pMonster_Manager->Wave_Setting(tWaveDesc2);

	return S_OK;
}

HRESULT CLevel_Battle_Motel_Front::Ready_Effects()
{
	return S_OK;
}

HRESULT CLevel_Battle_Motel_Front::Ready_UI()
{
	//if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Monster"))
	//	return E_FAIL;

	//if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Player"))
	//	return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Letter"))
		return E_FAIL;

	//if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
	//	return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Mechanic_Battle", L"Prototype_UI_InBattle_Mechanic"))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle_Motel_Front::Ready_Fog()
{
	FOG_DESCS FogDesc{};

	FogDesc.fFogDensity = 1.f;
	FogDesc.fFogDistStart = 260.f;
	FogDesc.fFogDistEnd = 600.f;
	XMStoreFloat4(&FogDesc.vFogColor, XMVectorSet(0.13f, 0.43f, 1.f, 1.f) * 1.1f);

	m_pGameInstance->Set_Active_Fog(true, &FogDesc);

	return S_OK;
}

HRESULT CLevel_Battle_Motel_Front::Ready_Cinematic()
{
	CINEMAMGR->Clear_CinemaObjects();

	CCinema_Damon* pCinemaDamon = static_cast<CCinema_Damon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Damon"));
	if (nullptr == pCinemaDamon)
		return E_FAIL;

	CCinema_Mbone* pCinemaMbone = static_cast<CCinema_Mbone*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Mbone"));
	if (nullptr == pCinemaMbone)
		return E_FAIL;

	CCinema_Treatment* pCinemaTreatment = static_cast<CCinema_Treatment*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Treatment"));
	if (nullptr == pCinemaTreatment)
		return E_FAIL;

	CCinema_Fullface* pCinemaFullface = static_cast<CCinema_Fullface*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Fullface"));
	if (nullptr == pCinemaFullface)
		return E_FAIL;

	CCinema_Tripleput* pCinemaTripleput = static_cast<CCinema_Tripleput*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Tripleput"));
	if (nullptr == pCinemaTripleput)
		return E_FAIL;

	CCinema_Jeane* pCinemaJeane = static_cast<CCinema_Jeane*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Jeane"));
	if (nullptr == pCinemaJeane)
		return E_FAIL;

	CCinema_Travis_Motel* pCinemaTravisMotel = static_cast<CCinema_Travis_Motel*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_Cinema_Travis_Motel"));
	if (nullptr == pCinemaTravisMotel)
		return E_FAIL;

	CINEMAMGR->Bind_DefaultEvents();

	return S_OK;
}

CLevel_Battle_Motel_Front* CLevel_Battle_Motel_Front::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Battle_Motel_Front* pInstance = new CLevel_Battle_Motel_Front(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Battle_Motel_Front"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Battle_Motel_Front::Free()
{
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Travis_Heavy_RedWeapon_Final");
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Trail_Final");
	__super::Free();

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG

	CMapLoader::Destroy_Instance();
	Safe_Release(m_pMonster_Manager);
}

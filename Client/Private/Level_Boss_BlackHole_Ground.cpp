#include "Level_Boss_BlackHole_Ground.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Effect_Manager.h"
#include "Travis_Manager.h"
#include "Travis.h"

#include "Levels_Header.h"

#include "UI_Manager.h"

_bool CLevel_Boss_BlackHole_Ground::IS_BOSS_DEAD = { false };

CLevel_Boss_BlackHole_Ground::CLevel_Boss_BlackHole_Ground(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Boss_BlackHole_Ground::Initialize()
{
	m_iLevelID = (_uint)LEVEL_BOSS_BLACKHOLE_GROUND;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - Mr Blackhole Battle", true, 0.5f);
	PLAY_SOUND("Sandstorm", true, 0.5f);

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

	// TODO : Add Additional Code Here.
	if (FAILED(Ready_UI()))
		return E_FAIL;

	if (FAILED(Ready_Fog()))
		return E_FAIL;

	m_pGameInstance->Set_Active_Sky(true);
	return S_OK;
}

void CLevel_Boss_BlackHole_Ground::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG

	if (IS_BOSS_DEAD && !m_bLevelCleard) {
		m_fTimeAcc_FinalDelay += _fTimeDelta;

		if (m_fTimeAcc_FinalDelay > 0.25f) {
			STOP_SOUND("Sandstorm");
			STOP_SOUND("BGM - Mr Blackhole Battle");
			PLAY_SOUND("BGM - Defeated or Dead", false, 1.f);
			//m_pGameInstance->Set_Active_FinalSlash_RedTone(true);

			RADIALBLUR_DESCS RD = {};
			RD.fBlurRadius = 10.f;
			RD.fBlurStrength = 10.f;
			RD.pActor = GET_PLAYER;
			m_pGameInstance->Active_RadialBlur(RD);

			m_pGameInstance->Set_TimeScaleWithRealTime(0.001f, 7.f);
			m_bLevelCleard = true;
			UIMGR->CommonBattleEnd(INS_LETTER_QUESTIONMARK);
		}
	}

	if (m_bLevelCleard) {
		m_fTimeAcc_LevelClear += _fTimeDelta;
		if (m_fTimeAcc_LevelClear > 5.5f)
			UIMGR->CommonBattleEndLetterDisappear(INS_LETTER_QUESTIONMARK);

		if (m_fTimeAcc_LevelClear > 7.f) {
			//m_pGameInstance->Set_Active_FinalSlash_RedTone(false);
			UIMGR->ChangeLevel();
			m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_BOSS_BLACKHOLE_SPACE, m_pDevice, m_pContext));
		}
	}
}

HRESULT CLevel_Boss_BlackHole_Ground::Render()
{
	return S_OK;
}

void CLevel_Boss_BlackHole_Ground::OnExitLevel()
{
	STOP_SOUND("BGM - Defeated or Dead");
	m_pGameInstance->Set_TimeScale(1.f);
	m_pGameInstance->Inactive_RadialBlur(0.1f);
	m_pGameInstance->Set_Active_Fog(false);
	m_pGameInstance->Set_Active_Sky(false);
}

#ifdef _DEBUG

HRESULT CLevel_Boss_BlackHole_Ground::Ready_Camera()
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

HRESULT CLevel_Boss_BlackHole_Ground::Ready_Fog()
{     
	FOG_DESCS FogDesc;
	FogDesc.vFogColor = _float4(0.45f, 0.07f, 0.f, 1.f);
	FogDesc.fFogHeightStart = 0.f;
	FogDesc.fFogHeightEnd = 40.f;
	FogDesc.fFogDistStart = 3.f;
	FogDesc.fFogDistEnd = 10.f;
	FogDesc.fFogDensity = 0.6f;
	FogDesc.bNoisedFog = true;
	FogDesc.fFogSpeed = 0.5f;
	FogDesc.fNoiseScale = 0.05f;

	m_pGameInstance->Set_Active_Fog(true, &FogDesc);

	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Ground::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/Land_MrBlackHole.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"BH_Terrain", L"Prototype_BH_Terrain");

	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Ground::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_BATTLE_GROUND);

	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Ground::Ready_Monster()
{
	/* Boss_MrBlackhole */
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Enemy", L"Prototype_GameObject_MrBlackhole"))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Ground::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Letter"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Boss"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Player"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Mechanic_Battle", L"Prototype_UI_InBattle_Mechanic"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_Battle_Result"))
		return E_FAIL;

	//if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
	//	return E_FAIL;

	return S_OK;
}


CLevel_Boss_BlackHole_Ground* CLevel_Boss_BlackHole_Ground::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Boss_BlackHole_Ground* pInstance = new CLevel_Boss_BlackHole_Ground(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Boss_BlackHole_Ground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Boss_BlackHole_Ground::Free()
{
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Travis_Heavy_RedWeapon_Final");
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Trail_Final");
	__super::Free();

	CMapLoader::Destroy_Instance();

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG


}

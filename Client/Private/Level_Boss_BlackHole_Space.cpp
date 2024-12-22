#include "Level_Boss_BlackHole_Space.h"

#include "GameInstance.h"

#include "Free_Camera.h"
#include "Travis_Manager.h"
#include "SkyBox.h"

#include "Levels_Header.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"

_bool CLevel_Boss_BlackHole_Space::IS_BOSS_DEAD = { false };

CLevel_Boss_BlackHole_Space::CLevel_Boss_BlackHole_Space(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Boss_BlackHole_Space::Initialize()
{
	m_iLevelID = (_uint)LEVEL_BOSS_BLACKHOLE_SPACE;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - Mr Blackhole Space", true, 0.5f);

#ifdef _DEBUG

	if (FAILED(Ready_Camera()))
		return E_FAIL;

#endif // _DEBUG

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_SkyBox()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_Monster()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	// TODO : Add Additional Code Here.

	//m_pGameInstance->Set_Active_SSR(true);

	return S_OK;
}

void CLevel_Boss_BlackHole_Space::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG

	if (IS_BOSS_DEAD && !m_bLevelCleard) {
		m_fTimeAcc_FinalDelay += _fTimeDelta;

		if (m_fTimeAcc_FinalDelay > 1.f) {
			m_pGameInstance->Set_Active_FinalSlash_RedTone(true);
			//m_pGameInstance->Set_TimeScaleWithRealTime(0.001f, 5.f);
			m_bLevelCleard = true;
			UIMGR->CommonBattleEnd(INS_LETTER_BLACKHOLE);
		}
	}

	if (m_bLevelCleard) {
		m_fTimeAcc_LevelClear += _fTimeDelta;
		if (m_fTimeAcc_LevelClear > 5.5f)
			UIMGR->CommonBattleEndLetterDisappear(INS_LETTER_BLACKHOLE);

		if (m_fTimeAcc_LevelClear > 7.f) {
			if (!GAME_FLOW_CHECK(FLOW_Boss_Blackhole_Cleared)) {
				m_pGameInstance->Set_Active_FinalSlash_RedTone(false);

				PLAYER_INITIALIZE_DESC PD = {};
				PD.vInitialPos = _float4(-13.f, 0.3f, -1.f, 1.f);
				PD.vInitialLookAt = _float4(-13.f, 0.3f, 0.f, 1.f);
				CTravis_Manager::Get_Instance()->Regist_IntializeDesc(LEVEL_WORLD_CITY, PD);

				GAME_FLOW_SET(FLOW_Boss_Blackhole_Cleared, true);

				UIMGR->ChangeLevel();
				m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_CITY, m_pDevice, m_pContext));
			}
		}
	}
}

HRESULT CLevel_Boss_BlackHole_Space::Render()
{
	return S_OK;
}

#ifdef _DEBUG

HRESULT CLevel_Boss_BlackHole_Space::Ready_Camera()
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

void CLevel_Boss_BlackHole_Space::OnExitLevel()
{
	STOP_SOUND("BGM - Defeated or Dead");
	m_pGameInstance->Set_Active_Shadow(true);
	m_pGameInstance->Set_Active_SSR(false);
	m_pGameInstance->Inactive_RadialBlur(0.1f);
	m_pGameInstance->Set_TimeScale(1.f);
}

HRESULT CLevel_Boss_BlackHole_Space::Ready_SkyBox()
{
	wstring wstrSkyTextureTag = L"Prototype_CubeTexture_Galaxy";
	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_SkyBox", L"Prototype_SkyBox", &wstrSkyTextureTag);

	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Space::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.fLightStrength = 0.7f;

	m_pGameInstance->Set_DirectionalLight(LightDesc);
	m_pGameInstance->Set_AmbientStrength(0.5f);

	if (FAILED(Initialize_IBL(L"Space")))
		return E_FAIL;

	m_pGameInstance->Set_Active_Shadow(false);

	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Space::Ready_Player()
{
	PLAYER_INITIALIZE_DESC PlayerInitializeDesc = {};
	PlayerInitializeDesc.strModelTag = L"Prototype_Model_Travis";
	PlayerInitializeDesc.vInitialPos = _float4(0.f, 0.f, 0.f, 1.f);

	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_BATTLE_SPACE, &PlayerInitializeDesc);


	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Space::Ready_Monster()
{
	/* Boss_MrBlackhole_Space */
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Enemy", L"Prototype_GameObject_MrBlackhole_Space"))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Space::Ready_Effects()
{
	return S_OK;
}

HRESULT CLevel_Boss_BlackHole_Space::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_Cosmic", L"Prototype_UI_Cosmic"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle", L"Prototype_UI_InBattle_Letter"))
		return E_FAIL;

	//if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
	//	return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle", L"Prototype_UI_Battle_Result"))
		return E_FAIL;

	return S_OK;
}

CLevel_Boss_BlackHole_Space* CLevel_Boss_BlackHole_Space::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Boss_BlackHole_Space* pInstance = new CLevel_Boss_BlackHole_Space(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Boss_BlackHole_Space"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Boss_BlackHole_Space::Free()
{
	EFFECTMGR->Inactive_Effect(LEVEL_BOSS_BLACKHOLE_SPACE, "Space_Explosion");
	__super::Free();

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG
}

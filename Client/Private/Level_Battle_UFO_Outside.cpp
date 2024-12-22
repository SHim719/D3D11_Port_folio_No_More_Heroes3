#include "Level_Battle_UFO_Outside.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"

/* Monster */
#include "Monster_Manager.h"

#include "Travis_Manager.h"
#include "Levels_Header.h"

#include "SkyCloud.h"

#include "UI_Manager.h"
#include "Effect_Manager.h"


CLevel_Battle_UFO_Outside::CLevel_Battle_UFO_Outside(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Battle_UFO_Outside::Initialize()
{
	m_iLevelID = (_uint)LEVEL_BATTLE_UFO_OUTSIDE;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - UFO Outside", true, 0.5f);

#ifdef _DEBUG

	if (FAILED(Ready_Camera()))
		return E_FAIL;

#endif // _DEBUG

	//if (FAILED(Ready_SkyBox()))
	//	return E_FAIL;

	if (FAILED(Ready_Map()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	// TODO : Add Additional Code Here.

	if (FAILED(Ready_UI()))
		return E_FAIL;

	m_pGameInstance->Set_Active_Sky(true);

	return S_OK;
}

void CLevel_Battle_UFO_Outside::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG

	if (KEY_DOWN(eKeyCode::F2)) {
		m_pGameInstance->Change_MainCamera(m_pDebugFreeCamera);
	}

#endif // _DEBUG

	CMonster_Manager::Get_Instance()->Update(_fTimeDelta);

	if (m_pMonster_Manager->Get_LastEnemyDead() && !m_bLevelCleard) {
		m_fTimeAcc_FinalDelay += _fTimeDelta;

		if (m_fTimeAcc_FinalDelay > 0.25f) {
			STOP_SOUND("BGM - UFO Outside");
			PLAY_SOUND("BGM - Defeated or Dead", false, 1.f);
			m_pGameInstance->Set_Active_FinalSlash_RedTone(true);
			m_pGameInstance->Set_TimeScaleWithRealTime(0.001f, 7.f);
			m_bLevelCleard = true;
			UIMGR->CommonBattleEnd();
		}
	}
	
	if (m_bLevelCleard) {
		m_fTimeAcc_LevelClear += _fTimeDelta;
		if (m_fTimeAcc_LevelClear > 7.f) {
			m_pGameInstance->Set_Active_FinalSlash_RedTone(false);
	
			UIMGR->ChangeLevel();
	
			m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_UFO, m_pDevice, m_pContext));
		}
	}
}

HRESULT CLevel_Battle_UFO_Outside::Render()
{
	return S_OK;
}

void CLevel_Battle_UFO_Outside::OnExitLevel()
{
	STOP_SOUND("BGM - Defeated or Dead");
	m_pGameInstance->Set_TimeScale(1.f);
	m_pGameInstance->Inactive_RadialBlur(0.f);
	m_pGameInstance->Set_Active_Fog(false);
	m_pGameInstance->Set_Active_Sky(false);
}

#ifdef _DEBUG

HRESULT CLevel_Battle_UFO_Outside::Ready_Camera()
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

HRESULT CLevel_Battle_UFO_Outside::Ready_Map()
{
	if (FAILED(CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/UFO_Outside.dat", m_iLevelID, m_pDevice, m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle_UFO_Outside::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_BATTLE_GROUND);

	return S_OK;
}

HRESULT CLevel_Battle_UFO_Outside::Ready_Monster()
{
	/* ===Memory Rick Warning=== */ /* Object pool */
	CMonster_Manager::MOSTER_MANAGER_DESC tManagerDesc = {};
	tManagerDesc.iBoneTotalCount = 2;
	tManagerDesc.iFullfaceTotalCount = 2;
	/* Create */
	m_pMonster_Manager = CMonster_Manager::Create(m_pDevice, m_pContext, &tManagerDesc);

	/* WAVE_1 */
	CMonster_Manager::MONSTER_WAVE_DESC tWaveDesc = {};
	tWaveDesc.iNumBone = 2;
	tWaveDesc.f4StartPos[0] = _float4{ -2.9f, 0.f, 8.859f, 1.f };
	//tWaveDesc.f4StartPos[1] = _float4{ 0.17f, 0.f, 8.859f, 1.f };
	tWaveDesc.f4StartPos[1] = _float4{ 3.3f, 0.f, 8.859f, 1.f };
	tWaveDesc.bLast = false;

	m_pMonster_Manager->Wave_Setting(tWaveDesc);

	/* WAVE_2 */
	CMonster_Manager::MONSTER_WAVE_DESC tWaveDesc2 = {};
	tWaveDesc2.iNumFullface = 2;
	tWaveDesc2.f4StartPos[0] = _float4{ -12.898f, 0.f, -0.760f, 1.f };
	tWaveDesc2.f4StartPos[1] = _float4{ 12.898f, 0.f, -0.760f, 1.f };
	tWaveDesc2.bLast = true;

	m_pMonster_Manager->Wave_Setting(tWaveDesc2);

	return S_OK;
}

HRESULT CLevel_Battle_UFO_Outside::Ready_Effects()
{
	return S_OK;
}

HRESULT CLevel_Battle_UFO_Outside::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Monster"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Player"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Battle_UFO_Outside::Ready_SkyBox()
{
	wstring wstrSkyTextureTag = L"Prototype_CubeTexture_Sky";
	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_SkyBox", L"Prototype_SkyBox", &wstrSkyTextureTag);

	return S_OK;
}

CLevel_Battle_UFO_Outside* CLevel_Battle_UFO_Outside::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Battle_UFO_Outside* pInstance = new CLevel_Battle_UFO_Outside(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Battle_UFO_Outside"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Battle_UFO_Outside::Free()
{
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Travis_Heavy_RedWeapon_Final");
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Trail_Final");
	__super::Free();

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG

	Safe_Release(m_pMonster_Manager);
}

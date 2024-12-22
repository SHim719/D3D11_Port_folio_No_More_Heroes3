#include "Level_Battle_UFO_Inside.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Free_Camera.h"
#include "Travis_Manager.h"

#include "Monster_Manager.h"

#include "Levels_Header.h"
#include "UI_Manager.h"
#include "UI_InBattle_Letter.h"
#include "Effect_Manager.h"

CLevel_Battle_UFO_Inside::CLevel_Battle_UFO_Inside(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Battle_UFO_Inside::Initialize()
{
	m_iLevelID = (_uint)LEVEL_BATTLE_UFO_INSIDE;
	SET_CURLEVEL(m_iLevelID);
	PLAY_SOUND("BGM - Beast Test", true, 0.5f);

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

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	// TODO : Add Additional Code Here.

	if (FAILED(Ready_UI()))
		return E_FAIL;

	m_pGameInstance->Set_Active_SSR(true);

	return S_OK;
}

void CLevel_Battle_UFO_Inside::Tick(_float _fTimeDelta)
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
			STOP_SOUND("BGM - Beast Test");
			PLAY_SOUND("BGM - Defeated or Dead", false, 1.f);
			m_pGameInstance->Set_Active_FinalSlash_RedTone(true);
			m_pGameInstance->Set_TimeScaleWithRealTime(0.001f, 7.f);
			m_bLevelCleard = true;
			UIMGR->CommonBattleEnd(INS_LETTER_KILL);
		}
	}

	if (m_bLevelCleard) {
		m_fTimeAcc_LevelClear += _fTimeDelta;

		if (m_fTimeAcc_LevelClear > 5.5f)
			UIMGR->CommonBattleEndLetterDisappear(INS_LETTER_KILL);

		if (m_fTimeAcc_LevelClear > 7.f) {
			UIMGR->ChangeLevel();
			m_pGameInstance->Set_Active_FinalSlash_RedTone(false);
			m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_WORLD_UFO, m_pDevice, m_pContext));
		}
	}
}

HRESULT CLevel_Battle_UFO_Inside::Render()
{
	return S_OK;
}

void CLevel_Battle_UFO_Inside::OnExitLevel()
{
	STOP_SOUND("BGM - Defeated or Dead");
	m_pGameInstance->Inactive_RadialBlur(0.1f);
	m_pGameInstance->Set_TimeScale(1.f);
}

#ifdef _DEBUG

HRESULT CLevel_Battle_UFO_Inside::Ready_Camera()
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

HRESULT CLevel_Battle_UFO_Inside::Ready_Map()
{
	CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/UFO_Inside.dat", m_iLevelID, m_pDevice, m_pContext);

	return S_OK;
}

HRESULT CLevel_Battle_UFO_Inside::Ready_Player()
{
	CTravis_Manager::Get_Instance()->Change_Player(m_iLevelID, (_uint)TRAVIS_TYPES::TRAVIS_BATTLE_GROUND);

	return S_OK;
}

HRESULT CLevel_Battle_UFO_Inside::Ready_Monster()
{
	/* ===Memory Rick Warning=== */ /* Object pool */
	CMonster_Manager::MOSTER_MANAGER_DESC tManagerDesc = {};
	tManagerDesc.iBoneTotalCount = 2;
	tManagerDesc.iTreatmentTotalCount = 1;

	m_pMonster_Manager = CMonster_Manager::Create(m_pDevice, m_pContext, &tManagerDesc);

	/* WAVE_1 */
	CMonster_Manager::MONSTER_WAVE_DESC tWaveDesc = {};
	tWaveDesc.iNumBone = 2;
	tWaveDesc.f4StartPos[0] = _float4{ -4.876f, 0.f, 23.408f, 1.f };
	tWaveDesc.f4StartPos[1] = _float4{ 4.137f, 0.f, 23.191f, 1.f };;
	tWaveDesc.iNumTreatment = 0;
	tWaveDesc.bLast = false;

	m_pMonster_Manager->Wave_Setting(tWaveDesc);

	/* WAVE_2*/
	CMonster_Manager::MONSTER_WAVE_DESC tWaveDesc2 = {};
	tWaveDesc2.iNumBone = 0;
	tWaveDesc2.iNumTreatment = 1;
	tWaveDesc2.f4StartPos[0] = _float4{ -0.317f, 0.f, 27.979f, 1.f };
	tWaveDesc2.bLast = true;

	m_pMonster_Manager->Wave_Setting(tWaveDesc2);

	return S_OK;
}

HRESULT CLevel_Battle_UFO_Inside::Ready_Effects()
{
	return S_OK;
}

HRESULT CLevel_Battle_UFO_Inside::Ready_UI()
{
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Monster"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Named"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Player"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Mechanic_Battle", L"Prototype_UI_InBattle_Mechanic"))
		return E_FAIL;

	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_InBattle_Scene", L"Prototype_UI_InBattle_Letter"))
		return E_FAIL;

	//if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_PlayerStatus", L"Prototype_UI_PlayerStatus"))
	//	return E_FAIL;

	return S_OK;
}

CLevel_Battle_UFO_Inside* CLevel_Battle_UFO_Inside::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Battle_UFO_Inside* pInstance = new CLevel_Battle_UFO_Inside(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Battle_UFO_Inside"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Battle_UFO_Inside::Free()
{
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Travis_Heavy_RedWeapon_Final");
	EFFECTMGR->Inactive_Effect(LEVEL_STATIC, "Trail_Final");
	__super::Free();

#ifdef _DEBUG

	Safe_Release(m_pDebugFreeCamera);

#endif // _DEBUG

	Safe_Release(m_pMonster_Manager);
}

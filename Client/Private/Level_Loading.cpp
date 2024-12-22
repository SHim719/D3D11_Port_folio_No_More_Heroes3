#include "Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Levels_Header.h"

#include "UI_Manager.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_iLevelID = LEVEL_LOADING;

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	/* 00. Loading */
	if (nullptr == m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI_Loading", L"Prototype_UI_Loading"))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float TimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		CLevel* pLevel = { nullptr };

		switch (m_eNextLevel)
		{
		case LEVEL_MAINMENU:
			pLevel = CLevel_MainMenu::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_WORLD_HOME:
			pLevel = CLevel_World_Home::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_WORLD_HOME_LAB:
			pLevel = CLevel_World_Home_Lab::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BATTLE_MOTEL_FRONT:
			pLevel = CLevel_Battle_Motel_Front::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_WORLD_CITY:
			pLevel = CLevel_World_City::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_WORLD_CITY_TRASHCOLLECTOR:
			pLevel = CLevel_World_City_TrashCollector::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_WORLD_CITY_TOILET:
			pLevel = CLevel_World_City_Toilet::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BATTLE_UFO_OUTSIDE:
			pLevel = CLevel_Battle_UFO_Outside::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_WORLD_UFO:
			pLevel = CLevel_World_UFO::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_WORLD_UFO_TOILET:
			pLevel = CLevel_World_UFO_Toilet::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BATTLE_UFO_INSIDE:
			pLevel = CLevel_Battle_UFO_Inside::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS_BLACKHOLE_GROUND:
			pLevel = CLevel_Boss_BlackHole_Ground::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS_BLACKHOLE_SPACE:
			pLevel = CLevel_Boss_BlackHole_Space::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_SMASHBROS_BOSS_DAMON:
			pLevel = CLevel_SmashBros_Boss_Damon::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS_SONICJUICE:
			pLevel = CLevel_Boss_SonicJuice::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_ARROW_A_ROW:
			pLevel = CLevel_Arrow_A_Row::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_MARIO:
			pLevel = CLevel_Mario::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BATTLE_ISLAND:
			pLevel = CLevel_Battle_Island::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_ENDING:
			pLevel = CLevel_Ending::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		ADD_EVENT(bind(&CGameInstance::Change_Level, m_pGameInstance, pLevel));
		UIMGR->Set_ReturnToPool(L"UI_Loading", true);
	}
}


HRESULT CLevel_Loading::Render()
{
	if (true == m_pLoader->isFinished())
	{
		SetWindowText(g_hWnd, L"로딩 완료.");
	}


	return S_OK;
}


CLevel_Loading* CLevel_Loading::Create(LEVEL eNextLevel, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}

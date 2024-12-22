#include "Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Level_AnimTool.h"
#include "Level_UiTool.h"
#include "Level_EffectTool.h"
#include "Level_MapTool.h"
#include "Level_CinemaTool.h"

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

	return S_OK;
}

void CLevel_Loading::Tick(_float TimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		CLevel* pLevel = { nullptr };

		switch (m_eNextLevel)
		{
		case LEVEL_ANIMTOOL:
			pLevel = CLevel_AnimTool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_UITOOL:
			pLevel = CLevel_UiTool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_EFFECTTOOL:
			pLevel = CLevel_EffectTool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_MAPTOOL:
			pLevel = CLevel_MapTool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_CINEMATOOL:
			pLevel = CLevel_CinemaTool::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		ADD_EVENT(bind(&CGameInstance::Change_Level, m_pGameInstance, pLevel));
	}
}


HRESULT CLevel_Loading::Render()
{

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

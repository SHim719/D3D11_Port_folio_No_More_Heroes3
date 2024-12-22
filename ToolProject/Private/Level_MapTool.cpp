#include "Level_MapTool.h"
#include "GameInstance.h"
#include "Free_Camera.h"

CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MapTool::Initialize()
{
	m_iLevelID = LEVEL_MAPTOOL;

	SET_CURLEVEL(m_iLevelID);

	m_pGameInstance->Set_PBR(true);

	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
{

}

HRESULT CLevel_MapTool::Render()
{
	m_pGameInstance->Render_PickingColor();

	return S_OK;
}

CLevel_MapTool* CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MapTool* pInstance = new CLevel_MapTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_MapTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{
	__super::Free();
}

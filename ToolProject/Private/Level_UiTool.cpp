#include "Level_UiTool.h"
#include "GameInstance.h"
#include "Free_Camera.h"

#include "ToolUIObj.h"

CLevel_UiTool::CLevel_UiTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_UiTool::Initialize()
{
	m_iLevelID = LEVEL_UITOOL;

	SET_CURLEVEL(m_iLevelID);

	return S_OK;
}

void CLevel_UiTool::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_UiTool::Render()
{
	return S_OK;
}

CLevel_UiTool* CLevel_UiTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_UiTool* pInstance = new CLevel_UiTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_UiTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_UiTool::Free()
{
	__super::Free();
}

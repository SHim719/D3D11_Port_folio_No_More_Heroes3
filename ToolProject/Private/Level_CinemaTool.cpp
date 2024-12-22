#include "Level_CinemaTool.h"
#include "GameInstance.h"
#include "Free_Camera.h"

CLevel_CinemaTool::CLevel_CinemaTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_CinemaTool::Initialize()
{
	m_iLevelID = LEVEL_CINEMATOOL;

	SET_CURLEVEL(m_iLevelID);

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 0.f, 0.f);
	LightDesc.fLightStrength = 1.f;

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Light", L"Prototype_LightObject", &LightDesc);

	return S_OK;
}

void CLevel_CinemaTool::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_CinemaTool::Render()
{
	return S_OK;
}

CLevel_CinemaTool* CLevel_CinemaTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_CinemaTool* pInstance = new CLevel_CinemaTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_CinemaTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_CinemaTool::Free()
{
	__super::Free();
	Safe_Release(pCamera);
}

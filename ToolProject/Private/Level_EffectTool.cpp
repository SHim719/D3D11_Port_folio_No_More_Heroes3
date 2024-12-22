#include "Level_EffectTool.h"
#include "GameInstance.h"
#include "Free_Camera.h"

CLevel_EffectTool::CLevel_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_EffectTool::Initialize()
{
	m_iLevelID = LEVEL_EFFECTTOOL;

	SET_CURLEVEL(m_iLevelID);

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 0.f, 0.f);
	LightDesc.fLightStrength = 1.f;

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Light", L"Prototype_LightObject", &LightDesc);
  	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"TestGround", L"Prototype_TestGround");


	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 100.f;
	camDesc.fFovy = 70.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));
	m_pGameInstance->Change_MainCamera(pCamera);

	return S_OK;
}

void CLevel_EffectTool::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_EffectTool::Render()
{
    return S_OK;
}

CLevel_EffectTool* CLevel_EffectTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_EffectTool* pInstance = new CLevel_EffectTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_EffectTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EffectTool::Free()
{
	__super::Free();
	Safe_Release(pCamera);
}

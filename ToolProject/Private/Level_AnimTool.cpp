#include "Level_AnimTool.h"
#include "GameInstance.h"
#include "Free_Camera.h"
#include "SkyBox.h"

CLevel_AnimTool::CLevel_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_AnimTool::Initialize()
{
	m_iLevelID = LEVEL_ANIMTOOL;

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
	
	wstring wstrSkyBoxTexture = L"Prototype_SkyBox_Texture";
	m_pGameInstance->Add_Prototype(GET_CURLEVEL, wstrSkyBoxTexture, CTexture::Create(m_pDevice, m_pContext, L"../../Resources/SkyBox/Sky.dds"));
	m_pGameInstance->Add_Prototype(L"Prototype_SkyBox", CSkyBox::Create(m_pDevice, m_pContext));
	
	m_pGameInstance->Add_Clone(LEVEL_STATIC, L"Layer_SkyBox", L"Prototype_SkyBox", &wstrSkyBoxTexture);

	m_pGameInstance->Set_PBR(false);

	return S_OK;
}

void CLevel_AnimTool::Tick(_float fTimeDelta)
{

}

HRESULT CLevel_AnimTool::Render()
{
	return S_OK;
}

CLevel_AnimTool* CLevel_AnimTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_AnimTool* pInstance = new CLevel_AnimTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_AnimTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_AnimTool::Free()
{
	__super::Free();
}

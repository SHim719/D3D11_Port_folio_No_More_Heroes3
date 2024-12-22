#include "Level_Ending.h"
#include "GameInstance.h"
#include "Various_Camera.h"
#include "Travis_Ending.h"
#include "Jeane_Ending.h"

#include "Free_Camera.h"

#include "FadeScreen.h"


CLevel_Ending::CLevel_Ending(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_Ending::Initialize()
{
	m_iLevelID = (_uint)LEVEL_ENDING;
	SET_CURLEVEL(m_iLevelID);
	//PLAY_SOUND("BGM - Start the game", true, 0.3f);



	m_pLastJeane = static_cast<CJeane_Ending*>(m_pGameInstance->Clone_GameObject(L"Prototype_Jeane_Ending"));

	//LIGHT_DESC			LightDesc{};
	//LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	//LightDesc.fLightStrength = 0.4f;
	//
	//m_pGameInstance->Set_DirectionalLight(LightDesc);
	//m_pGameInstance->Set_AmbientStrength(0.5f);

	CVarious_Camera::VARCAM_DESC CameraDesc = {};
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fFovy = 60.f;
	CameraDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	CameraDesc.vEye = { 0.f, 2.f, -2.f, 1.f };
	//CameraDesc.pOwner = pTravis;

	m_pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &CameraDesc));
	if (nullptr == m_pCamera)
		return E_FAIL;

	m_pCamera->Get_Transform()->Set_Position({ -0.784022f, 1.286824f, 3.08902f, 1.f });
	m_pCamera->Get_Transform()->LookTo({ -0.044353f, -0.07489f, -0.9962f, 0.f });

	m_pGameInstance->Change_MainCamera(m_pCamera);


	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI", L"Prototype_UI_Logo");


	m_pGameInstance->Set_Active_Sky(false);

	PLAY_SOUND("Ending", false, 1.f);

	return S_OK;
}

void CLevel_Ending::Tick(_float _fTimeDelta)
{
	switch (m_eEndingState)
	{
	case LOGO:
	{
		m_fTimeAcc += _fTimeDelta;
		if (m_fTimeAcc >= m_fLogoTime)
		{
			CFadeScreen::FADEDESC FadeDesc{};
			FadeDesc.eFadeColor = CFadeScreen::BLACK;
			FadeDesc.fFadeOutSpeed = 10.f;
			FadeDesc.fFadeInSpeed = 1.f;
			FadeDesc.pCallback_FadeInStart = bind(&CLevel_Ending::Setting_EndingScene, this);

			CGameObject* pFadeScreen = m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_FadeScreen", &FadeDesc);
			pFadeScreen->OnEnter_Layer(&FadeDesc);
			m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_FadeScreen", pFadeScreen);
			m_fTimeAcc = 0.f;
		}
		break;
	}
	case ING:
	{



		break;
	}
	case LAST_JEANE:
		break;

	}

}

HRESULT CLevel_Ending::Render()
{
	return S_OK;
}

void CLevel_Ending::OnExitLevel()
{
	m_pGameInstance->Set_TimeScale(1.f);
	m_pGameInstance->Inactive_RadialBlur(0.f);
	
}

void CLevel_Ending::Setting_EndingScene()
{
	m_eEndingState = ING;

	CTravis_Ending* pTravis = static_cast<CTravis_Ending*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Obj", L"Prototype_Travis_Ending"));
	CJeane_Ending* pJeane = static_cast<CJeane_Ending*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Obj", L"Prototype_Jeane_Ending"));

	//ÀÌÆåÆ®

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Text", L"Prototype_CreditText");
}

CLevel_Ending* CLevel_Ending::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Ending* pInstance = new CLevel_Ending(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Ending"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();
	Safe_Release(m_pCamera);
	Safe_Release(m_pLastJeane);
}

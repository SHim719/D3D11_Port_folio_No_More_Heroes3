#include "Level_UFO_Inside.h"

#include "GameInstance.h"
#include "MapLoader.h"

#include "Various_Camera.h"

CLevel_UFO_Inside::CLevel_UFO_Inside(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CClientLevel{ _pDevice,_pContext }
{
}

HRESULT CLevel_UFO_Inside::Initialize()
{
	m_iLevelID = (_uint)LEVEL_UFO_INSIDE;
	SET_CURLEVEL(m_iLevelID);

	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 1000.f;
	camDesc.fFovy = 60.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	CVarious_Camera* pVariousCamera = static_cast<CVarious_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_VariousCamera", &camDesc));
	m_pMainCam = pVariousCamera;
	m_pGameInstance->Change_MainCamera(m_pMainCam);

	LIGHT_DESC			LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.f, 0.f);
	LightDesc.fLightStrength = 0.8f;
	
	m_pGameInstance->Set_DirectionalLight(LightDesc);

	m_pGameInstance->Set_AmbientStrength(0.65f);

	//LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	//LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vPosition = _float4(0.f, 3.f, 0.f, 1.f);
	//LightDesc.vDirection = _float4(1.f, -1.f, 0.f, 0.f);
	//LightDesc.fSpotPower = 10.f;
	//LightDesc.fRange = 10.f;
	//LightDesc.fLightStrength = 1.f;


	//if (FAILED(Initialize_IBL(L"UFO_Inside")))
	//	return E_FAIL;

	if (FAILED(Ready_Map()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	static_cast<CVarious_Camera*>(m_pMainCam)->Initial_Setting((_uint)VARCAM_STATES::STATE_PLAYER_APPEARING);

	if (FAILED(Ready_Monster()))
		return E_FAIL;

	return S_OK;
}

void CLevel_UFO_Inside::Tick(_float _fTimeDelta)
{
}

HRESULT CLevel_UFO_Inside::Render()
{
	return S_OK;
}

HRESULT CLevel_UFO_Inside::Ready_Map()
{
	CMapLoader::Get_Instance()->Load_Map(L"../../Resources/MapData/UFO_Inside.dat", m_iLevelID);

	return S_OK;
}

HRESULT CLevel_UFO_Inside::Ready_Player()
{
	PLAYER_BATTLE_CLONE_DESC Desc = {};
	Desc.strModelTag = L"Prototype_Model_Travis";

	if (nullptr == m_pGameInstance->Add_Clone(m_iLevelID, L"Layer_Player", L"Prototype_GameObject_Travis", &Desc))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_UFO_Inside::Ready_Monster()
{
	return S_OK;
}

CLevel_UFO_Inside* CLevel_UFO_Inside::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_UFO_Inside* pInstance = new CLevel_UFO_Inside(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_UFO_Inside"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_UFO_Inside::Free()
{
	__super::Free();

	Safe_Release(m_pMainCam);
	CMapLoader::Destroy_Instance();
}

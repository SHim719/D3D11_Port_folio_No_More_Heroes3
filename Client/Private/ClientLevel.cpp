#include "ClientLevel.h"
#include "Instancing_Object.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Effect_Manager.h"

#include "Cinematic_Manager.h"


CClientLevel::CClientLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

void CClientLevel::Tick(_float fTimeDelta)
{
	CINEMAMGR->Tick(fTimeDelta);

}

HRESULT CClientLevel::Initialize_IBL(const wstring& wstrTag)
{
	wstring wstrPrototypeTag = L"Prototype_Irradiance_" + wstrTag;
	m_IBLTextures.pIrradianceIBLTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(GET_CURLEVEL, wstrPrototypeTag));
	if (nullptr == m_IBLTextures.pIrradianceIBLTexture)
		return E_FAIL;

	wstrPrototypeTag = L"Prototype_SpecularIBL_" + wstrTag;
	m_IBLTextures.pSpecularIBLTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(GET_CURLEVEL, wstrPrototypeTag));
	if (nullptr == m_IBLTextures.pSpecularIBLTexture)
		return E_FAIL;

	wstrPrototypeTag = L"Prototype_BRDF_" + wstrTag;
	m_IBLTextures.pBRDFTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Component(GET_CURLEVEL, wstrPrototypeTag));
	if (nullptr == m_IBLTextures.pBRDFTexture)
		return E_FAIL;

	m_pGameInstance->Set_Active_IBL(true, &m_IBLTextures);

	return S_OK;
}

void CClientLevel::OnExitLevel()
{
}

void CClientLevel::Free()
{
	EFFECTMGR->Clear(m_iLevelID);
	__super::Free();
	Safe_Release(m_IBLTextures.pBRDFTexture);
	Safe_Release(m_IBLTextures.pIrradianceIBLTexture);
	Safe_Release(m_IBLTextures.pSpecularIBLTexture);
}


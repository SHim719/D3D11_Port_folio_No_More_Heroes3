#include "..\Public\Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_Lights.end())
		return nullptr;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(CLight* pLight)
{
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace_back(pLight);
	Safe_AddRef(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer* pVIBuffer, _bool bPBR)
{
	if (m_pDirectionalLight && m_bActiveDirectional)
		m_pDirectionalLight->Render(pShader, pVIBuffer, bPBR);

	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer, bPBR);

	Clear_Lights();

	return S_OK;
}


void CLight_Manager::Clear_Lights()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}

void CLight_Manager::Set_DirectionalLight(const LIGHT_DESC& LightDesc)
{
	Safe_Release(m_pDirectionalLight);

	m_pDirectionalLight = CLight::Create(LightDesc);
}

_vector CLight_Manager::Get_LightDir() const
{
	if (m_pDirectionalLight)
		return XMVector3Normalize(XMLoadFloat4(&m_pDirectionalLight->Get_LightDesc()->vDirection));
	else
		return XMVectorZero();
}


CLight_Manager* CLight_Manager::Create()
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDirectionalLight);

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}


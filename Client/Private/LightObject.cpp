#include "LightObject.h"


CLightObject::CLightObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLightObject::CLightObject(const CLightObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLightObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	LIGHT_DESC* pLightDesc = (LIGHT_DESC*)pArg;

	m_pLight = CLight::Create(*pLightDesc);
	if (nullptr == m_pLight)
		return E_FAIL;

	return S_OK;
}


void CLightObject::LateTick(_float fTimeDelta)
{
	if (LIGHT_DESC::TYPE_POINT == m_pLight->Get_LightDesc()->eType)
	{
		_vector vLightPos = XMLoadFloat4(&m_pLight->Get_LightDesc()->vPosition);
		_float fRange = m_pLight->Get_LightDesc()->fRange;

		if (false == m_pGameInstance->In_WorldFrustum(vLightPos, fRange))
			return;
	}

	m_pGameInstance->Add_Light(m_pLight);
}


CLightObject* CLightObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightObject* pInstance = new CLightObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLightObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightObject::Clone(void* pArg)
{
	CLightObject* pInstance = new CLightObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLightObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightObject::Free()
{
	__super::Free();

	Safe_Release(m_pLight);
}



#include "Various_Camera_State_Cinematic_Jo.h"

#include "GameInstance.h"

#include "Various_Camera.h"

#include "Cinematic_Manager.h"

CVarious_Camera_State_Cinematic_Jo::CVarious_Camera_State_Cinematic_Jo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVarious_Camera_State_Base{ _pDevice,_pContext }
{
}

HRESULT CVarious_Camera_State_Cinematic_Jo::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CVarious_Camera_State_Cinematic_Jo::EnterState(void* _pArg)
{
	__super::EnterState(nullptr);
}

void CVarious_Camera_State_Cinematic_Jo::Update(_float _fTimeDelta)
{
	m_pOwner->Play_Cinematic(CINEMAMGR->Get_PlayTimeAcc());
}

void CVarious_Camera_State_Cinematic_Jo::Late_Update(_float _fTimeDelta)
{
}

void CVarious_Camera_State_Cinematic_Jo::ExitState()
{
	__super::ExitState();

}

CVarious_Camera_State_Cinematic_Jo* CVarious_Camera_State_Cinematic_Jo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CVarious_Camera_State_Cinematic_Jo* pInstance = new CVarious_Camera_State_Cinematic_Jo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Create : CVarious_Camera_State_Cinematic_Jo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVarious_Camera_State_Cinematic_Jo::Free()
{
	__super::Free();

}

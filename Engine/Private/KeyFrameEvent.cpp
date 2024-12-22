#include "KeyFrameEvent.h"
#include "GameInstance.h"


CKeyFrameEvent::CKeyFrameEvent()
{
}

CKeyFrameEvent::CKeyFrameEvent(const CKeyFrameEvent& rhs)
	: m_strEventName(rhs.m_strEventName)
	, m_pGameInstance(CGameInstance::Get_Instance())
	, m_fVolume(rhs.m_fVolume)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CKeyFrameEvent::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	KEYFRAME_DESC* pDesc = (KEYFRAME_DESC*)pArg;

	m_eKeyFrameType = pDesc->eKeyFrameType;
	m_strEventName = pDesc->strEventName;
	m_fVolume = pDesc->fVolume;

    return S_OK;
}



void CKeyFrameEvent::Execute()
{
	if (m_CallBackFunc && NORMAL == m_eKeyFrameType)
		m_CallBackFunc();
	else
		m_pGameInstance->Play(m_strEventName, false, m_fVolume);

}

CKeyFrameEvent* CKeyFrameEvent::Create(void* pArg)
{
    CKeyFrameEvent* pInstance = new CKeyFrameEvent;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKeyFrameEvent"));
		Safe_Release(pInstance);
	}

    return pInstance;
}

CKeyFrameEvent* CKeyFrameEvent::Clone()
{
	CKeyFrameEvent* pInstance = new CKeyFrameEvent(*this);

	return pInstance;
}

void CKeyFrameEvent::Free()
{
	Safe_Release(m_pGameInstance);
}

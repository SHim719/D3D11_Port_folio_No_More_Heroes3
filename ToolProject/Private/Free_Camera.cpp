#include "Free_Camera.h"

CFree_Camera::CFree_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CFree_Camera::CFree_Camera(const CFree_Camera& rhs, CTransform::TRANSFORMDESC* pTransformArg)
	: CCamera(rhs, pTransformArg)
{
}


void CFree_Camera::OnActive()
{
	m_tPrevMousePos = m_tCurrentMousePos;
	GetCursorPos(&m_tCurrentMousePos);
	ScreenToClient(g_hWnd, &m_tCurrentMousePos);
}

void CFree_Camera::Tick(_float fTimeDelta)
{
	m_tPrevMousePos = m_tCurrentMousePos;
	GetCursorPos(&m_tCurrentMousePos);
	ScreenToClient(g_hWnd, &m_tCurrentMousePos);

	if (m_pGameInstance->GetKeyDown(eKeyCode::LShift))
	{
		m_pTransform->Set_Speed(20.f);
	}

	else if (m_pGameInstance->GetKeyNone(eKeyCode::LShift))
	{
		m_pTransform->Set_Speed(3.f);
	}

	if (m_pGameInstance->GetKey(eKeyCode::RButton))
	{
		if (m_pGameInstance->GetKey(eKeyCode::W))
		{
			if (!m_bGrid)
				m_pTransform->Go_Straight(fTimeDelta * 4.f);
		}

		if (m_pGameInstance->GetKey(eKeyCode::S))
		{
			if (!m_bGrid)
				m_pTransform->Go_Backward(fTimeDelta * 4.f);
		}

		if (m_pGameInstance->GetKey(eKeyCode::A))
		{
			m_pTransform->Go_Left(fTimeDelta * 4.f);
		}

		if (m_pGameInstance->GetKey(eKeyCode::D))
		{
			m_pTransform->Go_Right(fTimeDelta * 4.f);
		}

		if (m_pGameInstance->GetKey(eKeyCode::E))
		{
			m_pTransform->Go_Up(fTimeDelta * 4.f);
		}

		if (m_pGameInstance->GetKey(eKeyCode::Q))
		{
			m_pTransform->Go_Down(fTimeDelta * 4.f);
		}
		Rotation_By_Mouse();
	}
}

void CFree_Camera::LateTick(_float fTimeDelta)
{
	
}

void CFree_Camera::Rotation_By_Mouse()
{
	if (!m_bGrid)
	{
		m_pTransform->Add_PitchInput(To_Radian((m_tCurrentMousePos.y - m_tPrevMousePos.y) * 0.2f));
		m_pTransform->Add_YAxisInput(To_Radian((m_tCurrentMousePos.x - m_tPrevMousePos.x) * 0.2f));
	}
}

CFree_Camera* CFree_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFree_Camera* pInstance = new CFree_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFree_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFree_Camera::Clone(void* pArg)
{
	CTransform::TRANSFORMDESC* pTransformDesc = (CTransform::TRANSFORMDESC*)pArg;

	CFree_Camera* pInstance = new CFree_Camera(*this, pTransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		assert(false);
		MSG_BOX(TEXT("Failed To Cloned : CFree_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFree_Camera::Free()
{
	__super::Free();
}

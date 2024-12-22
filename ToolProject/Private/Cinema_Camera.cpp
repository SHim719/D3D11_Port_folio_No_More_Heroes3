#include "Cinema_Camera.h"

CCinema_Camera::CCinema_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCinema_Camera::CCinema_Camera(const CCinema_Camera& rhs, CTransform::TRANSFORMDESC* pTransformArg)
	: CCamera(rhs, pTransformArg)
{
}


void CCinema_Camera::OnActive()
{
	m_tPrevMousePos = m_tCurrentMousePos;
	GetCursorPos(&m_tCurrentMousePos);
	ScreenToClient(g_hWnd, &m_tCurrentMousePos);
}

void CCinema_Camera::Tick(_float fTimeDelta)
{
	m_tPrevMousePos = m_tCurrentMousePos;
	GetCursorPos(&m_tCurrentMousePos);
	ScreenToClient(g_hWnd, &m_tCurrentMousePos);

	if (false == m_bPlaying)
	{
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
				m_pTransform->Go_Straight(fTimeDelta * 4.f);
			}

			if (m_pGameInstance->GetKey(eKeyCode::S))
			{
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
	else
	{
		if (LookFrameIt != m_CinemaDesc.Looks.end())
		{
			TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(LookFrameIt);

			if (NextIter != m_CinemaDesc.Looks.end() && true == NextIter->second.second)
			{
				_float fRatio = (m_fPlayTimeAcc - LookFrameIt->first) / (NextIter->first - LookFrameIt->first);

				_vector vLerpedQuatByLook = JoMath::Slerp_TargetLook(XMLoadFloat3(&LookFrameIt->second.first), XMLoadFloat3(&NextIter->second.first), fRatio);

				m_pTransform->Rotation_Quaternion(vLerpedQuatByLook);
			}
			else
			{
				_vector vLook = XMLoadFloat3(&LookFrameIt->second.first);

				m_pTransform->LookTo(vLook);
			}
		}
		

		if (PositionFrameIt != m_CinemaDesc.Positions.end())
		{
			TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER NextIter = std::next(PositionFrameIt);

			if (NextIter != m_CinemaDesc.Positions.end() && true == NextIter->second.second)
			{
				_float fRatio = (m_fPlayTimeAcc - PositionFrameIt->first) / (NextIter->first - PositionFrameIt->first);

				_vector vLerpedPosition = XMVectorLerp(XMLoadFloat3(&PositionFrameIt->second.first), XMLoadFloat3(&NextIter->second.first), fRatio);
				vLerpedPosition.m128_f32[3] = 1.f;

				m_pTransform->Set_Position(vLerpedPosition);
			}
			else
			{
				_vector vPosition = XMLoadFloat3(&PositionFrameIt->second.first);
				vPosition.m128_f32[3] = 1.f;

				m_pTransform->Set_Position(vPosition);
			}
		}
	}
}

void CCinema_Camera::LateTick(_float fTimeDelta)
{

}

void CCinema_Camera::Update_PlayTime(_float fPlayTime)
{
	for (LookFrameIt = m_CinemaDesc.Looks.begin(); LookFrameIt != m_CinemaDesc.Looks.end(); ++LookFrameIt)
	{
		_float fKeyPlayTime = LookFrameIt->first;
		if (fPlayTime < fKeyPlayTime)
		{
			if (LookFrameIt != m_CinemaDesc.Looks.begin())
				--LookFrameIt;
			break;
		}
	}

	if (m_CinemaDesc.Looks.end() == LookFrameIt && m_CinemaDesc.Looks.begin() != LookFrameIt)
		LookFrameIt = std::prev(LookFrameIt);


	for (PositionFrameIt = m_CinemaDesc.Positions.begin(); PositionFrameIt != m_CinemaDesc.Positions.end(); ++PositionFrameIt)
	{
		_float fKeyPlayTime = PositionFrameIt->first;
		if (fPlayTime < fKeyPlayTime)
		{
			if (PositionFrameIt != m_CinemaDesc.Positions.begin())
				--PositionFrameIt;
			break;
		}
	}

	if (m_CinemaDesc.Positions.end() == PositionFrameIt && m_CinemaDesc.Positions.begin() != PositionFrameIt)
		PositionFrameIt = std::prev(PositionFrameIt);

	for (EventFrameIt = m_CinemaDesc.Events.begin(); EventFrameIt != m_CinemaDesc.Events.end(); ++EventFrameIt)
	{
		_float fKeyPlayTime = EventFrameIt->first;
		if (fPlayTime < fKeyPlayTime)
		{
			if (EventFrameIt != m_CinemaDesc.Events.begin())
				--EventFrameIt;
			break;
		}
	}

	if (m_CinemaDesc.Events.end() == EventFrameIt && m_CinemaDesc.Events.begin() != EventFrameIt)
		EventFrameIt = std::prev(EventFrameIt);

	m_fPlayTimeAcc = fPlayTime;
}

void CCinema_Camera::Rotation_By_Mouse()
{
	m_pTransform->Add_PitchInput(To_Radian((m_tCurrentMousePos.y - m_tPrevMousePos.y) * 0.2f));
	m_pTransform->Add_YAxisInput(To_Radian((m_tCurrentMousePos.x - m_tPrevMousePos.x) * 0.2f));
}

CCinema_Camera* CCinema_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinema_Camera* pInstance = new CCinema_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCinema_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCinema_Camera::Clone(void* pArg)
{
	CTransform::TRANSFORMDESC* pTransformDesc = (CTransform::TRANSFORMDESC*)pArg;

	CCinema_Camera* pInstance = new CCinema_Camera(*this, pTransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		assert(false);
		MSG_BOX(TEXT("Failed To Cloned : CCinema_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Camera::Free()
{
	__super::Free();
}

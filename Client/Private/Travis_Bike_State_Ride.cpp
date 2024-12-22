#include "Travis_Bike_State_Ride.h"
#include "Bone.h"

#include "Travis_World.h"
#include "UI_Manager.h"
#include "UI_World_City.h"

CTravis_Bike_State_Ride::CTravis_Bike_State_Ride(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTravis_Bike_State_Base(_pDevice, _pContext)
{
}

HRESULT CTravis_Bike_State_Ride::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_pRigidbody = static_cast<CRigidbody*>(m_pOwner->Find_Component(L"Rigidbody"));
	if (nullptr == m_pRigidbody)
		return E_FAIL;

	m_pBone_Tire_R = m_pModel->Get_Bone("rtire_r");
	if (nullptr == m_pBone_Tire_R)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_R);

	m_pBone_Tire_L = m_pModel->Get_Bone("rtire_l");
	if (nullptr == m_pBone_Tire_L)
		return E_FAIL;
	Safe_AddRef(m_pBone_Tire_L);

	m_fCurRotAccel = m_fOriginRotAccel;

	return S_OK;
}

void CTravis_Bike_State_Ride::OnState_Start(void* _pArg)
{
	m_pModel->Change_Animation(ANI_PL_Vehicle_MOV_Run);
}

void CTravis_Bike_State_Ride::Update(_float _fTimeDelta)
{
	if (GET_PLAYER_WORLD->Get_ControlLock())
		return;

	if (KEY_PUSHING(eKeyCode::W) && false == m_bDrifting)
	{
		if (!m_bAccel)
		{
			PLAY_SOUND("SE_enCar_drivin_01", true, 1.f);
			m_bAccel = true;
		}

		m_pRigidbody->Add_Force(XMVectorSet(0.f, 0.f, m_fAccel, 0.f), CRigidbody::FORCEMODE::Force);
	}
	else
	{
		STOP_SOUND("SE_enCar_drivin_01");
		m_bAccel = false;
	}

	if (KEY_PUSHING(eKeyCode::S) && false == m_bDrifting)
	{
		m_pRigidbody->Add_Force(XMVectorSet(0.f, 0.f, -m_fAccel, 0.f), CRigidbody::FORCEMODE::Force);
	}

	if (KEY_PUSHING(eKeyCode::D))
	{
		if (m_eCurDir != RIGHT)
		{
			m_eCurDir = RIGHT;
			m_pModel->Change_Animation(ANI_PL_Vehicle_MOV_Turn_R);
		}

		m_pRigidbody->Add_Torque(YAXIS, m_fCurRotAccel, CRigidbody::FORCEMODE::Force);
	}

	if (KEY_PUSHING(eKeyCode::A))
	{
		if (m_eCurDir != LEFT)
		{
			m_eCurDir = LEFT;
			m_pModel->Change_Animation(ANI_PL_Vehicle_MOV_Turn_L);
		}
		
		m_pRigidbody->Add_Torque(YAXIS, -m_fCurRotAccel, CRigidbody::FORCEMODE::Force);
	}

	if (KEY_NONE(eKeyCode::A) && KEY_NONE(eKeyCode::D) && (LEFT == m_eCurDir || RIGHT == m_eCurDir))
	{
		m_pModel->Change_Animation(ANI_PL_Vehicle_MOV_Run);
		m_eCurDir = NEUTRAL;
	}

	if (KEY_DOWN(eKeyCode::LShift))
	{
		m_bDrifting = true;
		m_fCurRotAccel = m_fOriginRotAccel * 3.f;
		m_pRigidbody->Set_MaxAngularVelocity(180.f);
	}
	else if (KEY_UP(eKeyCode::LShift))
	{
		m_bDrifting = false;
		m_fCurRotAccel = m_fOriginRotAccel;
		m_pRigidbody->Set_MaxAngularVelocity(60.f);
	}

	if (KEY_DOWN(eKeyCode::Space))
	{
		ADD_EVENT(bind(&CTravis_Bike::Get_Off, m_pOwner));
	}

	static_cast<CUI_World_City*>(UIMGR->GetScene(L"SCENE_World_City"))->Set_Gauge_Speed(fabsf(XMVectorGetZ(m_pRigidbody->Get_Velocity()))
		/ XMVectorGetZ(m_pRigidbody->Get_MaxVelocity()));
}

void CTravis_Bike_State_Ride::Late_Update(_float _fTimeDelta)
{
	Rotate_Tire(_fTimeDelta);
	
}

void CTravis_Bike_State_Ride::OnState_End()
{
}

void CTravis_Bike_State_Ride::Rotate_Tire(_float _fTimeDelta)
{
	_float fForwardSpeed = XMVectorGetZ(m_pRigidbody->Get_Velocity());
	_float fMaxSpeed = XMVectorGetZ(m_pRigidbody->Get_MaxVelocity());
	_float fRotationSpeed = JoMath::Lerp(0.f, m_fMaxTireRotSpeed, fabsf(fForwardSpeed / fMaxSpeed));
	_float fSign = fForwardSpeed == 0.f ? 0.f : -fForwardSpeed / fForwardSpeed;

	m_fTireRotation += _fTimeDelta * fRotationSpeed;

	if (fForwardSpeed != 0.f)
	{
		m_pBone_Tire_R->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation * fSign)));
		m_pBone_Tire_L->Set_ExtraOffset(XMMatrixRotationX(To_Radian(m_fTireRotation * fSign)));
	}
}

CTravis_Bike_State_Ride* CTravis_Bike_State_Ride::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	CTravis_Bike_State_Ride* pInstance = new CTravis_Bike_State_Ride(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX(L"Failed to Create : CTravis_Bike_State_Ride");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTravis_Bike_State_Ride::Free()
{
	__super::Free();

	Safe_Release(m_pBone_Tire_R);
	Safe_Release(m_pBone_Tire_L);
}

#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Various_Camera_Enums.h"
#include "Various_Camera_States.h"
#include "Various_Camera_Structs.h"
#include "GameEffect.h"
BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CVarious_Camera final : public CCamera
{
public:
	typedef struct tagVariousCameraDesc : public CCamera::CAMERADESC {
		CGameObject* pOwner = { nullptr };

	}VARCAM_DESC;

private:
	CVarious_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVarious_Camera(const CVarious_Camera& _rhs, CTransform::TRANSFORMDESC* _pArg);
	virtual ~CVarious_Camera() = default;

public:
	virtual HRESULT Initialize_Prototype()			override;
	virtual HRESULT Initialize(void* _pArg)			override;
	virtual void	Tick(_float _fTimeDelta)		override;
	virtual void	LateTick(_float _fTimeDelta)	override;
	virtual HRESULT Render()						override;

	virtual void Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs);
public:
	virtual void Update_ViewProj()					override;
	virtual void OnActive()							override;

private:
	CNavigation* m_pNavigation = { nullptr };

public:
	void Initial_Setting(_uint _iStateIndex, void* _pArg = nullptr);
	void Set_Navigation(_uint _iLevelIndex, const wstring& _strNavigationTag);
	void Set_NoNavigation();

private:
	CGameObject* m_pOwner = { nullptr };
	CVarious_Camera_State_Base* m_States[(_uint)VARCAM_STATES::STATES_END] = {};
	_uint m_iCurStateIndex = { 0 };


private:
	HRESULT Ready_States();

private:
	void Rolling(_float _fTimeDelta);
	void VerticalShaking(_float _fTimeDelta);
	void ZoomInOut(_float _fTimeDelta);

public:
	void Change_State(_uint _iStateIndex, void* _pArg = nullptr);
	void Start_Rolling(_float _fRollingPower, _float _fRollingDuration, _uint _iRollingCount);
	void Start_VerticalShaking(_float _fShakingPower, _float _fShakingDuration, _uint _iShakingCount);
	void Start_ZoomIO(_float _fStartFov, _float _fEndFov, _float _fIODuration);
	void Finish_Rolling();
	void Finish_VerticalShaking();
	void Finish_ZoomIO();
	void Bind_Triggers(_uint _iLevelIndex);
	void Insert_Cinematic_Scenario(const vector<VARCAM_CINEMATIC_DESC>& _vecScenario) { m_vecCinematicDescs = _vecScenario;}
	const vector< VARCAM_CINEMATIC_DESC>& Get_Cinematic_Scenario() { return m_vecCinematicDescs; }
	void Clear_Cinematic_Scenario() { m_vecCinematicDescs.clear(); }

private:
	_float4x4 m_WorldMatrix_Origin = {};
	vector<VARCAM_CINEMATIC_DESC> m_vecCinematicDescs;

	_float	m_fShakingPower		= { 0.f };
	_float	m_fShakingDuration	= { 0.f };
	_float	m_fTimeAcc_Shake	= { 0.f };
	_uint	m_iShakingCount		= { 1 };
	_bool	m_bShaking			= { false };

	_float	m_fRollingPower		= { 0.f };
	_float	m_fRollingDuration	= { 0.f };
	_float	m_fTimeAcc_Roll		= { 0.f };
	_uint	m_iRollingCount		= { 1 };
	_bool	m_bRolling			= { false };

	_float	m_fFov_Origin		= { 0.f };
	_float  m_fFov_Dest			= { 0.f };
	_float	m_fZoomIODuration = { 0.f };
	_float	m_fTimeAcc_ZoomIO	= { 0.f };
	_bool	m_bZoomIO			= { false };

// Ground
private:
	_bool m_bLockOn = { false };
	_bool m_bHold = { false };

public:
	void Set_LockOn(_bool _bVal) { m_bLockOn = _bVal; }
	void Set_Hold(_bool _bVal) { m_bHold = _bVal; }

public:
	_bool Get_Hold() const { return m_bHold; }
	CGameObject* Get_CameraOwner() { return m_pOwner; }
	_uint Get_CurStateIndex() const { return m_iCurStateIndex; }

// Space
private:


public:
	static CVarious_Camera* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free()	override;

};

END
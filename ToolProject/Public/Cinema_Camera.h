#pragma once

#include "Tool_Defines.h"
#include "Camera.h"


BEGIN(Tool)

class CCinema_Camera final : public CCamera
{
private:
	CCinema_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinema_Camera(const CCinema_Camera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCinema_Camera() = default;

public:
	void OnActive()						override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

private:
	POINT m_tPrevMousePos = {};
	POINT m_tCurrentMousePos = { -1, -1 };

private:
	TOOL_CINEMATIC_DESCS m_CinemaDesc;
	_bool			m_bPlaying = { false };
	_float			m_fPlayTimeAcc = { 0.f };

	TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER PositionFrameIt;
	TOOL_CINEMATIC_DESCS::CINE_FRAMES_ITER LookFrameIt;
	TOOL_CINEMATIC_DESCS::CINE_EVENTS_ITER EventFrameIt; // 이벤트를 순회하기위한 반복자

public:
	TOOL_CINEMATIC_DESCS& Get_CinemaDesc() {
		return m_CinemaDesc;
	}

	void Set_CinemaDesc(const TOOL_CINEMATIC_DESCS& CinemaDesc) {
		m_CinemaDesc = CinemaDesc;
	}


	void Play_Cinematic() {
		m_bPlaying = true;
	}
	
	void End_Cinematic() {
		m_bPlaying = false;
	}

	void Update_PlayTime(_float fPlayTime);

private:
	void Rotation_By_Mouse();

public:
	static CCinema_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free()	override;
};

END


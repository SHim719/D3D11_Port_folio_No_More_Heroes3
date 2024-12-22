#pragma once

/* Ŭ�󿡼� ������ ī�޶���� �θ�. */
/* ��� ī�޶� �������� �⺻���� ����� �����Ѵ�. */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float4		vEye, vAt;
		_float		fFovy, fAspect, fNear, fFar;

		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMERADESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

public:
	virtual void Update_ViewProj();
	void Update_Cascade();

protected:
	CAMERADESC					m_CameraDesc;

public:
	const CAMERADESC& Get_CameraDesc() const {
		return m_CameraDesc;
	}

	_float	Get_FovY() const { return m_CameraDesc.fFovy; }
	void	Set_FovY(_float fFovDegree) { m_CameraDesc.fFovy = fFovDegree; }


private:
	_float		m_fCascadeEnd[4] = {};
	_float3		m_PrevShadowCamPos[3] = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
};

END
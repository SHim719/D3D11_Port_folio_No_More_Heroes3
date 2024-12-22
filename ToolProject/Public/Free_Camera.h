#pragma once

#include "Tool_Defines.h"
#include "Camera.h"


BEGIN(Tool)

class CFree_Camera final : public CCamera
{
protected:
	CFree_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFree_Camera(const CFree_Camera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CFree_Camera() = default;

public:
	void OnActive()						override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

public:
	void Set_Grid(_bool _bGrid) { m_bGrid = _bGrid; }

private:
	POINT m_tPrevMousePos = {};
	POINT m_tCurrentMousePos = { -1, -1 };

	_bool m_bGrid = false;

private:
	void Rotation_By_Mouse();

public:
	static CFree_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free()	override;
};

END


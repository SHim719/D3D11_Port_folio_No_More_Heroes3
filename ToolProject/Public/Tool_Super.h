#pragma once

#include "Tool_Defines.h"
#include "Base.h"

#include "GameObject.h"
#include "Free_Camera.h"

BEGIN(Tool)

class CTool_Super : public CBase
{
protected:
	CTool_Super();
	virtual ~CTool_Super() = default;

public:
	virtual HRESULT Initialize();
	virtual void Start_Tool();
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

protected:
	CGameInstance*			m_pGameInstance = nullptr;
	CFree_Camera*			m_pCamera = nullptr;

protected:
	void Camera_Window();

public:
	void Free() override;

};

END


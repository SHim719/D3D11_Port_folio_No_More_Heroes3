#pragma once

#include "Character.h"

BEGIN(Client)

class CTravis_AW final : public CCharacter
{
private:
	CTravis_AW(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTravis_AW(const CTravis_AW& _rhs);
	virtual ~CTravis_AW() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* _pArg)				override;
	void	PriorityTick(_float _fTimeDelta)	override;
	void	Tick(_float _fTimeDelta)			override;
	void	LateTick(_float _fTimeDelta)		override;
	HRESULT Render()							override;

private:
	HRESULT Ready_Components();

public:
	static CTravis_AW* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END


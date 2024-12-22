#pragma once

#include "Client_Defines.h"
#include "Character.h"


BEGIN(Client)

class CCinemaActor : public CCharacter
{
protected:
	CCinemaActor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCinemaActor(const CCinemaActor& _rhs);
	virtual ~CCinemaActor() = default;

public:
	HRESULT Initialize(void* _pArg)				override;
	void	PriorityTick(_float _fTimeDelta)	override;
	void	Tick(_float _fTimeDelta)			override;
	HRESULT Render()							override;

	void Set_CinemaActorTag(const string& strActorTag) {
		m_strCinemaActorTag = strActorTag; 
	}

protected:
	virtual _bool Bind_CinemaEtcFunc();


private:
	HRESULT Ready_Components();


public:
	static CCinemaActor* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END
#pragma once

#include "Character.h"

BEGIN(Client)

class CTravis_Ending : public CCharacter
{
private:
	CTravis_Ending(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTravis_Ending(const CTravis_Ending& _rhs);
	virtual ~CTravis_Ending() = default;

private:
	HRESULT		Initialize(void* _pArg)		override;
	void		Tick(_float _fTimeDelta)	override;
	HRESULT		Render()					override;

public:
	static CTravis_Ending* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END
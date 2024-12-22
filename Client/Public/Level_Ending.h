#pragma once

#include "ClientLevel.h"

BEGIN(Client)

class CLevel_Ending final : public CClientLevel
{
public:
	enum ENDING_STATE { LOGO, ING, LAST_JEANE};

private:
	CLevel_Ending(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Ending() = default;

public:
	virtual HRESULT Initialize()				override;
	virtual void	Tick(_float _fTimeDelta)	override;
	virtual HRESULT Render()					override;
	virtual void	OnExitLevel()				override;

private:
	class CFree_Camera*		m_pCamera = { nullptr };
	class CJeane_Ending*	m_pLastJeane = { nullptr };

	ENDING_STATE			m_eEndingState = { LOGO };

	_float					m_fTimeAcc = { 0.f };
	_float					m_fLogoTime = { 8.f };

private:
	void Setting_EndingScene();



public:
	static CLevel_Ending* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END
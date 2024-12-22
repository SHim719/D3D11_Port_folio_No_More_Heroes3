#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCreditText final : public CGameObject
{
private:
	CCreditText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreditText(const CCreditText& rhs);
	virtual ~CCreditText() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	wstring		m_wstrCreditText = { L"" };

	_float2		m_vTextPos = { 900.f, 720.f };
	
	_float		m_fCreditSpeed = { 60.f };


public:
	static CCreditText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END
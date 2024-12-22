#pragma once

#include "UI.h"



BEGIN(Client)

class CFadeScreen final : public CUI
{
public:
	enum FADECOLOR { BLACK, WHITE, COLOR_END};
	enum FADESTATE { FADEOUT, EXTRA, FADEIN};

public:
	typedef struct tagFadeDesc
	{
		FADECOLOR			eFadeColor;
		_float				fFadeOutSpeed;
		_float				fFadeInSpeed;
		_float				fExtraTime = 0.f;
		function<void()>	pCallback_FadeOutStart = nullptr;
		function<void()>	pCallback_FadeOutEnd =  nullptr;
		function<void()>	pCallback_FadeInStart = nullptr;
		function<void()>	pCallback_FadeInEnd = nullptr;
	}FADEDESC;	

private:
	CFadeScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFadeScreen(const CFadeScreen& rhs);
	virtual ~CFadeScreen() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

public:
	HRESULT OnEnter_Layer(void* pArg)	override;

private:
	CVIBuffer_Point* m_pVIBuffer_Point = { nullptr };

private:
	_float		m_fAlpha = { 0.f };
	_float		m_fFadeOutSpeed = { 0.f };
	_float		m_fFadeInSpeed = { 0.f };
	_float		m_fExtraTime = { 0.f };
	_bool		m_bComplete = { false };

	FADESTATE	m_eFadeState = { FADEOUT };

	_float4		m_vFadeColor = {};

	function<void()>	m_Callback_FadeOutStart = { nullptr };
	function<void()>	m_Callback_FadeOutEnd = { nullptr };
	function<void()>	m_Callback_FadeInStart = { nullptr };
	function<void()>	m_Callback_FadeInEnd = { nullptr };

public:
	static CFadeScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END


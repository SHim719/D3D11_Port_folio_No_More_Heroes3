#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{	
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Initialize(const GRAPHIC_DESC& GraphicDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

private:
	ID3D11Device*				m_pDevice		 = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	IDXGISwapChain*				m_pSwapChain	 = nullptr;
											
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11ShaderResourceView*	m_pBackBufferSRV = nullptr;
											 
	ID3D11Texture2D*			m_pDSBuffer		 = nullptr;
	ID3D11DepthStencilView*		m_pDSView		 = nullptr;

	_uint						m_iNumQualityLevel;

public:
	ID3D11ShaderResourceView* Get_BackBufferSRV() const {
		return m_pBackBufferSRV;
	}

private:
	HRESULT Init_Device_And_SwapChain(const GRAPHIC_DESC& GraphicDesc);
	HRESULT Init_RTV();
	HRESULT Init_DepthStencil(const GRAPHIC_DESC& GraphicDesc);

public:
	static CGraphic_Device* Create(const GRAPHIC_DESC& GraphicDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

END
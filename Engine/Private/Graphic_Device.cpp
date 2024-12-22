#include "../Public/Graphic_Device.h"
#include "debugapi.h"

CGraphic_Device::CGraphic_Device()
{

}


HRESULT CGraphic_Device::Initialize(const GRAPHIC_DESC& GraphicDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppDeviceContextOut)
{
	if (FAILED(Init_Device_And_SwapChain(GraphicDesc)))
		assert(false);

	if (FAILED(Init_RTV()))
		assert(false);

	if (FAILED(Init_DepthStencil(GraphicDesc)))
		assert(false);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDSView);

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)GraphicDesc.iWinSizeX;
	ViewPortDesc.Height = (_float)GraphicDesc.iWinSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDeviceOut = m_pDevice;
	*ppDeviceContextOut = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::Init_Device_And_SwapChain(const GRAPHIC_DESC& GraphicDesc)
{
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL featureLevel = {};
	UINT iCreateDeviceFlag = 0;
//#ifdef _DEBUG 
//	iCreateDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
//#endif

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = GraphicDesc.iWinSizeX;
	swapChainDesc.BufferDesc.Height = GraphicDesc.iWinSizeY;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 144;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapChainDesc.OutputWindow = GraphicDesc.hWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, driverType, 0, iCreateDeviceFlag, featureLevels, 1,
		D3D11_SDK_VERSION, &swapChainDesc
		, &m_pSwapChain
		, &m_pDevice
		, &featureLevel
		, &m_pDeviceContext)))
		return E_FAIL;


	m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_iNumQualityLevel);
	assert(m_iNumQualityLevel > 0);

	return S_OK;
}

HRESULT CGraphic_Device::Init_RTV()
{
	ID3D11Texture2D* pBackBuffer;

	if (FAILED(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pBackBuffer, nullptr, &m_pBackBufferSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGraphic_Device::Init_DepthStencil(const GRAPHIC_DESC& GraphicDesc)
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
	depthStencilBufferDesc.Width = GraphicDesc.iWinSizeX;
	depthStencilBufferDesc.Height = GraphicDesc.iWinSizeY;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &m_pDSBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDSBuffer, nullptr, &m_pDSView)))
		return E_FAIL;

	return S_OK;
}

CGraphic_Device* CGraphic_Device::Create(const GRAPHIC_DESC& GraphicDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppDeviceContextOut)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(GraphicDesc, ppDeviceOut, ppDeviceContextOut)))
	{
		MSG_BOX(TEXT("Failed to Created : CGraphic_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
//#if defined(DEBUG) || defined(_DEBUG)
//	ID3D11Debug* d3dDebug;
//	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
//	if (SUCCEEDED(hr))
//	{
//		OutputDebugString(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugString(L"                                                                    D3D11 Live Object ref Count Checker \r ");
//		OutputDebugString(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//
//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
//
//		OutputDebugString(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugString(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
//		OutputDebugString(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//	}
//	if (d3dDebug != nullptr)            d3dDebug->Release();
//#endif

	Safe_Release(m_pSwapChain);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDSBuffer);
	Safe_Release(m_pDSView);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}

#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"


CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(void* pArg)
{
	RT_DESC* pRTDesc = (RT_DESC*)pArg;

	D3D11_TEXTURE2D_DESC TextureDesc = {};

	TextureDesc.Width = pRTDesc->iWidth;
	TextureDesc.Height = pRTDesc->iHeight;
	TextureDesc.Format = pRTDesc->eFormat;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;// | D3D11_BIND_UNORDERED_ACCESS;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pSRV)))
		return E_FAIL;

	//if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture, nullptr, &m_pUAV)))
	//	return E_FAIL;

	m_vClearColor = pRTDesc->vClearColor;

	return S_OK;
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
}

HRESULT CRenderTarget::Bind_ShaderResourceView(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_ShaderResourceView(pConstantName, m_pSRV);
}

HRESULT CRenderTarget::Copy(ID3D11Texture2D* pResource)
{
	m_pContext->CopyResource(pResource, m_pTexture);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_DEBUG(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_matrix WorldMatrix = XMMatrixIdentity();

	WorldMatrix.r[0].m128_f32[0] = fSizeX;
	WorldMatrix.r[1].m128_f32[1] = fSizeY;

	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	WorldMatrix.r[3].m128_f32[0] = fX - ViewportDesc.Width * 0.5f;
	WorldMatrix.r[3].m128_f32[1] = -fY + ViewportDesc.Height * 0.5f;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(WorldMatrix));

	return S_OK;
}

HRESULT CRenderTarget::Render_DEBUG(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	/* 월드행렬을 따로 셋팅하는 이유 : 렌더타겟을 확인해뵉위한 디버긍요 사각형 버퍼를 각기 다른 위치에 다른 사이즈로 직교투영햐아여 그려주기위해ㅓㅅ. */
	if (FAILED(pShader->Bind_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	/* 렌더타겟에 뭐가 그려지는 지를 확인하기위해서 레[ㄴ터타겟텍스쳐를 쉐이더로 던진다. */
	if (FAILED(pShader->Bind_ShaderResourceView("g_Texture", m_pSRV)))
		return E_FAIL;

	if (FAILED(pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(L"Failed to Created : CRenderTarget");
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

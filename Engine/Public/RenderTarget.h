#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CRenderTarget : public CBase
{
public:
	typedef struct tagRenderTargetDesc
	{
		_uint		iWidth = 0;
		_uint		iHeight = 0;
		DXGI_FORMAT eFormat = DXGI_FORMAT_UNKNOWN;
		_float4		vClearColor = {};
	}RT_DESC;

private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(void* pArg);
	void Clear();
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pConstantName);
	HRESULT Copy(ID3D11Texture2D* pResource);

#ifdef _DEBUG
public:
	HRESULT Ready_DEBUG(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_DEBUG(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	ID3D11Texture2D*			m_pTexture = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11UnorderedAccessView*	m_pUAV = { nullptr };

	_float4						m_vClearColor = {};

#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;

};
END

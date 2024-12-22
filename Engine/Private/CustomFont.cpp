#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CCustomFont::Render(const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRotation, _float fScale, Align eAlign)
{
	_vector size = m_pFont->MeasureString(strText.c_str()) * fScale;
	_float2 fPos = { 0.f, 0.f };
	
	if (eAlign == FontAlignLeft)
		fPos = vPosition;
	if (eAlign == FontAlignCenter)
		fPos = { vPosition.x - XMVectorGetX(size) * 0.5f, vPosition.y };
	if (eAlign == FontAlignRight)
		fPos = { vPosition.x - XMVectorGetX(size), vPosition.y };


	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), fPos, vColor, fRotation, _float2(0.f, 0.f), fScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CCustomFont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCustomFont::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

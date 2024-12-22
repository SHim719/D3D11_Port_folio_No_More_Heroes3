#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_NorTex final : public CVIBuffer
{
private:
	CVIBuffer_Rect_NorTex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_NorTex(const CVIBuffer_Rect_NorTex& rhs);
	virtual ~CVIBuffer_Rect_NorTex() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_Rect_NorTex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END
#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Screen final : public CVIBuffer
{
private:
	CVIBuffer_Screen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Screen(const CVIBuffer_Screen& rhs);
	virtual ~CVIBuffer_Screen() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_Screen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END
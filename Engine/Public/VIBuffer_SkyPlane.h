#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_SkyPlane final : public CVIBuffer
{
private:
	CVIBuffer_SkyPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_SkyPlane(const CVIBuffer_SkyPlane& rhs);
	virtual ~CVIBuffer_SkyPlane() = default;

public:
	HRESULT Initialize_Prototype(_uint iPlaneResolution, _float fPlaneWidth, _float fPlaneTop, _float fPlaneBottom, _uint iTextureRepeat);

public:
	static CVIBuffer_SkyPlane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		_uint iPlaneResolution, _float fPlaneWidth, _float fPlaneTop, _float fPlaneBottom, _uint iTextureRepeat);
	CComponent* Clone(void* pArg) override;
	void Free() override;
};

END
#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sphere final : public CVIBuffer
{
private:
	CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs);
	virtual ~CVIBuffer_Sphere() = default;

public:
	HRESULT Initialize_Prototype(const _uint iNumStacks, const _uint iNumSlices);
	HRESULT Initialize(void* pArg)	override;

public:
	static CVIBuffer_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint iNumStacks, const _uint iNumSlices);
	CComponent* Clone(void* pArg)	override;
	void Free()	override;
};

END
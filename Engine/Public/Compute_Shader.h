#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CCompute_Shader final : public CComponent
{
private:
	CCompute_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCompute_Shader(const CCompute_Shader& rhs);
	virtual ~CCompute_Shader() = default;

public:
	HRESULT Initialize_Prototype(const wstring& strShaderFilePath);
	HRESULT Initialize(void* pArg)	override;

	void Dispatch(_uint x, _uint y, _uint z);

	void Set_ConstantBuffer(_uint iStartNum, _uint iArrayNum, ID3D11Buffer** ppBuffer);
	void Set_ShaderResource(_uint iStartNum, _uint iArrayNum, ID3D11ShaderResourceView** ppSRV);
	void Set_UnorderedAccess(_uint iStartNum, _uint iArrayNum, ID3D11UnorderedAccessView** ppUAV);
	
private:
	ID3D11ComputeShader* m_pCompute_Shader = { nullptr };


public:
	static CCompute_Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath );
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
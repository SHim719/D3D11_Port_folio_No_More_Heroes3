#include "Compute_Shader.h"
#include <comdef.h>
CCompute_Shader::CCompute_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent{ pDevice, pContext }
{
}

CCompute_Shader::CCompute_Shader(const CCompute_Shader& rhs)
	:CComponent(rhs)
	, m_pCompute_Shader{ rhs.m_pCompute_Shader }
{
	Safe_AddRef(m_pCompute_Shader);
}

HRESULT CCompute_Shader::Initialize_Prototype(const wstring& strShaderFilePath)
{

	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pCsBlob = nullptr;
	UINT iFlag = { 0 };

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;
#else 
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif	

	
	if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"CS_MAIN", "cs_5_0", iFlag, 0, &pCsBlob, &pErrorBlob)))
	{
		if (pErrorBlob)
		{
			_char* errorMessage = static_cast<_char*>(pErrorBlob->GetBufferPointer());
			MSG_BOX_CStr(errorMessage);
			assert(false);
			pErrorBlob->Release();
		}
		return E_FAIL;
	}


	HRESULT HR = m_pDevice->CreateComputeShader(pCsBlob->GetBufferPointer(), pCsBlob->GetBufferSize(), nullptr, &m_pCompute_Shader);
	if(FAILED(HR))
	{
		DWORD error = GetLastError();
		// HRESULT를 문자열로 변환하여 확인
		_com_error err(HR);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugString(errMsg);
	}



	return S_OK;
}

HRESULT CCompute_Shader::Initialize(void* pArg)
{
	return S_OK;
}

void CCompute_Shader::Dispatch(_uint x, _uint y, _uint z)
{
	m_pContext->CSSetShader(m_pCompute_Shader, nullptr, 0);
	m_pContext->Dispatch(x, y, z);
	m_pContext->CSSetShader(nullptr, nullptr, 0);		// Shader Unbinding
}

void CCompute_Shader::Set_ConstantBuffer(_uint iStartNum, _uint iArrayNum, ID3D11Buffer** ppBuffer)
{
	m_pContext->CSSetConstantBuffers(iStartNum, iArrayNum, ppBuffer);
}

void CCompute_Shader::Set_ShaderResource(_uint iStartNum, _uint iArrayNum, ID3D11ShaderResourceView** ppSRV)
{
	m_pContext->CSSetShaderResources(iStartNum, iArrayNum, ppSRV);
}

void CCompute_Shader::Set_UnorderedAccess(_uint iStartNum, _uint iArrayNum, ID3D11UnorderedAccessView** ppUAV)
{
	m_pContext->CSSetUnorderedAccessViews(iStartNum, iArrayNum, ppUAV, nullptr);
}

CCompute_Shader* CCompute_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath)
{
	CCompute_Shader* pInstance = new CCompute_Shader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath)))
	{
		assert(false);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCompute_Shader::Clone(void* pArg)
{
	CCompute_Shader* pInstance = new CCompute_Shader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		assert(false);
		MSG_BOX(TEXT("Failed To Cloned : CCompute_Shader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCompute_Shader::Free()
{
	__super::Free();
	Safe_Release(m_pCompute_Shader);
}

#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instance final : public CComponent
{
private:
	CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instance(const CModel_Instance& rhs);
	virtual ~CModel_Instance() = default;

public:
	HRESULT Initialize_Prototype(class CModel* pTargetModel);

	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, TextureType eTextureType, const char* pConstantName);
	HRESULT Bind_Buffers(_uint iMeshIdx);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0); 
	
private:
	_uint									m_iNumMeshes = { 0 };
	vector<class CMesh_Instance*>			m_InstanceMeshes;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	HRESULT Ready_Instancing(vector<_float4x4>& WorldMatrices, vector<_int>& ObjFlags);

private:
	_uint													m_iNumMaterials = { 0 };
	vector<MATERIALDESC>									m_Materials;

public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, class CModel* pTargetModel);
	 CComponent* Clone(void* pArg = nullptr)	override;
	 void Free() override;
};

END


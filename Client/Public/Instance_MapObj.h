#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CModel_Instance;
class CShader;
END

BEGIN(Client)

class CInstance_MapObj final : public CGameObject
{
public:
	typedef struct tagInstanceMapObjDesc : public CTransform::TRANSFORMDESC
	{
		_uint	iLevel;
		wstring strInstanceComponentTag;
		_float  fRadius;
		_float	fMipLevel;
		_int	iObjFlag = {};
	}INSTANCE_MAPOBJ_DESC;
	
	using OBJ_FLAG = enum tagObjDesc
	{
		SHADOW, REFLECTION, FLAG_END
	};

protected:
	CInstance_MapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_MapObj(const CInstance_MapObj& rhs);
	virtual ~CInstance_MapObj() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void PriorityTick(_float fTimeDelta)	override;
	void Tick(_float fTimeDelta)			override;
	void LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;
	HRESULT Render_Cascade()				override;
	HRESULT Render_ShadowDepth()			override;

private:
	vector<_float4x4>		m_WorldMatrices = {};
	vector<_int>			m_vecObjFlags = {};

public:
	void Add_WorldMatrix(const _float4x4& WorldMatrix) {
		m_WorldMatrices.emplace_back(WorldMatrix);
	}

	void Add_ObjFlag(const _int& _iObjFlag) { m_vecObjFlags.emplace_back(_iObjFlag); }

	vector<_float4x4> Get_WorldMatrices() { return m_WorldMatrices; }
	vector<_int> Get_ObjFlags() { return m_vecObjFlags; }

protected:
	CShader* m_pShader = { nullptr };
	CModel_Instance* m_pModel_Instance = { nullptr };

private:
	HRESULT Ready_Components();

private:
	_uint	m_iLevel = {};
	wstring m_strInstanceComponentTag = {};
	_int	m_iObjFlag = {};
	_float	m_fMipLevel = {};

	INSTANCE_MAPOBJ_DESC	m_ModelDesc;


public:
	CModel_Instance* Get_Model() const {
		return m_pModel_Instance;
	}

public:
	static CInstance_MapObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
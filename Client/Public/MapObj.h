#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CMapObj final : public CGameObject
{
public:
	typedef struct tagMapObjDesc 
	{
		wstring		strComponentTag;
		_float4x4	WorldMatrix;
		_float		fRadius;
	} MAPOBJ_DESC;

private:
	CMapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObj(const CMapObj& rhs);
	virtual ~CMapObj() = default;

public:
	HRESULT Initialize_Prototype()			override;
	HRESULT Initialize(void* pArg)			override;
	void PriorityTick(_float fTimeDelta)	override;
	void Tick(_float fTimeDelta)			override;
	void LateTick(_float fTimeDelta)		override;
	HRESULT Render()						override;

private:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	_float		m_fRadius = { 1.f };

private:
	HRESULT Ready_Components(MAPOBJ_DESC* pDesc);

public:
	static CMapObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END
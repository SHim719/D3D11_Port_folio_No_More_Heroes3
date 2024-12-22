#pragma once

#include "UI.h"

BEGIN(Engine)
class CVIBuffer_Point;

END

BEGIN(Client)

class CUI_BasicBase : public CUI
{
protected:
	CUI_BasicBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BasicBase(const CUI_BasicBase& rhs);
	virtual ~CUI_BasicBase() = default;

protected:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	HRESULT Render()					override;

public:
	void Change_ShaderPass(_uint Index) { m_iPass = Index; }
	void Set_Texture_Index(_uint Index) { m_iTexIndex = Index; }
	_uint Get_Texture_Index() { return m_iTexIndex; }
	_vector Convert_3D();
	void	Set_Billboard();


private:
	HRESULT Ready_Components();

protected:
	CVIBuffer_Point*	m_pVIBuffer_Point = { nullptr };
	_uint				m_iTexIndex = { 0 };
	_uint				m_iPass = { 0 };

	
public:
	static CUI_BasicBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END


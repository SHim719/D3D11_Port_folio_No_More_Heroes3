#pragma once

#include "Component.h"

/* 현재 내 지형을 타고 다닐 수 있도록 삼각형들을 가지고 있는다. */
/* 이 네비게이션을 이용하는 객체가 어떤 셀안에 잇ㄴ느지에 대한 삼각형 인덱스를 저장하고 있는다. */

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int		iCurrentCellIndex;
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	HRESULT Initialize_Prototype(const wstring& strNavigationDataFile);
	HRESULT Initialize(void* pArg) override;
	HRESULT Render() override;

public:
	_bool  isMove(_fvector vPosition, OUT _float4* pNormal);
	_bool  IsInCells(_fvector vPosition);
	_float Decide_YPos(_fvector vPosition);

	_bool Set_CurrentIdx(_fvector vPosition);	// JS - 0921 return _bool
	void Set_CurrentIdx(_uint iIdx) {
		m_iCurrentIndex = iIdx;
	}
	
	_uint Get_CurrentIdx() const {
		return m_iCurrentIndex;
	}

public:
	/* Tool */
	vector<_float3>	Get_InitialNavData() { return m_vecNavPoints; }

	HRESULT Tool_DrawPoint(_float3& fPoint, _int iCurIdx);
	HRESULT	Tool_DrawNav(vector<_float3>& vecPoints);
	HRESULT Tool_LoadNav();
	HRESULT Tool_ClearNav();

private:
	/* Cell */
	vector<class CCell*>		m_Cells;
	vector<class CCellPoint*>	m_CellPoints;
	_int						m_iCurrentIndex = { -1 };

	_float4						m_vColor = { 0.f, 1.f, 0.f, 1.f };
	_float4						m_vPointColor = { 0.f, 1.f, 0.f, 1.f };
	vector<_float3>				m_vecNavPoints = {};
	_int						m_iCurOrder = { 0 };

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationDataFile);
	CComponent* Clone(void* pArg) override;
	void Free() override;
};

END

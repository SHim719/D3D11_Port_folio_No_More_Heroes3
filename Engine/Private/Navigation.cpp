#include "Navigation.h"
#include "Cell.h"
#include "CellPoint.h"

#include "GameInstance.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells{ rhs.m_Cells }
	, m_iCurrentIndex{ rhs.m_iCurrentIndex }
#ifdef _DEBUG
	, m_pShader{ rhs.m_pShader }
	, m_vecNavPoints{ rhs.m_vecNavPoints }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
	Safe_AddRef(m_pGameInstance);

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

	for (auto& pCellPoint : m_CellPoints)
		Safe_AddRef(pCellPoint);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationDataFile)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		for (size_t i = 0; i < 3; i++)
		{
			m_vecNavPoints.push_back(vPoints[i]);
		}

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);

		CCellPoint* pCellPoint = CCellPoint::Create(m_pDevice, m_pContext, vPoints);
		if (nullptr == pCellPoint)
			return E_FAIL;

		m_CellPoints.emplace_back(pCellPoint);
	}

	SetUp_Neighbors();

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CNavigation::NAVIGATION_DESC* pDesc = static_cast<CNavigation::NAVIGATION_DESC*>(pArg);
		m_iCurrentIndex = pDesc->iCurrentCellIndex;
	}

	return S_OK;
}

HRESULT CNavigation::Render()
{
#ifdef _DEBUG
	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Bind_RawValue("g_WorldMatrix", &IdentityMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (-1 == m_iCurrentIndex)
	{
		m_vColor.x = 0.f;
		m_vColor.y = 1.f;

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();

		if (!m_CellPoints.empty()) 
		{
			_int iCurIdx = static_cast<int>(m_CellPoints.size());

			for (size_t i = 0; i < m_CellPoints.size(); ++i) 
			{
				auto& pCellPoint = m_CellPoints[i]; 

				if (m_iCurOrder <= 2)
				{
					if (m_CellPoints[i] == m_CellPoints[iCurIdx - 1])
						m_CellPoints[iCurIdx - 1]->Render(m_pShader, m_iCurOrder);

					else if (m_CellPoints[i] == m_CellPoints[iCurIdx - 2])
						m_CellPoints[iCurIdx - 2]->Render(m_pShader, 1);

					else
						m_CellPoints[i]->Render(m_pShader, 3);
				}
				else
					pCellPoint->Render(m_pShader, 3);
			}
		}

	}
	else
	{
		m_vColor.x = 1.f;
		m_vColor.y = 0.f;

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentIndex]->Render();

	}

#endif

	return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition, OUT _float4* pNormal)
{
	if (-1 == m_iCurrentIndex || m_iCurrentIndex >= m_Cells.size())
		return false;

	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentIndex]->isIn(vPosition, &iNeighborIndex, pNormal))
		return true;

	else
	{
		if (-1 == iNeighborIndex)
			return false;

		else
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, pNormal))
					break;
			}

			m_iCurrentIndex = iNeighborIndex;
			return true;
		}
	}
}

_bool CNavigation::IsInCells(_fvector vPosition)
{
	_int		iNeighborIndex = { -1 };

	for (auto& iter : m_Cells) {
		if (iter->isIn(vPosition, &iNeighborIndex)) {
			return true;
		}
	}
	return false;
}

_float CNavigation::Decide_YPos(_fvector vPosition)
{
	return m_Cells[m_iCurrentIndex]->Calc_YPos(vPosition);
}

_bool CNavigation::Set_CurrentIdx(_fvector vPosition)
{
	_int iNeighbor = 0;
	_int iCellIdx = 0;

	for (auto& pCell : m_Cells)
	{
		if (pCell->isIn(vPosition, &iNeighbor))
		{
			m_iCurrentIndex = iCellIdx;
			return true;
		}
		++iCellIdx;
	}

	return false;
}

HRESULT CNavigation::Tool_DrawPoint(_float3& fPoint, _int iCurIdx)
{
	CCellPoint* pCellPoint = CCellPoint::Create(m_pDevice, m_pContext, &fPoint);

	if (nullptr == pCellPoint)
		return E_FAIL;

	m_CellPoints.emplace_back(pCellPoint);

	m_iCurOrder = iCurIdx;

	return S_OK;
}

HRESULT CNavigation::Tool_DrawNav(vector<_float3>& vecPoints)
{
	_float3		vPoints[CCell::POINT_END] = {};

	for (int i = 0; i < 3; i++)
	{
		vPoints[i] = vecPoints[i];
	}

	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.emplace_back(pCell);

	return S_OK;
}

HRESULT CNavigation::Tool_LoadNav()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = nullptr;
	Ofn.nMaxFile = MAX_PATH;
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/DataFiles/NavData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return E_FAIL;
	}

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	m_vecNavPoints.clear();

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return E_FAIL;
	}

	while (true)
	{
		_float3		vPoints[CCell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		for (size_t i = 0; i < 3; i++)
			m_vecNavPoints.push_back(vPoints[i]);

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CNavigation::Tool_ClearNav()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	for (auto& pCellPoint : m_CellPoints)
		Safe_Release(pCellPoint);

	m_CellPoints.clear();

	m_vecNavPoints.clear();

	return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	for (auto& pCellPoint : m_CellPoints)
		Safe_Release(pCellPoint);

	m_CellPoints.clear();
}

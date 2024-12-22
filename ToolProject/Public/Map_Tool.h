#pragma once

#include "Tool_Super.h"
#include "ToolMapObj.h"
#include "ToolTriggerObj.h"
#include "Terrain.h"
#include "Grid.h"
#include "Water.h"
#include "Grass_Cube.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CVIBuffer_Grid;
class CVIBuffer_Cube;
END

BEGIN(Tool)

class CMap_Tool : public CTool_Super
{
public:
	using TAB = enum tabCurrentTab { 
		TAB_MESH, TAB_GRID, TAB_END
	};

	enum Flags { SHADOW, REFLECTION, FLAG_END };
public:
	using GUIZMODESC = struct tagGuizmoDesc
	{
		ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::WORLD;
		ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
		bool  bUseSnap = false;
		bool  boundSizing = false;
		bool  boundSizingSnap = false;
		float snap[3] = { 0.5f, 0.5f, 0.5f };
		float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
		float boundsSnap[3] = { 0.1f, 0.1f, 0.1f };
	};

protected:
	CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize();
	void	Start_Tool()			override;
	void	Tick(_float fTimeDelta)	override;

private:
	void	Main_Window();
	void	Tab_Bar();
	void	Env_Bar();
	void	Tool_Bar();
	void	TerrainTool_Bar();

private:
	void	Add_SelectedModels(const wstring& strModel, const wstring& strLayer);

	void	Generate_List(const wstring& strPath, _bool bPicked);
	void	Save_ModelData();
	void	Save_AddressData(const _tchar* wszFileName);
	void	Load_ModelData();
	void	Add_ModelData();
	void	Clear_ModelData();
	void	Delete_ModelData(_uint iCurIdxSelected);

	void	Create_Prototype(_char szConverted[MAX_PATH], wstring& strFolderName, const wstring& strFilePath, _vector vPickedPos);
	void	Clone_Object();
	void	Generate_ListBox(const string& strFolder, const wstring& strFilePath, _bool bPicked);

	void	Model_ForTest();
	void	Show_TestModel(_bool bTest);
	void	Show_Terrain(_bool bTerrain);

	_bool	NaturalSortCompare(const wstring& a, const wstring& b);
	void	Sort_SelectedModels();
	void	Sort_VecObjects();

private:
	/* Picking - MapObjects */
	void	Add_ByPicking(_char szConverted[MAX_PATH], wstring& strFolderName, const wstring& strFilePath);
	void	Grid_Picking(_char szConverted[MAX_PATH], wstring& strFolderName, const wstring& strFilePath, _bool bDrag);
	void	Grid_Picking();
	void	Select_ByPicking();

private:
	/* Event Trigger */
	void	Trigger_Info(_uint iType, _bool bPicked);
	void	Add_Trigger(_uint iType, wstring& strTag);
	void	Delete_Trigger(_uint iCurIdxSelected);
	void	Save_TriggerData();
	void	Load_TriggerData();

private:
	_uint	Position_Info();
	_uint	Add_PickedPosData(const string& strPositionTag);
	void	Delete_PositionData(_uint iCurIdx);
	void	Reset_PositionData();

	HRESULT	Save_PositionData();
	HRESULT	Load_PositionData();

	string  m_strPositionTag;

private:
	/* Navigation */
	void				Nav_Picking();

	_float3				Snap(_float3& vPointsToCompare, vector<_float3>& vecPoints);
	vector<_float3>&	ClockWise(vector<_float3>& vecPoints);
	bool				IsClockwise(const _float3& p1, const _float3& p2, const _float3& p3);

	HRESULT				Save_NavigationData();
	HRESULT				Load_NavigationData();
	HRESULT				Reset_NavigationData();

	vector<_float3>		m_vecPoints;
	vector<_float3>		m_vecNavigationPoints;
	list<_float4>		m_listGridPos;
	CTerrain*			m_pTerrain = { nullptr };
	CGrid*				m_pGrid = { nullptr };
	CGrass_Cube*		m_pGrass_Cube = { nullptr };
	CNavigation*		m_pNavigation = { nullptr };

private:
	/* Light & Shadow */
	void				IBLTextureInfo(_bool bActivate, const _tchar* strBrdf, const _tchar* strDiffuse, const _tchar* strSpecular);
	HRESULT				UpdateIBLTexture();

	void				Update_LightInfo();
	void				Update_ShadowInfo();
	void				Save_LightInfo(HANDLE hFile, _ulong& dwByte);
	void				Save_ShadowInfo(HANDLE hFile, _ulong& dwByte);
	void				Save_IBLInfo(HANDLE hFile, _ulong& dwByte);
	void				Save_EnvInfo(HANDLE hFile, _ulong& dwByte);

	_float4				m_vDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float4				m_vDirection = { 0.f, -1.f, 1.f, 0.f };
	_float				m_fLightStrength = 1.f;
	_float				m_fAmbientStrength = 0.5f;

	_float4				m_vShadowDir = { 0.f, -1.f, 1.f, 0.f };
	_float4				m_vShadowPos = { 0.f, 50.f, -30.f, 1.f };
	_float				m_fShadowBias = { 0.0000015f };
	_float4x4			m_matShadowPos = {};

	_tchar				m_strBrdf[MAX_PATH] = {};
	_tchar				m_strDiffuseHDR[MAX_PATH] = {};
	_tchar				m_strSpecularHDR[MAX_PATH] = {};
	wstring				m_strIBLImage = {};

private:
	/* Environment (Sky, Water) */
	SKY_DESCS			m_SkyDesc = {};
	WATER_DESCS			m_WaterDesc = {};

	class CWater*		m_pWater = { nullptr };
	_float4				m_vWaterPos = { 0.f, -50.f, 0.f, 1.f };
	_float3				m_vWaterScale = { 3000.f, 3000.f, 1.f };

	void				Add_Water(WATER_DESCS WaterDesc, _float4 vWaterPos, _float3 vWaterScale);
	void				Remove_Water();
	
	_bool				m_bGrid = { false };
	_bool				m_bIbl = { false };
	_bool				m_bSky = { true };
	_bool				m_bWater = { false };

private:
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<wstring>			Find_Model(const wstring& strFilePath);

	IBL_TEXTURES			m_IBL = {};
	TAB						m_Tab = { TAB_MESH };

private:
	class CToolMapObj* m_pModelObject = nullptr;
	class CToolMapObj* m_pTestObject = nullptr;
	class CToolTriggerObj* m_pTrigger = nullptr;

private:
	map<wstring, CToolMapObj::MODEL_DESC> m_ModelDesc;

	_bool					m_bFoundModel = false;
	_bool					m_bPathChanged = false;
	vector<wstring>			vecModels;

	vector<class CToolMapObj*>		m_vecObjects;
	vector<pair<wstring, wstring>>	vecSelectedModels;

	vector<wstring>			 m_vecSelectedTriggers;
	static map<wstring, int> m_ModelCounter;

private:
	map<wstring, CToolTriggerObj::TRIGGER_DESC*> m_TriggerDesc;
	vector<class CToolTriggerObj*>		m_vecAddedTriggers;
	wstring m_strSelectedTrigger;
	wstring m_wstrDefaultDir;

private:
	vector<pair<string, _float3>>			m_vecPositionData;
private:
	GUIZMODESC			m_tGuizmoDesc = {};

	LIGHT_DESC			m_LightDesc = {};

	/* Find, Save, Delete */
	//wstring m_FileName;

	wstring m_strModelName;
	wstring m_strModelLayer;
	wstring m_strFilePath;

	wstring m_strSelectedPath;
	wstring m_strSelectedModel;
	wstring m_hModelFile;

	_int	m_iCurIdx = { 0 };
	_int	m_iIdx = { -1 };
	_bool	m_bPicked = { false };
	_bool	m_bShowGui = { true };
	_bool   m_bAdded = { false };

	_bool	m_bObjFlag[FLAG_END] = {};
	_bool	m_bLoadMap = { false };
	_bool   m_bInitial = { true };
	_bool	m_bNewIBLTexture = { false };
	_bool	m_bNoIBLTexture = { true };

public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END
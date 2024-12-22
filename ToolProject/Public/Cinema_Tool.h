#pragma once

#include "Tool_Super.h"
#include "ToolMapObj.h"
#include "Water.h"


BEGIN(Tool)

class CCinema_Tool : public CTool_Super
{
private:
	enum CinemaKey { SCALE, LOOK, POSITION, EVENT, KEY_END};

private:
	CCinema_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCinema_Tool() = default;

public:
	HRESULT Initialize()			override;
	void Start_Tool()				override;
	void Tick(_float fTimeDelta)	override;

private:
	void Main_Window();
	void Menu_Bar();
	void Tab_Bar();

	void Actor_KeyInput();
	void Actor_ListBox();
	void Actor_Buttons();
	void Actor_Info();
	void Actor_Transform();
	void Actor_ModelInfo();
	void Actor_Gizmo();

	void Actor_AddCinemaKey();

	void Anim_Window();
	void Actor_AnimListBox();
	void Anim_Buttons();

	void Cinema_Window();
	void Cinema_ListBox();
	void Cinema_FrameDesc();

	void Scale_ListBox();
	void Look_ListBox();
	void Position_ListBox();
	void Event_ListBox();

private:
	void Play_Cinematic(_float fTimeDelta);
	void Start_Cinematic();
	void End_Cinematic();

	HRESULT Save_CinematicData();
	HRESULT Load_CinematicData();
	HRESULT Load_MapData();
	HRESULT Load_SoundData();
	
	HRESULT Load_EffectData();

	wstring Get_FolderPath(const wstring& wstrInitPath);

private:
	void	Update_LightInfo();
	void	Update_ShadowInfo();
	void	Add_Water(WATER_DESCS WaterDesc, _float4 vWaterPos, _float3 vWaterScale);

	void	Add_SelectedModels(const wstring& strModel, const wstring& strLayer);

	_bool	NaturalSortCompare(const wstring& a, const wstring& b);
	void	Sort_SelectedModels();
	
private:
	vector<class CCinemaObject*>	m_CinemaObjects;
	_int							m_iCinemaObjIdx = { 0 };

	_bool							m_bIsPlaying = { false };
	_float							m_fPlayTimeAcc = { 0.f };
	_float							m_fPlayTime = { 0.f };

	_bool							m_bEditCamera = { false };
	class CCinema_Camera*			m_pCamera = { nullptr };

	_char							m_szActorTag[MAX_PATH] = {};
	vector<string>					m_strModelTags;
	_int							m_iSelModelIdx = { 0 };

	const _char*					m_szKeyTypes[4]	= { "Scale", "Look", "Position", "Event" };
	CinemaKey						m_eKeyToAdd = { SCALE };
	
	CinemaKey						m_eKeyToListBox = { SCALE };
	_int							m_iSelectKey = { 0 };
	_float							m_fNowKeyTime = { 0.f };

	const _char*					m_szCinemaEventTypes[CINEMA_EVENT_END] = { "Change Animation", "Play Sound", "Stop Sound", "Active Effect", "Inactive Effect", "Etc"};
	_char							m_szEventParam[MAX_PATH] = {};
private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	class CToolMapObj*				m_pModelObject = nullptr;

	_tchar							m_strBrdf[MAX_PATH] = {};
	_tchar							m_strDiffuseHDR[MAX_PATH] = {};
	_tchar							m_strSpecularHDR[MAX_PATH] = {};

	_float4							m_vDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float4							m_vDirection = { 0.f, -1.f, 1.f, 0.f };
	_float							m_fLightStrength = 1.f;
	_float							m_fAmbientStrength = 0.5f;

	_float4							m_vShadowDir = { 0.f, -1.f, 1.f, 0.f };
	_float4							m_vShadowPos = { 0.f, 50.f, -30.f, 1.f };
	_float							m_fShadowBias = { 0.0000015f };
	_float4x4						m_matShadowPos = {};

	_bool							m_bLoadMap = { false };

	class CWater*					m_pWater = { nullptr };

	LIGHT_DESC						m_LightDesc = {};
	SKY_DESCS						m_SkyDesc = {};
	WATER_DESCS						m_WaterDesc = {};

	_float4							m_vWaterPos = { 0.f, -50.f, 0.f, 1.f };
	_float3							m_vWaterScale = { 3000.f, 3000.f, 1.f };

	map<wstring, CToolMapObj::MODEL_DESC> m_ModelDesc;

	static map<wstring, int>		m_ModelCounter;
	list<_float4>					m_listGridPos;
	vector<class CToolMapObj*>		m_vecObjects;
	vector<pair<wstring, wstring>>	vecSelectedModels;

private:
	void Create_NewActor();

private:
	using GIZMODESC = struct tagGuizmoDesc
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

	GIZMODESC	m_tGizmoDesc;
	_float3		m_vPosition = {};
	_float3		m_vLook = {};
	_float3		m_vScale = {};

	void Reset_Transform(_fmatrix WorldMatrix);

public:
	static CCinema_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END


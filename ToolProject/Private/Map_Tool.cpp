#include "Map_Tool.h"

#include "Animation.h"
#include "Bone.h"
#include "Model.h"
#include "Layer.h"
#include "Navigation.h"

#include "ToolColliderObj.h"
#include "GameInstance.h"
#include <cmath>

map<wstring, int> CMap_Tool::m_ModelCounter;

CMap_Tool::CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool_Super()
	, m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMap_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	m_LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	m_LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Set_Active_Shadow(true);
	m_pGameInstance->Set_Active_Sky(true);

	return S_OK;
}

void CMap_Tool::Start_Tool()
{
	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 1000.f;
	camDesc.fFovy = 70.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	m_pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));

	ImGui::StyleColorsClassic();

	m_pGameInstance->Change_MainCamera(m_pCamera);
}

void CMap_Tool::Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_DirectionalLight(m_LightDesc);
	m_pGameInstance->Set_AmbientStrength(m_fAmbientStrength);

	if (KEY_PUSHING(eKeyCode::LAlt) && KEY_DOWN(eKeyCode::G))
	{
		if (m_bShowGui)
		{
			m_bShowGui = false;
			return;
		}
		else
		{
			m_bShowGui = true;
			return;
		}
	}

	if (KEY_PUSHING(eKeyCode::LAlt) && KEY_DOWN(eKeyCode::One))
		m_tGuizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;

	else if (KEY_PUSHING(eKeyCode::LAlt) && KEY_DOWN(eKeyCode::Two))
		m_tGuizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;

	else if (KEY_PUSHING(eKeyCode::LAlt) && KEY_DOWN(eKeyCode::Three))
		m_tGuizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;

	else if (KEY_PUSHING(eKeyCode::LAlt) && KEY_DOWN(eKeyCode::Four))
		m_tGuizmoDesc.CurrentGizmoMode = m_tGuizmoDesc.CurrentGizmoMode == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);

	if (m_bShowGui)
		Main_Window();
}

void CMap_Tool::Main_Window()
{
	ImGui::Begin("Map Objects", (bool*)0);
	Tab_Bar();
	ImGui::End();

	ImGui::Begin("Environment", (bool*)0);
	Env_Bar();
	ImGui::End();

	ImGui::Begin("Tool", (bool*)0);
	Tool_Bar();
	ImGui::End();
}

void CMap_Tool::Tab_Bar()
{
	static _bool bPicked = false;
	static _bool bShowCollider = false;
	static _bool bTestModel = false;
	static _bool bTerrain = false;

	_int iFlag = 0;

	//if (KEY_DOWN(eKeyCode::P))
	//	bPicked = !bPicked;

	//if (KEY_DOWN(eKeyCode::M))
	//	bTestModel = !bTestModel;

	//if (KEY_DOWN(eKeyCode::I))
	//{
	//	iFlag++;
	//	m_bIbl = !m_bIbl;
	//}

	//if (KEY_DOWN(eKeyCode::T))
	//	bTerrain = !bTerrain;

	//if (KEY_DOWN(eKeyCode::C))
	//	bShowCollider = !bShowCollider;

	if (nullptr != m_pModelObject)
	{
		_int iObjFlag = m_pModelObject->Get_ModelDesc().iObjFlag;

		for (_int i = 0; i < FLAG_END; ++i)
			m_bObjFlag[i] = (iObjFlag & (1 << i)) != 0;
	}
	else
	{
		m_bObjFlag[SHADOW] = true;
		m_bObjFlag[REFLECTION] = false;
	}

	ImGui::SeparatorText("Tool Settings");
	ImGui::Checkbox("Enable Picking", &bPicked);
	ImGui::SameLine();
	ImGui::Checkbox("Show Colliders", &bShowCollider);
	ImGui::Checkbox("Use Snap", &m_tGuizmoDesc.bUseSnap);
	ImGui::SameLine();
	ImGui::Checkbox("TestModel", &bTestModel);
	ImGui::SameLine();
	ImGui::Checkbox("Terrain", &bTerrain);

	Show_TestModel(bTestModel);
	Show_Terrain(bTerrain);

	ImGui::SeparatorText("Options");
	if (ImGui::Button("Save Data") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::S))
		Save_ModelData();

	ImGui::SameLine();
	if (ImGui::Button("Load Data") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::L))
		Load_ModelData();

	ImGui::SameLine();
	if (ImGui::Button("Add Data") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::A))
		Add_ModelData();

	ImGui::SameLine();
	if (ImGui::Button("Clear Data"))
		Clear_ModelData();

	_bool bOptionChanged = false;

	ImGui::SeparatorText("Shader Settings");
	if (ImGui::Checkbox("Shadow", &m_bObjFlag[SHADOW]))
		bOptionChanged = true;

	if (KEY_DOWN(eKeyCode::Space))
	{
		m_bObjFlag[SHADOW] = !m_bObjFlag[SHADOW];
		bOptionChanged = true;
	}

	ImGui::SameLine();
	if (ImGui::Checkbox("Reflection", &m_bObjFlag[REFLECTION]))
		bOptionChanged = true;

	if (bOptionChanged && m_pModelObject != nullptr)
	{
		_int iNewObjFlag = 0;

		for (_int i = 0; i < FLAG_END; ++i)
		{
			if (m_bObjFlag[i])
				iNewObjFlag |= (1 << i);
		}

		m_pModelObject->Set_Flag(iNewObjFlag);
	}

	ImGui::SeparatorText("IBL Settings");
	if (ImGui::Button("Choose Image Texture"))
		UpdateIBLTexture();

	ImGui::SameLine();
	if (ImGui::Checkbox("Activate IBL", &m_bIbl) || m_bNewIBLTexture || iFlag)
		IBLTextureInfo(m_bIbl, m_strBrdf, m_strDiffuseHDR, m_strSpecularHDR);

	if (!m_bNoIBLTexture)
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), " [ Applied Texture ] : %s", Convert_WStrToStr(m_strIBLImage).c_str());
	else
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "[ No Texture Applied ]", Convert_WStrToStr(m_strIBLImage).c_str());

	if (m_pModelObject != nullptr)
		m_pModelObject->Show_Collider(bShowCollider);

	if (m_pTrigger != nullptr)
		m_pTrigger->Show_Trigger(bShowCollider);

	if (ImGui::BeginTabBar("Models"))
	{
		if (ImGui::BeginTabItem("Mesh"))
		{
			const char* strPath[] = {
				"Vehicles",
				//"Map_Boss/Damon",
				"Map_Boss/Land_MrBlackHole",
				//"Map_Boss/UFO",
				//"Map_Normal/Story_TravisRoom",
				//"Map_Normal/Story_NaomiLab",
				"Map_Normal/NaturePackage",
				"Map_Normal/IndustrialProps",
				"Map_Normal/SantaDestroy_A",
				"Map_Normal/SantaDestroy_B",
				"Map_Normal/SantaDestroy_Building",
				"Map_Normal/SantaDestroy_Ground", 
				//"Map_Normal/SantaDestroy_Highway",
				//"Map_Normal/SantaDestroy_Motel",
				//"Map_Normal/SantaDestroy_Road",
				//"Map_Normal/SantaDestroy_Sidewalk",
				"Map_Normal/LakeSideCabin",
				"Map_Boss/GarbageCollecting",
				"Map_Boss/ChainedFence",
				"Map_Boss/SonicJuice",
				"Map_Normal/World_City",
				"Map_Boss/CubeMesh",
				"Portal",
			};

			if (m_strSelectedPath.empty() || m_Tab != TAB_MESH)
			{
				m_strSelectedPath = Convert_StrToWStr(strPath[0]);
				m_Tab = TAB_MESH;
				m_iCurIdx = 0;
			}

			static int iCurPathIdx = 0;
			const char* strPreview = strPath[iCurPathIdx];

			ImGui::SeparatorText("Select Path");
			if (ImGui::BeginCombo("    ", strPreview))
			{
				for (int n = 0; n < IM_ARRAYSIZE(strPath); n++)
				{
					const bool bIsSelected = (iCurPathIdx == n);
					if (ImGui::Selectable(strPath[n], bIsSelected))
					{
						iCurPathIdx = n;
						m_strSelectedPath = Convert_StrToWStr(strPath[n]);
						m_bInitial = true;
						m_bPathChanged = true;
					}

					if (bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (!vecModels.empty() || m_bPathChanged)
			{
				vecModels.clear();
				m_bFoundModel = false;
				m_bPathChanged = false;
			}

			Generate_List(m_strSelectedPath, bPicked);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Trigger"))
		{
			static _uint  iType = { 0 };

			const char* strType[] = {
			   "AABB",
			   "OBB",
			   "Sphere"
			};

			static int iCurTypeIdx = 0;
			const char* strPreviewType = strType[iCurTypeIdx];

			ImGui::SeparatorText("Collider Type");
			if (ImGui::BeginCombo("##ColliderType", strPreviewType))
			{
				for (int n = 0; n < IM_ARRAYSIZE(strType); n++)
				{
					const bool bTypeSelected = (iCurTypeIdx == n);
					if (ImGui::Selectable(strType[n], bTypeSelected))
					{
						iCurTypeIdx = n;

						if (strType[n] == "AABB")
							iType = 0;
						else if (strType[n] == "OBB")
							iType = 1;
						else
							iType = 2;
					}

					if (bTypeSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			Trigger_Info(iType, bPicked);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Navi")) {
			ImGui::SeparatorText("List of Point Arrays");

			ImGuiIO& io = ImGui::GetIO(); (void)io;

			if (m_pTerrain == nullptr)
			{
				m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_GameObject(LEVEL_MAPTOOL, L"Terrain", 0));
				Safe_AddRef(m_pTerrain);
			}

			if (m_pNavigation == nullptr && m_pTerrain != nullptr)
			{
				m_pNavigation = static_cast<CNavigation*>(m_pTerrain->Find_Component(L"Navigation"));
				Safe_AddRef(m_pNavigation);
			}

			if (false == io.WantCaptureMouse && /*KEY_PUSHING(eKeyCode::LCtrl) && */KEY_DOWN(eKeyCode::LButton) && m_pNavigation != nullptr)
				Nav_Picking();

			if (KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::Z) && m_vecPoints.size() > 0)
			{
				m_vecPoints.pop_back();
				m_vecNavigationPoints.pop_back();
			}

			if (ImGui::CollapsingHeader("Nav Points"))
			{
				if (ImGui::BeginListBox("    "))
				{
					int pointIndex = 0;
					for (const auto& vecPoint : m_vecNavigationPoints)
					{
						char buffer[256];
						sprintf_s(buffer, "[%d]: %.2f , %.2f , %.2f ",
							pointIndex, vecPoint.x, vecPoint.y, vecPoint.z);

						string pointLabel = buffer;

						ImGui::Selectable(pointLabel.c_str());
						++pointIndex;
					}
					ImGui::EndListBox();
				}
			}

			if (ImGui::Button("Save") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::S))
				Save_NavigationData();

			ImGui::SameLine();
			if (ImGui::Button("Load") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::L))
				Load_NavigationData();

			ImGui::SameLine();
			if (ImGui::Button("Reset") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::R))
				Reset_NavigationData();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Shadow")) {
			Update_ShadowInfo();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Light")) {
			Update_LightInfo();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Grid")) {
			bPicked = true;
			m_pCamera->Set_Grid(true);

			if (!m_bGrid)
			{
				m_pCamera->Get_Transform()->Set_Position({ 0.f, 2.f, -16.f, 1.f });
				m_pCamera->Get_Transform()->LookAt({ 0.f, 2.f, 0.f, 1.f });

				m_pGrid = static_cast<CGrid*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Grid", L"Prototype_Grid"));
				Safe_AddRef(m_pGrid);

				m_bGrid = true;
			}

			const char* strPath[] = {
				"Map_Boss/CubeMesh",
				"Map_Boss/Mario"
			};

			if (m_Tab == TAB_MESH)
			{
				m_strSelectedPath = Convert_StrToWStr(strPath[0]);
				m_Tab = TAB_GRID;
				m_iCurIdx = 0;
			}

			static int iCurPathIdx = 0;
			const char* strPreview = strPath[iCurPathIdx];

			ImGui::SeparatorText("Select Path");
			if (ImGui::BeginCombo("    ", strPreview))
			{
				for (int n = 0; n < IM_ARRAYSIZE(strPath); n++)
				{
					const bool bIsSelected = (iCurPathIdx == n);
					if (ImGui::Selectable(strPath[n], bIsSelected))
					{
						iCurPathIdx = n;
						m_strSelectedPath = Convert_StrToWStr(strPath[n]);
						m_bInitial = true;
						m_bPathChanged = true;
					}

					if (bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (!vecModels.empty() || m_bPathChanged)
			{
				vecModels.clear();
				m_bFoundModel = false;
				m_bPathChanged = false;
			}

			Generate_List(m_strSelectedPath, bPicked);

			static _float4 vGridColor = { 1.f, 1.f, 1.f, 0.f };

			ImGui::SeparatorText("Grid Color");
			ImGui::DragFloat4("##GridColor", &vGridColor.x, 0.1f, 0.f, 1.f, "%.2f");

			if (nullptr != m_pGrid)
				m_pGrid->Set_GridColor(vGridColor);

			ImGui::EndTabItem();
		}
		else
		{
			m_pCamera->Set_Grid(false);
			m_bGrid = false;
			Safe_Release(m_pGrid);
			m_pGameInstance->Clear_Layer(LEVEL_MAPTOOL, L"Grid");
		}

		ImGui::EndTabBar();
	}
}

void CMap_Tool::Env_Bar()
{
	SKY_DESCS UpdatedSkyDesc = m_SkyDesc;
	_int iFlag = 0;

	if (ImGui::BeginTabBar("WaterSky"))
	{
		if (ImGui::BeginTabItem("Sky")) {

			ImGui::SeparatorText("Save Details");
			ImGui::Checkbox("Sky Info", &m_bSky);
			ImGui::SameLine();
			ImGui::Checkbox("Water Info", &m_bWater);

			ImGui::SeparatorText("Environment Settings");
			/* Sky */
			if (ImGui::TreeNode("Sky"))
			{
				ImGui::PushItemWidth(210.f);
				ImGui::SeparatorText("Top Color");
				ImGui::DragFloat4("##TopColor", &UpdatedSkyDesc.vTopColor.x, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::SeparatorText("Bottom Color");
				ImGui::DragFloat4("##BottomColor", &UpdatedSkyDesc.vBottomColor.x, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::SeparatorText("Horizon Color");
				ImGui::DragFloat3("##HorizonColor", &UpdatedSkyDesc.vHorizon_Color.x, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::SeparatorText("Horizon Blur");
				ImGui::DragFloat("##HorizonBlur", &UpdatedSkyDesc.fHorizon_Blur, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::TreePop();
			}

			/* Sun */
			if (ImGui::TreeNode("Sun"))
			{
				ImGui::SeparatorText("Sun Direction");
				ImGui::DragFloat3("##SunDir", &UpdatedSkyDesc.vSunDir.x, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::SeparatorText("Sun Color");
				ImGui::DragFloat3("##SunColor", &UpdatedSkyDesc.vSunColor.x, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::SeparatorText("Sun Size");
				ImGui::DragFloat("##SunSize", &UpdatedSkyDesc.fSunSize, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::SeparatorText("Sun Blur");
				ImGui::DragFloat("##SunBlur", &UpdatedSkyDesc.fSunBlur, 0.01f, -100.f, 100.f, "%.2f");

				ImGui::TreePop();
			}

			/* Cloud */
			if (ImGui::TreeNode("Cloud"))
			{
				ImGui::SeparatorText("Edge Color");
				ImGui::DragFloat3("##CloudEdgeColor", &UpdatedSkyDesc.vCloud_EdgeColor.x, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Top Color");
				ImGui::DragFloat3("##CloudTopColor", &UpdatedSkyDesc.vCloud_TopColor.x, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Middle Color");
				ImGui::DragFloat3("##CloudMiddleColor", &UpdatedSkyDesc.vCloud_MiddleColor.x, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Bottom Color");
				ImGui::DragFloat3("##CloudBottomColor", &UpdatedSkyDesc.vCloud_BottomColor.x, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Speed");
				ImGui::DragFloat("##CloudSpeed", &UpdatedSkyDesc.fCloud_Speed, 0.1f, -100.f, 100.f);

				ImGui::SeparatorText("Direction");
				ImGui::DragFloat("##CloudDirection", &UpdatedSkyDesc.fCloud_Direction, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Scale");
				ImGui::DragFloat("##CloudScale", &UpdatedSkyDesc.fCloud_Scale, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("CutOff");
				ImGui::DragFloat("##CloudCutOff", &UpdatedSkyDesc.fCloud_CutOff, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Fuzziness");
				ImGui::DragFloat("##CloudFuzziness", &UpdatedSkyDesc.fCloud_Fuzziness, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Weight");
				ImGui::DragFloat("##CloudWeight", &UpdatedSkyDesc.fCloud_Weight, 0.01f, -100.f, 100.f);

				ImGui::SeparatorText("Blur");
				ImGui::DragFloat("##CloudBlur", &UpdatedSkyDesc.fCloud_Blur, 0.01f, -100.f, 100.f);

				ImGui::TreePop();
			}

			ImGui::EndTabItem();
		}

		if (UpdatedSkyDesc != m_SkyDesc)
		{
			m_SkyDesc = UpdatedSkyDesc;
			m_pGameInstance->Set_SkyDescs(m_SkyDesc);
		}

		if (ImGui::BeginTabItem("Water"))
		{
			ImGui::SeparatorText("Create");
			if (ImGui::Button("Add Water"))
				Add_Water(m_WaterDesc, m_vWaterPos, m_vWaterScale);

			ImGui::SameLine();
			if (ImGui::Button("Remove Water"))
				Remove_Water();

			ImGui::PushItemWidth(200.f);
			ImGui::SeparatorText("Position");
			iFlag += ImGui::DragFloat4("##WaterPos", &m_vWaterPos.x, 0.01f, -100.f, 100.f, "%.2f");

			ImGui::SeparatorText("Scale");
			iFlag += ImGui::DragFloat3("##WaterScale", &m_vWaterScale.x, 0.01f, -100.f, 100000.f, "%.2f");

			ImGui::SeparatorText("Water Color");
			iFlag += ImGui::DragFloat4("##WaterColor", &m_WaterDesc.fWaterColor.x, 0.01f, -100.f, 100.f, "%.2f");

			ImGui::SeparatorText("Light Color");
			iFlag += ImGui::DragFloat4("##WaterLightColor", &m_WaterDesc.fLightColor.x, 0.01f, -100.f, 100.f, "%.2f");

			ImGui::SeparatorText("Flow Speed");
			iFlag += ImGui::DragFloat2("##FlowSpeed", &m_WaterDesc.vFlowSpeed.x, 0.01f, -100.f, 100.f);

			ImGui::SeparatorText("Voronoi Scale");
			iFlag += ImGui::DragFloat("##VoronoiScale", &m_WaterDesc.fVoronoiScale, 0.01f, -100.f, 1000.f);

			ImGui::SeparatorText("Noise Scale");
			iFlag += ImGui::DragFloat("##WaterNoiseScale", &m_WaterDesc.fWaterNoiseScale, 0.01f, -100.f, 1000.f);

			ImGui::EndTabItem();
		}

		if (m_pWater != nullptr)
		{
			if (iFlag)
			{
				m_pWater->m_pTransform->Set_Position(XMLoadFloat4(&m_vWaterPos));
				m_pWater->m_pTransform->Set_Scale(m_vWaterScale);
				m_pWater->Set_WaterDesc(m_WaterDesc);
			}
		}
		ImGui::EndTabBar();
	}
}

void CMap_Tool::Tool_Bar()
{
	static _uint iCurIdx = 0;

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	iCurIdx = Position_Info();

	if (false == io.WantCaptureMouse && KEY_DOWN(eKeyCode::LButton))
		Add_PickedPosData(m_strPositionTag);

	if (KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::Z) && m_vecPositionData.size() > 0)
		m_vecPositionData.pop_back();

	if (ImGui::Button("Delete") || KEY_DOWN(eKeyCode::Delete))
		Delete_PositionData(iCurIdx);

	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		Reset_PositionData();

	ImGui::SeparatorText("Save & Load");
	if (ImGui::Button("Save"))
		Save_PositionData();

	ImGui::SameLine();
	if (ImGui::Button("Load"))
		Load_PositionData();
}

void CMap_Tool::TerrainTool_Bar()
{

}

void CMap_Tool::Add_SelectedModels(const wstring& strModel, const wstring& strLayer)
{
	pair<wstring, wstring> NameAndLayer = { strModel, strLayer };
	vecSelectedModels.push_back(NameAndLayer);
}

void CMap_Tool::Generate_List(const wstring& strPath, _bool bPicked)
{
	wstring strFullPath = L"../../Resources/Models/MapObjects/" + strPath;
	wstring strFullPath1 = strFullPath + L"/";

	if (!m_bFoundModel)
	{
		vecModels = Find_Model(strFullPath);
		m_bFoundModel = true;
	}

	if (m_bInitial)
	{
		if (m_iCurIdx >= vecModels.size())
		{
			m_iCurIdx = 0;
			m_strModelName = vecModels[0];
		}
		else
			m_strModelName = vecModels[m_iCurIdx];
		m_bInitial = false;
	}

	string strPathConverted = Convert_WStrToStr(strPath) + "/";
	Generate_ListBox(strPathConverted, strFullPath1, bPicked);
}

void CMap_Tool::Save_ModelData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/MapData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_int iRet = { 0 };

	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Save File");
		return;
	}

	_tchar wszFileName[MAX_PATH] = {};
	wcscpy_s(wszFileName, Ofn.lpstrFile + Ofn.nFileOffset);

	Save_AddressData(wszFileName);

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return;
	}

	Save_LightInfo(hFile, dwByte);
	Save_ShadowInfo(hFile, dwByte);

	_uint iModelCnt = (_uint)m_vecObjects.size();
	WriteFile(hFile, &iModelCnt, sizeof _uint, &dwByte, nullptr);

	if (!m_vecObjects.empty())
	{
		for (_uint i = 0; i < iModelCnt; ++i)
		{
			if (nullptr != m_vecObjects[i])
				m_vecObjects[i]->Save_ModelData(hFile, dwByte);
		}
	}

	Save_IBLInfo(hFile, dwByte);
	Save_EnvInfo(hFile, dwByte);

	CloseHandle(hFile);

	return;
}
void CMap_Tool::Save_AddressData(const _tchar* wszFileName)
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/DataFiles/AddressData/", MAX_PATH);
	wcscat_s(wszAbsolutePath, wszFileName);
	Ofn.lpstrFile = wszAbsolutePath;
	Ofn.lpstrInitialDir = wszAbsolutePath;
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return;
	}

	_uint iModelCnt = (_uint)vecSelectedModels.size();
	set<pair<wstring, wstring>> setNameAndAddress;

	if (!m_vecObjects.empty())
	{
		for (_uint i = 0; i < iModelCnt; ++i)
		{
			if (nullptr != m_vecObjects[i])
			{
				wstring wstrName = Convert_StrToWStr(m_vecObjects[i]->Get_ModelDesc().strName);
				wstring wstrAddress = Convert_StrToWStr(m_vecObjects[i]->Get_ModelDesc().strAddress);

				pair<wstring, wstring> NameAndAddress = { wstrName, wstrAddress };
				setNameAndAddress.emplace(NameAndAddress);
			}
		}
	}

	_uint iSetCount = (_uint)setNameAndAddress.size();
	WriteFile(hFile, &iSetCount, sizeof _uint, &dwByte, nullptr);

	for (const auto& item : setNameAndAddress)
	{
		DWORD iLen1 = static_cast<DWORD>(item.first.size());
		DWORD iLen2 = static_cast<DWORD>(item.second.size());
		WriteFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		WriteFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
		WriteFile(hFile, item.first.c_str(), iLen1 * sizeof(wchar_t), &dwByte, nullptr);
		WriteFile(hFile, item.second.c_str(), iLen2 * sizeof(wchar_t), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	return;
}

void CMap_Tool::Load_ModelData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = nullptr;
	Ofn.nMaxFile = MAX_PATH;
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/MapData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return;
	}

	m_ModelDesc.clear();
	m_ModelCounter.clear();
	m_listGridPos.clear();

	for (size_t i = 0; i < m_vecObjects.size(); ++i)
		Safe_Release(m_vecObjects[i]);
	m_vecObjects.clear();

	m_bLoadMap = true;

	ReadFile(hFile, &m_vDiffuse, sizeof _float4, &dwByte, nullptr);
	ReadFile(hFile, &m_vDirection, sizeof _float4, &dwByte, nullptr);
	ReadFile(hFile, &m_fLightStrength, sizeof _float, &dwByte, nullptr);
	ReadFile(hFile, &m_fAmbientStrength, sizeof _float, &dwByte, nullptr);
	Update_LightInfo();

	ReadFile(hFile, &m_fShadowBias, sizeof _float, &dwByte, nullptr);
	ReadFile(hFile, &m_vShadowDir, sizeof _float4, &dwByte, nullptr);
	ReadFile(hFile, &m_vShadowPos, sizeof _float4, &dwByte, nullptr);
	Update_ShadowInfo();

	_uint iModelCnt = {};
	vector<wstring> vecComponents;
	ReadFile(hFile, &iModelCnt, sizeof _uint, &dwByte, nullptr);

	while (iModelCnt)
	{
		_float4x4	WorldMatrix = {};
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		CToolMapObj::MODEL_DESC pModelDesc{};

		ReadFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);
		ReadFile(hFile, &pModelDesc.fRadius, sizeof _float, &dwByte, nullptr);
		ReadFile(hFile, &pModelDesc.fMipLevel, sizeof _float, &dwByte, nullptr);

		DWORD iLen1 = 0;
		DWORD iLen2 = 0;
		DWORD iLen3 = 0;
		DWORD iLen4 = 0;

		ReadFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen3, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen4, sizeof DWORD, &dwByte, nullptr);

		ReadFile(hFile, pModelDesc.strName, iLen1, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc.strNumbered, iLen2, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc.strAddress, iLen3, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc.strFileLayer, iLen4, &dwByte, nullptr);
		ReadFile(hFile, &pModelDesc.iObjFlag, sizeof(_int), &dwByte, nullptr);

		pModelDesc.strName[iLen1] = '\0';
		pModelDesc.strNumbered[iLen2] = '\0';
		pModelDesc.strAddress[iLen3] = '\0';
		pModelDesc.strFileLayer[iLen4] = '\0';

		pModelDesc.matModel = WorldMatrix;

		wstring strModelNumbered = Convert_StrToWStr(pModelDesc.strNumbered);
		m_ModelDesc.emplace(strModelNumbered, pModelDesc);

		wstring strModelName = Convert_StrToWStr(pModelDesc.strName);
		m_ModelCounter[strModelName]++;

		--iModelCnt;
	}

	DWORD D1{};
	DWORD D2{};
	DWORD D3{};

	ReadFile(hFile, &D1, sizeof DWORD, &dwByte, nullptr);
	ReadFile(hFile, &D2, sizeof DWORD, &dwByte, nullptr);
	ReadFile(hFile, &D3, sizeof DWORD, &dwByte, nullptr);
	ReadFile(hFile, m_strBrdf, D1 * sizeof(_tchar), &dwByte, nullptr);
	ReadFile(hFile, m_strDiffuseHDR, D2 * sizeof(_tchar), &dwByte, nullptr);
	ReadFile(hFile, m_strSpecularHDR, D3 * sizeof(_tchar), &dwByte, nullptr);

	m_bIbl = true;
	
	IBLTextureInfo(m_bIbl, m_strBrdf, m_strDiffuseHDR, m_strSpecularHDR);

	fs::path dirPath = m_strBrdf;
	fs::path filename = dirPath.stem();
	m_strIBLImage = filename.wstring().substr(0, filename.wstring().find(L"Brdf"));

	_bool   bSky{};
	_bool   bWater{};

	ReadFile(hFile, &bSky, sizeof _bool, &dwByte, nullptr);
	ReadFile(hFile, &bWater, sizeof _bool, &dwByte, nullptr);

	if (bSky)
	{
		SKY_DESCS		SkyDesc = {};
		ReadFile(hFile, &SkyDesc, sizeof SKY_DESCS, &dwByte, nullptr);

		m_pGameInstance->Set_SkyDescs(SkyDesc);
		m_SkyDesc = SkyDesc;
	}

	if (bWater)
	{
		WATER_DESCS			WaterDesc = {};
		_float4				vWaterPos = {};
		_float3				vWaterScale = {};

		ReadFile(hFile, &WaterDesc, sizeof WATER_DESCS, &dwByte, nullptr);
		ReadFile(hFile, &vWaterPos, sizeof _float4, &dwByte, nullptr);
		ReadFile(hFile, &vWaterScale, sizeof _float3, &dwByte, nullptr);

		Add_Water(WaterDesc, vWaterPos, vWaterScale);

		m_WaterDesc = WaterDesc;
		m_vWaterPos = vWaterPos;
		m_vWaterScale = vWaterScale;
	}
	else
		Safe_Release(m_pWater);

	CloseHandle(hFile);

	_uint iLayerNum = m_pGameInstance->Get_LayerNum(LEVEL_MAPTOOL);
	vector<wstring> LayersToDelete;

	for (_uint i = 0; i < iLayerNum; ++i)
	{
		wstring strLayerName = m_pGameInstance->Get_LayerName(LEVEL_MAPTOOL, i);
		CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_MAPTOOL, strLayerName);

		if (bWater) {
			if (L"Light" == strLayerName || L"Terrain" == strLayerName || L"Water" == strLayerName || L"Grid" == strLayerName)
				continue;
		}
		else {
			if (L"Light" == strLayerName || L"Terrain" == strLayerName || L"Grid" == strLayerName)
				continue;
		}

		LayersToDelete.push_back(strLayerName);
	}

	for (const auto& LayerName : LayersToDelete)
	{
		m_pGameInstance->Clear_Layer(LEVEL_MAPTOOL, LayerName);
	}

	for (auto& pModelDesc : m_ModelDesc)
	{
		_char szAddress[256];
		_char szName[256];
		_char szFileLayer[256];

		strcpy_s(szAddress, pModelDesc.second.strAddress);
		wstring wstrAddress = Convert_StrToWStr(szAddress);
		string strAddress = Convert_WStrToStr(wstrAddress);

		if (L"../../Resources/Models/MapObjects/Map_Boss/CubeMesh/" == wstrAddress)
		{
			_float4	vGridCenter = {};

			vGridCenter.x = pModelDesc.second.matModel._41;
			vGridCenter.y = pModelDesc.second.matModel._42;
			vGridCenter.z = pModelDesc.second.matModel._43;
			vGridCenter.w = pModelDesc.second.matModel._44;

			m_listGridPos.emplace_back(vGridCenter);
		}

		strcpy_s(szFileLayer, pModelDesc.second.strFileLayer);
		wstring wstrFileLayer = Convert_StrToWStr(szFileLayer);

		strcpy_s(szName, pModelDesc.second.strName);
		wstring wstrName = Convert_StrToWStr(szName);

		wstring wstrPrototypeComponent = L"Prototype_Model_" + wstrName;

		string strName_dat = Convert_WStrToStr(wstrName) + ".dat";

		auto it = find(vecComponents.begin(), vecComponents.end(), wstrPrototypeComponent);

		if (it == vecComponents.end())
		{
			m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, wstrPrototypeComponent, CModel::Create(m_pDevice, m_pContext, strAddress, strName_dat));
			vecComponents.emplace_back(wstrPrototypeComponent);
		}

		/* Clone */
		m_pModelObject = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, wstrFileLayer, L"Prototype_ToolMapObj", &pModelDesc.second));
		Safe_AddRef(m_pModelObject);
		m_pModelObject->Show_Collider(false);
		m_vecObjects.emplace_back(m_pModelObject);
	}

	vecSelectedModels.clear();
	for (const auto& pModel : m_ModelDesc)
	{
		wstring wstrLayer = Convert_StrToWStr(pModel.second.strFileLayer);
		Add_SelectedModels(pModel.first, wstrLayer);
	}

	Sort_SelectedModels();
}

void CMap_Tool::Add_ModelData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = nullptr;
	Ofn.nMaxFile = MAX_PATH;
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/MapData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return;
	}

	m_bLoadMap = true;

	ReadFile(hFile, &m_vDiffuse, sizeof _float4, &dwByte, nullptr);
	ReadFile(hFile, &m_vDirection, sizeof _float4, &dwByte, nullptr);
	ReadFile(hFile, &m_fLightStrength, sizeof _float, &dwByte, nullptr);
	ReadFile(hFile, &m_fAmbientStrength, sizeof _float, &dwByte, nullptr);
	//Update_LightInfo();

	ReadFile(hFile, &m_fShadowBias, sizeof _float, &dwByte, nullptr);
	ReadFile(hFile, &m_vShadowDir, sizeof _float4, &dwByte, nullptr);
	ReadFile(hFile, &m_vShadowPos, sizeof _float4, &dwByte, nullptr);
	//Update_ShadowInfo();

	_uint iModelCnt = {};
	ReadFile(hFile, &iModelCnt, sizeof _uint, &dwByte, nullptr);

	while (iModelCnt)
	{
		_float4x4	WorldMatrix = {};
		_float		fRadius = {};
		_float		fMipLevel = {};
		_int		iObjFlag = {};
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		CToolMapObj::MODEL_DESC pModelDesc{};

		ReadFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);
		ReadFile(hFile, &fRadius, sizeof _float, &dwByte, nullptr);
		ReadFile(hFile, &fMipLevel, sizeof _float, &dwByte, nullptr);

		DWORD iLen1 = 0;
		DWORD iLen2 = 0;
		DWORD iLen3 = 0;
		DWORD iLen4 = 0;

		ReadFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen3, sizeof DWORD, &dwByte, nullptr);
		ReadFile(hFile, &iLen4, sizeof DWORD, &dwByte, nullptr);

		ReadFile(hFile, pModelDesc.strName, iLen1, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc.strNumbered, iLen2, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc.strAddress, iLen3, &dwByte, nullptr);
		ReadFile(hFile, pModelDesc.strFileLayer, iLen4, &dwByte, nullptr);
		ReadFile(hFile, &iObjFlag, sizeof(_int), &dwByte, nullptr);

		pModelDesc.strName[iLen1] = '\0';
		pModelDesc.strNumbered[iLen2] = '\0';
		pModelDesc.strAddress[iLen3] = '\0';
		pModelDesc.strFileLayer[iLen4] = '\0';

		pModelDesc.matModel = WorldMatrix;
		pModelDesc.fRadius = fRadius;
		pModelDesc.fMipLevel = fMipLevel;
		pModelDesc.iObjFlag = iObjFlag;

		wstring strModelNumbered = Convert_StrToWStr(pModelDesc.strNumbered);
		m_ModelDesc.emplace(strModelNumbered, pModelDesc);

		wstring strModelName = Convert_StrToWStr(pModelDesc.strName);
		m_ModelCounter[strModelName]++;

		--iModelCnt;
	}

	DWORD D1{};
	DWORD D2{};
	DWORD D3{};

	ReadFile(hFile, &D1, sizeof DWORD, &dwByte, nullptr);
	ReadFile(hFile, &D2, sizeof DWORD, &dwByte, nullptr);
	ReadFile(hFile, &D3, sizeof DWORD, &dwByte, nullptr);
	ReadFile(hFile, m_strBrdf, D1 * sizeof(_tchar), &dwByte, nullptr);
	ReadFile(hFile, m_strDiffuseHDR, D2 * sizeof(_tchar), &dwByte, nullptr);
	ReadFile(hFile, m_strSpecularHDR, D3 * sizeof(_tchar), &dwByte, nullptr);

	_bool   bSky{};
	_bool   bWater{};

	ReadFile(hFile, &bSky, sizeof _bool, &dwByte, nullptr);
	ReadFile(hFile, &bWater, sizeof _bool, &dwByte, nullptr);

	if (bSky)
	{
		SKY_DESCS		SkyDesc = {};
		ReadFile(hFile, &SkyDesc, sizeof SKY_DESCS, &dwByte, nullptr);

		/*m_pGameInstance->Set_SkyDescs(SkyDesc);
		m_SkyDesc = SkyDesc;*/
	}

	if (bWater)
	{
		WATER_DESCS			WaterDesc = {};
		_float4				vWaterPos = {};
		_float3				vWaterScale = {};

		ReadFile(hFile, &WaterDesc, sizeof WATER_DESCS, &dwByte, nullptr);
		ReadFile(hFile, &vWaterPos, sizeof _float4, &dwByte, nullptr);
		ReadFile(hFile, &vWaterScale, sizeof _float3, &dwByte, nullptr);

		/*Add_Water(WaterDesc, vWaterPos, vWaterScale);

		m_WaterDesc = WaterDesc;
		m_vWaterPos = vWaterPos;
		m_vWaterScale = vWaterScale;*/
	}
	/*else
		Safe_Release(m_pWater);*/

	CloseHandle(hFile);

	for (auto& pModelDesc : m_ModelDesc)
	{
		_char szAddress[256];
		_char szName[256];
		_char szFileLayer[256];

		strcpy_s(szAddress, pModelDesc.second.strAddress);
		wstring wstrAddress = Convert_StrToWStr(szAddress);
		string strAddress = Convert_WStrToStr(wstrAddress);

		strcpy_s(szFileLayer, pModelDesc.second.strFileLayer);
		wstring wstrFileLayer = Convert_StrToWStr(szFileLayer);

		strcpy_s(szName, pModelDesc.second.strName);
		wstring wstrName = Convert_StrToWStr(szName);

		wstring wstrPrototypeComponent = L"Prototype_Model_" + wstrName;

		string strName_dat = Convert_WStrToStr(wstrName) + ".dat";

		vector<wstring> vecComponents;
		auto pComponents = find(vecComponents.begin(), vecComponents.end(), wstrPrototypeComponent);

		if (pComponents == vecComponents.end()) {
			m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, wstrPrototypeComponent, CModel::Create(m_pDevice, m_pContext, strAddress, strName_dat));
			vecComponents.emplace_back(wstrPrototypeComponent);
		}

		/* Clone */
		m_pModelObject = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, wstrFileLayer, L"Prototype_ToolMapObj", &pModelDesc.second));
		Safe_AddRef(m_pModelObject);

		m_pModelObject->Show_Collider(false);
		m_vecObjects.emplace_back(m_pModelObject);
	}

	vecSelectedModels.clear();
	for (const auto& pModel : m_ModelDesc)
	{
		wstring wstrLayer = Convert_StrToWStr(pModel.second.strFileLayer);
		Add_SelectedModels(pModel.first, wstrLayer);
	}

	Sort_SelectedModels();
}

void CMap_Tool::Clear_ModelData()
{
	m_ModelDesc.clear();
	m_ModelCounter.clear();
	m_listGridPos.clear();

	for (size_t i = 0; i < m_vecObjects.size(); ++i)
		Safe_Release(m_vecObjects[i]);
	m_vecObjects.clear();

	_uint iLayerNum = m_pGameInstance->Get_LayerNum(LEVEL_MAPTOOL);
	vector<wstring> LayersToDelete;

	for (_uint i = 0; i < iLayerNum; ++i)
	{
		wstring strLayerName = m_pGameInstance->Get_LayerName(LEVEL_MAPTOOL, i);
		CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_MAPTOOL, strLayerName);

		if (L"Light" == strLayerName || L"Terrain" == strLayerName)
			continue;

		LayersToDelete.push_back(strLayerName);
	}

	for (const auto& LayerName : LayersToDelete)
	{
		m_pGameInstance->Clear_Layer(LEVEL_MAPTOOL, LayerName);
		m_pGameInstance->Clear_Renderer();
	}

	vecSelectedModels.clear();
	Safe_Release(m_pModelObject);
}

void CMap_Tool::Delete_ModelData(_uint iCurIdxSelected)
{
	if (iCurIdxSelected >= vecSelectedModels.size())
		return;

	auto vecIter = vecSelectedModels.begin();
	std::advance(vecIter, iCurIdxSelected);
	wstring modelName = vecIter->first;
	vecSelectedModels.erase(vecIter);
	m_ModelDesc.erase(modelName);

	wstring wstrName = CutOffNumbers(modelName);
	wstrName.pop_back();

	vector<pair<wstring, CToolMapObj::MODEL_DESC>> ModelsToRearrange;
	for (auto& pair : m_ModelDesc)
	{
		if (0 == pair.first.find(wstrName))
			ModelsToRearrange.push_back(pair);
	}

	for (size_t i = 1; i <= ModelsToRearrange.size(); ++i)
	{
		m_ModelDesc.erase(ModelsToRearrange[i - 1].first);
	}

	for (size_t i = 0; i < ModelsToRearrange.size(); ++i)
	{
		wstring wstrNewKey = wstrName + L"_" + to_wstring(i + 1);
		CToolMapObj::MODEL_DESC		ModelDesc = ModelsToRearrange[i].second;
		WideCharToMultiByte(CP_ACP, 0, wstrNewKey.c_str(), (_int)wstrNewKey.size(), ModelDesc.strNumbered, MAX_PATH, nullptr, nullptr);
		m_ModelDesc.insert({ wstrNewKey , ModelDesc });
	}

	auto counterIter = m_ModelCounter.find(wstrName);
	if (counterIter != m_ModelCounter.end()) {
		if (--counterIter->second <= 0) {
			m_ModelCounter.erase(counterIter);

		}
	}

	CToolMapObj* pObject = m_vecObjects[iCurIdxSelected];
	pObject->Set_Destroy(true);
	Safe_Release(m_pModelObject);

	auto ObjectIter = m_vecObjects.begin();
	advance(ObjectIter, iCurIdxSelected);
	m_vecObjects.erase(ObjectIter);

	vecSelectedModels.clear();
	for (auto& pModel : m_ModelDesc)
	{
		wstring wstrLayer = Convert_StrToWStr(pModel.second.strFileLayer);
		Add_SelectedModels(pModel.first, wstrLayer);
	}

	Sort_SelectedModels();
}

void CMap_Tool::Create_Prototype(_char szConverted[MAX_PATH], wstring& strFolderName, const wstring& strFilePath, _vector vPickedPos)
{
	if (m_ModelCounter.find(m_strModelName) == m_ModelCounter.end()) {
		m_ModelCounter[m_strModelName] = 0;
	}

	int& iNum = ++m_ModelCounter[m_strModelName];

	/* Create Prototype Component */
	wstring strFullPath = L"../../Resources/Models/MapObjects/" + strFolderName;

	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), lstrlenW(strFullPath.c_str()), szConverted, MAX_PATH, nullptr, nullptr);

	wstring strComponentTag = L"Prototype_Model_" + m_strModelName;

	string strTmp = ".dat";

	strTmp = Convert_WStrToStr(m_strModelName) + strTmp;

	m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, strComponentTag, CModel::Create(m_pDevice, m_pContext, szConverted, strTmp));

	wstring strNumberedName;
	strNumberedName = m_strModelName + L"_" + to_wstring(iNum);
	_char szFileName[256] = { 0 };
	_char szFileNumbered[256] = { 0 };
	_char szFileAddress[256] = { 0 };
	_char szFileLayer[256] = { 0 };
	wstring strLayer = L"Layer_";

	CToolMapObj::MODEL_DESC ModelDesc{};

	WideCharToMultiByte(CP_ACP, 0, m_strModelName.c_str(), lstrlenW(m_strModelName.c_str()), szFileName, MAX_PATH, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, strNumberedName.c_str(), lstrlenW(strNumberedName.c_str()), szFileNumbered, MAX_PATH, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, strFilePath.c_str(), lstrlenW(strFilePath.c_str()), szFileAddress, MAX_PATH, nullptr, nullptr);

	wstring strFileLayer = strLayer + Convert_StrToWStr(szFileName);
	WideCharToMultiByte(CP_ACP, 0, strFileLayer.c_str(), lstrlenW(strFileLayer.c_str()), szFileLayer, MAX_PATH, nullptr, nullptr);

	_matrix mat = XMMatrixIdentity();
	_float4x4 WorldMat;

	mat.r[3] = vPickedPos;
	XMStoreFloat4x4(&WorldMat, mat);

	ModelDesc.matModel = WorldMat;
	strcpy_s(ModelDesc.strName, szFileName);
	strcpy_s(ModelDesc.strNumbered, szFileNumbered);
	strcpy_s(ModelDesc.strAddress, szFileAddress);
	strcpy_s(ModelDesc.strFileLayer, szFileLayer);
	ModelDesc.fRadius = 1.f;
	ModelDesc.fMipLevel = 0.f;
	ModelDesc.iObjFlag = (1 << SHADOW);

	if (!m_strModelName.empty())
	{
		if (nullptr != m_pModelObject)
			m_pModelObject->Show_Collider(false);

		m_pModelObject = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, strFileLayer, L"Prototype_ToolMapObj", &ModelDesc));
		Safe_AddRef(m_pModelObject);
	}

	m_vecObjects.emplace_back(m_pModelObject);
	Sort_VecObjects();

	auto it = find(m_vecObjects.begin(), m_vecObjects.end(), m_pModelObject);

	if (it != m_vecObjects.end())
		m_iIdx = static_cast<_int>(distance(m_vecObjects.begin(), it));

	m_bAdded = true;
	Add_SelectedModels(strNumberedName, strFileLayer);
	Sort_SelectedModels();
	m_ModelDesc.emplace(strNumberedName, ModelDesc);
}

void CMap_Tool::Clone_Object()
{
	CToolMapObj::MODEL_DESC ModelDescToClone = m_pModelObject->Get_ModelDesc();
	m_strModelName = Convert_StrToWStr(ModelDescToClone.strName);

	if (m_ModelCounter.find(m_strModelName) == m_ModelCounter.end()) {
		m_ModelCounter[m_strModelName] = 0;
	}

	int& iNum = ++m_ModelCounter[m_strModelName];

	wstring strNumberedName;
	strNumberedName = m_strModelName + L"_" + to_wstring(iNum);
	_char szFileName[256] = { 0 };
	_char szFileNumbered[256] = { 0 };
	_char szFileLayer[256] = { 0 };
	wstring strLayer = L"Layer_";

	WideCharToMultiByte(CP_ACP, 0, m_strModelName.c_str(), lstrlenW(m_strModelName.c_str()), szFileName, MAX_PATH, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, strNumberedName.c_str(), lstrlenW(strNumberedName.c_str()), szFileNumbered, MAX_PATH, nullptr, nullptr);

	wstring strFileLayer = strLayer + Convert_StrToWStr(szFileName);
	WideCharToMultiByte(CP_ACP, 0, strFileLayer.c_str(), lstrlenW(strFileLayer.c_str()), szFileLayer, MAX_PATH, nullptr, nullptr);

	CToolMapObj::MODEL_DESC ModelDesc{};

	ModelDesc.matModel = ModelDescToClone.matModel;
	strcpy_s(ModelDesc.strName, ModelDescToClone.strName);
	strcpy_s(ModelDesc.strNumbered, szFileNumbered);
	strcpy_s(ModelDesc.strAddress, ModelDescToClone.strAddress);
	strcpy_s(ModelDesc.strFileLayer, ModelDescToClone.strFileLayer);
	ModelDesc.fRadius = ModelDescToClone.fRadius;
	ModelDesc.iObjFlag = ModelDescToClone.iObjFlag;

	if (!m_strModelName.empty())
	{
		_matrix WorldMatOfClone = m_pModelObject->Get_Transform()->Get_WorldMatrix();

		m_pModelObject->Show_Collider(false);
		m_pModelObject = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, strFileLayer, L"Prototype_ToolMapObj", &ModelDesc));
		Safe_AddRef(m_pModelObject);

		m_pModelObject->Get_Transform()->Set_WorldMatrix(WorldMatOfClone);
	}

	m_vecObjects.emplace_back(m_pModelObject);
	Sort_VecObjects();
	Add_SelectedModels(strNumberedName, strFileLayer);
	Sort_SelectedModels();

	m_ModelDesc.emplace(strNumberedName, ModelDesc);
}

void CMap_Tool::Generate_ListBox(const string& strFolder, const wstring& strFilePath, _bool bPicked)
{
	static _float3 vPos = { 0.f, 0.f, 0.f };
	static _float3 vScale = { 0.1f, 0.1f, 0.1f };
	static _float3 fRot = { 0.f, 0.f, 0.f };

	_char strConverted[MAX_PATH] = "";
	_float4x4 WorldMat, ViewMat, ProjMat;

	static _bool bInitialSelect = false;

	ImGui::SeparatorText("Select Model");
	static _int iLayerIndex = 0;
	static _int iCurIdxSelected = 0;
	static _float fRadius = 1.f;
	static _float fMipLevel = 0;

	const char** szModels = new const char* [vecModels.size()];
	string narrowStr{};
	for (size_t i = 0; i < vecModels.size(); i++) {
		narrowStr = Convert_WStrToStr(vecModels[i]);

		szModels[i] = new char[narrowStr.size() + 1];

		strcpy_s(const_cast<char*>(szModels[i]), narrowStr.size() + 1, narrowStr.c_str());
	}

	ImGui::PushItemWidth(210.f);
	ImGui::ListBox("##ModelListBox", &m_iCurIdx, szModels, (_int)vecModels.size(), 5);
	m_strModelName = vecModels[m_iCurIdx];
	ImGui::PopItemWidth();

	for (size_t i = 0; i < vecModels.size(); i++)
		Safe_Delete_Array(szModels[i]);

	Safe_Delete_Array(szModels);

	WideCharToMultiByte(CP_ACP, 0, m_strModelName.c_str(), lstrlen(m_strModelName.c_str()), strConverted, MAX_PATH, nullptr, nullptr);

	if (m_iIdx >= 0 && m_iIdx < vecSelectedModels.size())
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), " [ %d ] %s", m_iIdx, Convert_WStrToStr(vecSelectedModels[m_iIdx].first).c_str());

	if (nullptr != m_pModelObject)
	{
		CTransform* pModelTransform = m_pModelObject->Get_Transform();

		ViewMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
		ProjMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

		XMStoreFloat4x4(&WorldMat, XMLoadFloat4x4(&pModelTransform->Get_WorldFloat4x4()));

		XMStoreFloat3(&vPos, pModelTransform->Get_Position());
		vScale = pModelTransform->Get_Scale();

		ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &fRot.x, &vScale.x);
		ImGui::PushItemWidth(200.f);
		ImGui::SeparatorText("POSITION");
		ImGui::InputFloat3("##Pos", &vPos.x);

		ImGui::SeparatorText("ROTATION");
		ImGui::InputFloat3("##Rot", &fRot.x);

		ImGui::SeparatorText("SCALE");
		ImGui::InputFloat3("##Scale", &vScale.x);
		ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &fRot.x, &vScale.x, &WorldMat._11);

		if (!m_bGrid)
		{
			ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
				, m_tGuizmoDesc.CurrentGizmoOperation
				, m_tGuizmoDesc.CurrentGizmoMode
				, (_float*)&WorldMat
				, NULL
				, m_tGuizmoDesc.bUseSnap ? &m_tGuizmoDesc.snap[0] : NULL
				, m_tGuizmoDesc.boundSizing ? m_tGuizmoDesc.bounds : NULL
				, m_tGuizmoDesc.boundSizingSnap ? m_tGuizmoDesc.boundsSnap : NULL);
		}

		_fmatrix matModel = XMLoadFloat4x4(&WorldMat);
		pModelTransform->Set_WorldMatrix(matModel);

		fRadius = m_pModelObject->Get_Radius();
		fMipLevel = m_pModelObject->Get_MipLevel();

		ImGui::SeparatorText("RADIUS");

		if (KEY_PUSHING(eKeyCode::LAlt) && KEY_PUSHING(eKeyCode::Up))
			fRadius += 0.1f;

		if (KEY_PUSHING(eKeyCode::LAlt) && KEY_PUSHING(eKeyCode::Down))
			fRadius -= 0.1f;

		ImGui::DragFloat("##Radius", &fRadius, 0.1f, -100.f, 500.f);
		m_pModelObject->Set_Radius(fRadius);

		ImGui::SeparatorText("MIPLEVEL");
		ImGui::DragFloat("##MipLevel", &fMipLevel, 0.1f, -100.f, 100.f);
		m_pModelObject->Set_MipLevel(fMipLevel);

		ImGui::PopItemWidth();
	}

	if (ImGui::Button("Create") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::Enter)) {
		wstring strTmp = Convert_StrToWStr(strFolder);
		_vector vIdentity{};
		Create_Prototype(strConverted, strTmp, strFilePath, vIdentity);
	}

	if (KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::V)) {
		Clone_Object();
	}

	if (bPicked) {

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		if (false == io.WantCaptureMouse && KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::LButton))
		{
			wstring strTmp = Convert_StrToWStr(strFolder);
			Add_ByPicking(strConverted, strTmp, strFilePath);
		}

		if (false == io.WantCaptureMouse && KEY_DOWN(eKeyCode::LButton)) {
			wstring strTmp = Convert_StrToWStr(strFolder);

			if (!m_bGrid)
				Select_ByPicking();

			if (m_bGrid)
				Grid_Picking(strConverted, strTmp, strFilePath, false);
		}

		if (false == io.WantCaptureMouse && KEY_PUSHING(eKeyCode::MButton)) {
			wstring strTmp = Convert_StrToWStr(strFolder);

			if (m_bGrid)
				Grid_Picking(strConverted, strTmp, strFilePath, true);
		}

		if (KEY_DOWN(eKeyCode::R))
		{
			Grid_Picking();
		}
	}

	if (ImGui::CollapsingHeader("Added Model List"))
	{
		if (ImGui::BeginListBox("##AddedModels"))
		{
			for (_int i = 0; i < vecSelectedModels.size(); ++i) {
				const auto& modelFile = vecSelectedModels[i].first;
				const bool isSelected = (iCurIdxSelected == i);
				string strTemp = Convert_WStrToStr(modelFile);

				if (ImGui::Selectable(strTemp.c_str(), isSelected)) {
					m_pModelObject = m_vecObjects[iCurIdxSelected];
					iCurIdxSelected = i;

					m_strSelectedModel = modelFile;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}

	//ImGui::SameLine();
	if (ImGui::Button("Delete") || KEY_DOWN(eKeyCode::Delete))
	{
		Delete_ModelData(m_iIdx);
		bInitialSelect = false;
	}
}

void CMap_Tool::Model_ForTest()
{
	m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Model_TestCross", CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/MapObjects/Map_Boss/Land_MrBlackHole/", "Land_MrBH_CrossR.dat"));

	_float4x4 WorldMat;
	_char szFileName[256] = "TestCross";
	XMStoreFloat4x4(&WorldMat, XMMatrixTranslation(0.5f, 0.5f, 0.5f) * XMMatrixIdentity());

	CToolMapObj::MODEL_DESC ModelDesc{};

	ModelDesc.matModel = WorldMat;
	strcpy_s(ModelDesc.strName, szFileName);
	ModelDesc.iObjFlag = (1 << SHADOW);

	m_pTestObject = static_cast<CToolMapObj*>(m_pGameInstance->Clone_GameObject(L"Prototype_ToolMapObj", &ModelDesc));
}

void CMap_Tool::Show_TestModel(_bool bTest)
{
	static _bool bStart = true;

	if (!bTest)
	{
		if (m_pTestObject != nullptr)
		{
			Safe_Release(m_pTestObject);
			bStart = true;
			return;
		}
		else
			return;
	}

	if (bTest && bStart)
	{
		Model_ForTest();
		bStart = false;
	}

	static _float3 vPos = { 0.3f, 2.3f, 15.2f };
	_float4x4 WorldMat, ViewMat, ProjMat;
	static _float3 vScale = { 0.1f, 0.1f, 0.1f };
	static _float3 fRot = { 0.f, 0.f, 0.f };
	_float fTimeDelta{};

	if (ImGui::CollapsingHeader("TEST GUIZMO"))
	{
		if (nullptr != m_pTestObject)
		{
			CTransform* pTestModel = m_pTestObject->Get_Transform();

			ViewMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
			ProjMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

			XMStoreFloat4x4(&WorldMat, XMLoadFloat4x4(&pTestModel->Get_WorldFloat4x4()));

			if (!bStart)
				ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &fRot.x, &vScale.x);

			ImGui::SeparatorText("TEST MODEL POS");
			ImGui::InputFloat4("##TestModel Pos", &vPos.x, "%.2f");

			ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &fRot.x, &vScale.x, &WorldMat._11);

			ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11,
				m_tGuizmoDesc.CurrentGizmoOperation,
				m_tGuizmoDesc.CurrentGizmoMode,
				(_float*)&WorldMat,
				NULL,
				m_tGuizmoDesc.bUseSnap ? &m_tGuizmoDesc.snap[0] : NULL,
				m_tGuizmoDesc.boundSizing ? m_tGuizmoDesc.bounds : NULL,
				m_tGuizmoDesc.boundSizingSnap ? m_tGuizmoDesc.boundsSnap : NULL);

			_fmatrix matModel = XMLoadFloat4x4(&WorldMat);
			pTestModel->Set_WorldMatrix(matModel);
		}
	}
	if (nullptr != m_pTestObject)
	{
		m_pTestObject->LateTick(fTimeDelta);
		m_pTestObject->Show_Collider(false);
	}
}

void CMap_Tool::Show_Terrain(_bool bTerrain)
{
	if (bTerrain && m_pTerrain == nullptr)
	{
		m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, L"Terrain", L"Prototype_Terrain"));
		Safe_AddRef(m_pTerrain);
	}
	else if (!bTerrain && m_pTerrain)
	{
		m_pGameInstance->Clear_Layer(LEVEL_MAPTOOL, L"Terrain");
		Safe_Release(m_pTerrain);
	}
}

_bool CMap_Tool::NaturalSortCompare(const wstring& a, const wstring& b)
{
	size_t i = 0, j = 0;
	size_t len_a = a.length();
	size_t len_b = b.length();

	while (i < len_a && j < len_b) {
		if (isdigit(a[i]) && isdigit(b[j])) {
			size_t start_i = i;
			while (i < len_a && isdigit(a[i])) i++;
			size_t start_j = j;
			while (j < len_b && isdigit(b[j])) j++;

			long num_a = stol(a.substr(start_i, i - start_i));
			long num_b = stol(b.substr(start_j, j - start_j));

			if (num_a != num_b)
				return num_a < num_b;
		}
		else {
			if (a[i] != b[j])
				return a[i] < b[j];
			i++;
			j++;
		}
	}

	return a.length() < b.length();
}

void CMap_Tool::Sort_SelectedModels()
{
	sort(vecSelectedModels.begin(), vecSelectedModels.end(),
		[this](const pair<wstring, wstring>& a, const pair<wstring, wstring>& b) -> bool {
			return NaturalSortCompare(a.first, b.first);
		}
	);
}

void CMap_Tool::Sort_VecObjects()
{
	sort(m_vecObjects.begin(), m_vecObjects.end(),
		[this](const CToolMapObj* a, const CToolMapObj* b) {
			return NaturalSortCompare(a->m_strComponentTag, b->m_strComponentTag);
		}
	);
}

void CMap_Tool::Add_ByPicking(_char szConverted[MAX_PATH], wstring& strFolderName, const wstring& strFilePath)
{
	_float3 vPos = {};
	_vector vPickedPos = XMLoadFloat4(&m_pGameInstance->Get_PickedPos());
	XMStoreFloat3(&vPos, vPickedPos);

	if (vPos.x == 0.f)
		return;

	Create_Prototype(szConverted, strFolderName, strFilePath, vPickedPos);
}

void CMap_Tool::Grid_Picking(_char szConverted[MAX_PATH], wstring& strFolderName, const wstring& strFilePath, _bool bDrag)
{
	const _float fGridSize = 1.f;
	_float3 vPickedPos = {};
	_float	fGridStartX = {};
	_float	fGridStartY = {};
	_float	fGridEndX = {};
	_float	fGridEndY = {};
	_float4	vGridCenter = {};
	XMStoreFloat3(&vPickedPos, XMLoadFloat4(&m_pGameInstance->Get_PickedPos()));

	fGridStartX = floor(vPickedPos.x / fGridSize) * fGridSize;
	fGridStartY = floor(vPickedPos.y / fGridSize) * fGridSize;
	fGridEndX = fGridStartX + fGridSize;
	fGridEndY = fGridStartY + fGridSize;
	vGridCenter = { (fGridStartX + fGridEndX) / 2.f, (fGridStartY + fGridEndY) / 2.f, 0.f, 1.f };

	auto iter = std::find_if(m_listGridPos.begin(), m_listGridPos.end(),
		[&](const _float4& item) {
			return (fabs(item.x - vGridCenter.x) < 0.001f) &&
				(fabs(item.y - vGridCenter.y) < 0.001f);
		});

	cout << "Picked Pos: " << vPickedPos.x << ", " << vPickedPos.y << endl;
	cout << "Grid Center: " << vGridCenter.x << ", " << vGridCenter.y << endl;

	if (!bDrag)
	{
		if (iter == m_listGridPos.end())
		{
			Create_Prototype(szConverted, strFolderName, strFilePath, XMLoadFloat4(&vGridCenter));
			m_listGridPos.emplace_back(vGridCenter);
		}
		else
		{
			Select_ByPicking();
			Delete_ModelData(m_iIdx);
			m_listGridPos.erase(iter);
			m_iIdx = -1;
		}
	}
	else
	{
		if (iter == m_listGridPos.end())
		{
			Create_Prototype(szConverted, strFolderName, strFilePath, XMLoadFloat4(&vGridCenter));
			m_listGridPos.emplace_back(vGridCenter);
		}
	}
}

void CMap_Tool::Grid_Picking()
{
	_float3 vPickedPos = {};
	_float	fGridStartX = {};
	_float	fGridStartY = {};
	_float	fGridEndX = {};
	_float	fGridEndY = {};
	_float4	vGridCenter = {};
	_vector vecGridCenter = {};
	XMStoreFloat3(&vPickedPos, XMLoadFloat4(&m_pGameInstance->Get_PickedPos()));

	fGridStartX = floor(vPickedPos.x / 1.f) * 1.f;
	fGridStartY = floor(vPickedPos.y / 1.f) * 1.f;
	fGridEndX = fGridStartX + 1.f;
	fGridEndY = fGridStartY + 1.f;
	vGridCenter = { (fGridStartX + fGridEndX) / 2.f, (fGridStartY + fGridEndY) / 2.f, 0.f, 1.f };
	vecGridCenter = XMLoadFloat4(&vGridCenter);

	m_pGrass_Cube = static_cast<CGrass_Cube*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, L"Layer_Grass_Cube", L"Prototype_Grass_Cube"));
	Safe_AddRef(m_pGrass_Cube);
}

void CMap_Tool::Select_ByPicking()
{
	m_bPicked = true;

	_float4 vRGB = {};
	_float4 vTarget = { 1.f, 1.f, 1.f, 1.f };

	vRGB.x = roundf(m_pGameInstance->Get_RGBValue().x);
	vRGB.y = roundf(m_pGameInstance->Get_RGBValue().y);
	vRGB.z = roundf(m_pGameInstance->Get_RGBValue().z);
	vRGB.w = roundf(m_pGameInstance->Get_RGBValue().w);

	_vector vPickedValue = XMLoadFloat4(&vRGB);
	_vector vTargetValue = XMLoadFloat4(&vTarget);

	if (XMVector4Equal(vPickedValue, vTargetValue))
		return;

	auto it = std::find_if(m_vecObjects.begin(), m_vecObjects.end(), [&vPickedValue](const auto& obj) {
		_float4 vModelVal = obj->Get_RGBValue();
		_vector vModelValue = XMLoadFloat4(&vModelVal);
		return XMVector3Equal(vPickedValue, vModelValue);
		});

	if (it != m_vecObjects.end()) {
		if (nullptr != m_pModelObject)
			m_pModelObject->Show_Collider(false);

		m_iIdx = static_cast<_int>(distance(m_vecObjects.begin(), it));
		m_pModelObject = *it;
	}
}

void CMap_Tool::Trigger_Info(_uint iType, _bool bPicked)
{
	static _char strTag[MAX_PATH] = "";
	ImGui::SeparatorText("Trigger:");
	ImGui::InputText("##strTag", strTag, MAX_PATH);
	wstring strTagg = Convert_StrToWStr(strTag);

	static _float3 vPos = { 0.f, 0.f, 0.f };
	static _float3 vScale = { 1.f, 1.f, 1.f };
	static _float3 fRot = { 0.f, 0.f, 0.f };

	static _int iLayerIndex = 0;
	static _int iCurIdxSelected = 0;

	_float4x4 WorldMat, ViewMat, ProjMat;

	if (m_pTrigger != nullptr)
	{
		CTransform* pTriggerTransform = m_pTrigger->Get_Transform();

		ViewMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
		ProjMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

		XMStoreFloat4x4(&WorldMat, XMLoadFloat4x4(&pTriggerTransform->Get_WorldFloat4x4()));

		XMStoreFloat3(&vPos, pTriggerTransform->Get_Position());
		vScale = pTriggerTransform->Get_Scale();

		ImGuizmo::DecomposeMatrixToComponents(&WorldMat._11, &vPos.x, &fRot.x, &vScale.x);
		ImGui::PushItemWidth(200.f);
		ImGui::SeparatorText("Position:");
		ImGui::InputFloat3("##Pos", &vPos.x);

		ImGui::SeparatorText("Rotation:");
		ImGui::InputFloat3("##Rot", &fRot.x);

		ImGui::SeparatorText("Scale:");
		ImGui::InputFloat3("##Scale", &vScale.x);
		ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &fRot.x, &vScale.x, &WorldMat._11);

		ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
			, m_tGuizmoDesc.CurrentGizmoOperation
			, m_tGuizmoDesc.CurrentGizmoMode
			, (_float*)&WorldMat
			, NULL
			, m_tGuizmoDesc.bUseSnap ? &m_tGuizmoDesc.snap[0] : NULL
			, m_tGuizmoDesc.boundSizing ? m_tGuizmoDesc.bounds : NULL
			, m_tGuizmoDesc.boundSizingSnap ? m_tGuizmoDesc.boundsSnap : NULL);

		_fmatrix matModel = XMLoadFloat4x4(&WorldMat);
		pTriggerTransform->Set_WorldMatrix(matModel);

		ImGui::PopItemWidth();
	}

	if (ImGui::Button("Add") || KEY_DOWN(eKeyCode::Enter))
	{
		if (!strTagg.empty())
		{
			Add_Trigger(iType, strTagg);
			strcpy_s(strTag, "");
		}
		else
			MSG_BOX(L"Empty Name");
	}

	ImGui::SameLine();
	if (ImGui::Button("Save") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::S))
		Save_TriggerData();

	ImGui::SameLine();
	if (ImGui::Button("Load") || KEY_PUSHING(eKeyCode::LCtrl) && KEY_DOWN(eKeyCode::L))
		Load_TriggerData();

	if (ImGui::CollapsingHeader("Added Triggers"))
	{
		ImGui::SeparatorText("Triggers Added");

		if (ImGui::BeginListBox("  "))
		{
			for (_int i = 0; i < m_vecSelectedTriggers.size(); ++i) {
				const auto& modelFile = m_vecSelectedTriggers[i];
				const auto& modelLayer = m_vecSelectedTriggers[i];
				const bool isSelected = (iCurIdxSelected == i);
				string strTemp = Convert_WStrToStr(modelFile);

				if (ImGui::Selectable(strTemp.c_str(), isSelected)) {
					m_pTrigger = m_vecAddedTriggers[i];
					iCurIdxSelected = i;

					m_strSelectedTrigger = modelFile;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Delete") || KEY_DOWN(eKeyCode::Delete))
			Delete_Trigger(iCurIdxSelected);
	}
}

void CMap_Tool::Add_Trigger(_uint iType, wstring& strTag)
{
	_float4x4 WorldMat;
	XMStoreFloat4x4(&WorldMat, XMMatrixIdentity());

	CToolTriggerObj::TRIGGER_DESC* TriggerDesc = new CToolTriggerObj::TRIGGER_DESC;

	switch (iType)
	{
	case 0:
		TriggerDesc->eType = CToolTriggerObj::TRIGGER_AABB;
		break;
	case 1:
		TriggerDesc->eType = CToolTriggerObj::TRIGGER_OBB;
		break;
	case 2:
		TriggerDesc->eType = CToolTriggerObj::TRIGGER_Sphere;
		break;
	default:
		TriggerDesc->eType = CToolTriggerObj::TRIGGER_END;
		break;
	}

	TriggerDesc->matTrigger = WorldMat;
	TriggerDesc->strName = strTag;

	if (!strTag.empty())
	{
		m_pTrigger = static_cast<CToolTriggerObj*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, L"Layer_ToolTriggerObj", L"Prototype_ToolTriggerObj", TriggerDesc));
		Safe_AddRef(m_pTrigger);
	}

	m_vecAddedTriggers.emplace_back(m_pTrigger);
	m_vecSelectedTriggers.push_back(strTag);
}

void CMap_Tool::Delete_Trigger(_uint iCurIdxSelected)
{
	if (iCurIdxSelected >= m_vecSelectedTriggers.size())
		return;

	auto vecIter = m_vecSelectedTriggers.begin();
	std::advance(vecIter, iCurIdxSelected);
	wstring modelName = (*vecIter);

	m_vecSelectedTriggers.erase(vecIter);

	/*CGameObject* pObject = m_pGameInstance->Find_GameObject(LEVEL_MAPTOOL, L"Layer_ToolTriggerObj", iCurIdxSelected);
	pObject->Set_Destroy(true);
	m_pTrigger = nullptr;*/

	CGameObject* pObject = m_vecAddedTriggers[iCurIdxSelected];
	pObject->Set_Destroy(true);
	Safe_Release(m_pTrigger);

	auto ObjectIter = m_vecAddedTriggers.begin();
	std::advance(ObjectIter, iCurIdxSelected);
	m_vecAddedTriggers.erase(ObjectIter);
}

void CMap_Tool::Save_TriggerData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/DataFiles/TriggerData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_int iRet = { 0 };

	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Save File");
		return;
	}

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return;
	}

	_uint iModelCnt = (_uint)m_vecAddedTriggers.size();
	WriteFile(hFile, &iModelCnt, sizeof _uint, &dwByte, nullptr);

	if (!m_vecAddedTriggers.empty())
	{
		for (_uint i = 0; i < iModelCnt; ++i)
		{
			if (nullptr != m_vecAddedTriggers[i])
				m_vecAddedTriggers[i]->Save_TriggerData(hFile, dwByte);
		}
	}

	CloseHandle(hFile);

	return;
}

void CMap_Tool::Load_TriggerData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = nullptr;
	Ofn.nMaxFile = MAX_PATH;
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/DataFiles/TriggerData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return;
	}

	for (auto& iter : m_TriggerDesc)
		Safe_Delete(iter.second);
	m_TriggerDesc.clear();

	for (size_t i = 0; i < m_vecAddedTriggers.size(); ++i)
		Safe_Release(m_vecAddedTriggers[i]);
	m_vecAddedTriggers.clear();

	_uint iModelCnt = {};
	ReadFile(hFile, &iModelCnt, sizeof _uint, &dwByte, nullptr);

	while (iModelCnt)
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

		CToolTriggerObj::TRIGGER_DESC* pTriggerDesc = new CToolTriggerObj::TRIGGER_DESC;

		ReadFile(hFile, &WorldMatrix, sizeof _float4x4, &dwByte, nullptr);

		_uint iType = 0;
		DWORD iLen = 0;
		ReadFile(hFile, &iType, sizeof _uint, &dwByte, nullptr);
		ReadFile(hFile, &iLen, sizeof DWORD, &dwByte, nullptr);

		wstring strName(iLen, L'\0');
		ReadFile(hFile, &strName[0], iLen * sizeof _tchar, &dwByte, nullptr);

		pTriggerDesc->matTrigger = WorldMatrix;
		pTriggerDesc->eType = (CToolTriggerObj::TRIGGER_TYPE)iType;
		pTriggerDesc->strName = strName;

		m_TriggerDesc.emplace(strName, pTriggerDesc);

		--iModelCnt;
	}

	CloseHandle(hFile);

	for (auto& pTriggerDesc : m_TriggerDesc)
	{
		wstring strTag = pTriggerDesc.second->strName;

		if (!strTag.empty())
		{
			m_pTrigger = static_cast<CToolTriggerObj*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, L"Layer_ToolTriggerObj", L"Prototype_ToolTriggerObj", pTriggerDesc.second));
			Safe_AddRef(m_pTrigger);
		}

		m_vecAddedTriggers.emplace_back(m_pTrigger);
		m_pTrigger->Show_Trigger(true);
	}

	m_vecSelectedTriggers.clear();
	for (const auto& pTriggers : m_TriggerDesc)
		m_vecSelectedTriggers.push_back(pTriggers.first);

	return;
}

_uint CMap_Tool::Position_Info()
{
	static _char strTag[MAX_PATH] = "";
	_int	iFlag = 0;

	ImGui::SeparatorText("Position Tag:");
	iFlag += ImGui::InputText("##PosTag", strTag, MAX_PATH);

	if (iFlag)
		m_strPositionTag = strTag;

	ImGui::SeparatorText("Picked Info");
	static _uint iCurIdxSelected = -1;

	if (ImGui::BeginListBox("    ")) {
		_int iPosIndex = 0;
		for (const auto& vecPos : m_vecPositionData)
		{
			_char szBuffer[256];
			sprintf_s(szBuffer, "%d. [%s]: %.2f , %.2f , %.2f ",
				iPosIndex, vecPos.first.c_str(), vecPos.second.x, vecPos.second.y, vecPos.second.z);

			string strIndex = szBuffer;

			if (ImGui::Selectable(strIndex.c_str(), iCurIdxSelected == iPosIndex)) {
				iCurIdxSelected = iPosIndex; 
			}
			
			++iPosIndex;
		}
		ImGui::EndListBox();
	}

	return iCurIdxSelected;
}


_uint CMap_Tool::Add_PickedPosData(const string& strPositionTag)
{
	_float3 vPoints = {};

	_vector vPickedPos = XMVectorSetW(XMLoadFloat4(&m_pGameInstance->Get_PickedPos()), 1.f);
	XMStoreFloat3(&vPoints, vPickedPos);

	m_vecPositionData.emplace_back(strPositionTag, vPoints);

	return static_cast<_uint>(m_vecPositionData.size() - 1);
}

void CMap_Tool::Delete_PositionData(_uint iCurIdx)
{
	if (iCurIdx < m_vecPositionData.size())
		m_vecPositionData.erase(m_vecPositionData.begin() + iCurIdx);
	else
		return;
}

void CMap_Tool::Reset_PositionData()
{
	m_vecPositionData.clear();
}

HRESULT CMap_Tool::Save_PositionData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/DataFiles/PositionData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_int iRet = { 0 };

	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Save File");
		return E_FAIL;
	}

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return E_FAIL;
	}

	_int  iCount = (_int)m_vecPositionData.size();
		WriteFile(hFile, &iCount, sizeof(_int), &dwByte, nullptr);

	for (size_t i = 0; i < m_vecPositionData.size(); i++)
	{
		const auto& vPositionData = m_vecPositionData[i];
		DWORD dwLength = (DWORD)vPositionData.first.size();

		WriteFile(hFile, &dwLength, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, vPositionData.first.c_str(), dwLength, &dwByte, nullptr);
		WriteFile(hFile, &vPositionData.second, sizeof(_float3), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMap_Tool::Load_PositionData()
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
	_wfullpath(wszAbsolutePath, L"../../Resources/DataFiles/PositionData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;

	_int iRet = { 0 };
	iRet = GetOpenFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Load File");
		return E_FAIL;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return E_FAIL;
	}

	_int iCount = 0;

	ReadFile(hFile, &iCount, sizeof(_int), &dwByte, nullptr);

	while (iCount)
	{
		DWORD dwLength = 0;
		_char szName[MAX_PATH] = {};
		_float3 vPosition{};

		ReadFile(hFile, &dwLength, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, szName, dwLength, &dwByte, nullptr);
		ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);

		string strName(szName);

		m_vecPositionData.emplace_back(strName, vPosition);

		iCount--;
	}

	CloseHandle(hFile);

	return S_OK;
}

void CMap_Tool::Nav_Picking()
{
	_float3 vPoints = {};

	_vector vPickedPos = XMVectorSetW(XMLoadFloat4(&m_pGameInstance->Get_PickedPos()), 1.f);
	XMStoreFloat3(&vPoints, vPickedPos);

	vPoints = Snap(vPoints, m_vecNavigationPoints);
	m_vecPoints.push_back(vPoints);
	m_pNavigation->Tool_DrawPoint(vPoints, static_cast<_int>(m_vecPoints.size()));

	if (!m_vecPoints.empty() && m_vecPoints.size() % 3 == 0)
	{
		ClockWise(m_vecPoints);
		m_pNavigation->Tool_DrawNav(m_vecPoints);

		for (size_t i = 0; i < 3; i++)
			m_vecNavigationPoints.pop_back();

		for (size_t i = 0; i < 3; i++)
			m_vecNavigationPoints.push_back(m_vecPoints[i]);

		m_vecPoints.clear();
	}
}

_float3 CMap_Tool::Snap(_float3& vPointsToCompare, vector<_float3>& vecPoints)
{
	_float fTemp = {};
	_bool bIsSnapped = { false };
	for (int i = 0; i < vecPoints.size(); i++)
	{
		fTemp = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointsToCompare) - XMLoadFloat3(&vecPoints[i])));

		if (fTemp < 0.5f) //0.2
		{
			vPointsToCompare = vecPoints[i];
			vecPoints.push_back(vPointsToCompare);
			bIsSnapped = true;
			break;
		}
	}
	if (false == bIsSnapped)
		vecPoints.push_back(vPointsToCompare);

	return vPointsToCompare;
}

vector<_float3>& CMap_Tool::ClockWise(vector<_float3>& vecPoints)
{
	if (vecPoints.size() != 3)
		return vecPoints;

	if (!IsClockwise(vecPoints[0], vecPoints[1], vecPoints[2]))
		std::swap(vecPoints[1], vecPoints[2]);

	return vecPoints;
}

bool CMap_Tool::IsClockwise(const _float3& p1, const _float3& p2, const _float3& p3)
{
	_vector vecP1 = XMLoadFloat3(&p1);
	_vector vecP2 = XMLoadFloat3(&p2);
	_vector vecP3 = XMLoadFloat3(&p3);

	_vector vec1 = vecP2 - vecP1;
	_vector vec2 = vecP3 - vecP1;

	_vector vCross = XMVector3Cross(vec1, vec2);

	float fYComponent = XMVectorGetY(vCross);

	return fYComponent > 0;
}

HRESULT CMap_Tool::Save_NavigationData()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

	Ofn.lStructSize = sizeof OPENFILENAME;
	Ofn.hwndOwner = g_hWnd;
	Ofn.hInstance = NULL;
	Ofn.lpstrFile = szFilePath;
	Ofn.lpstrFilter = NULL;
	Ofn.nMaxFile = MAX_PATH;
	Ofn.lpstrDefExt = L"dat";
	_tchar wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/DataFiles/NavData/", MAX_PATH);
	Ofn.lpstrInitialDir = wszAbsolutePath;
	Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

	_int iRet = { 0 };

	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		MSG_BOX(L"Failed To Save File");
		return E_FAIL;
	}

	_ulong dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Save");
		return E_FAIL;
	}

	_float3 vPoints[3] = {};

	for (int i = 0; i < m_vecNavigationPoints.size(); i++) {
		vPoints[i % 3] = m_vecNavigationPoints[i];

		if ((i + 1) % 3 == 0)
			WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
	}

	if (m_vecNavigationPoints.size() % 3 != 0)
		WriteFile(hFile, vPoints, sizeof(_float3) * (m_vecNavigationPoints.size() % 3), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMap_Tool::Load_NavigationData()
{
	if (m_pNavigation == nullptr)
	{
		MSG_BOX(L"No Terrain");
		return E_FAIL;
	}

	m_pNavigation->Tool_LoadNav();
	m_vecNavigationPoints = m_pNavigation->Get_InitialNavData();

	return S_OK;
}

HRESULT CMap_Tool::Reset_NavigationData()
{
	m_pNavigation->Tool_ClearNav();

	m_vecNavigationPoints.clear();
	m_vecPoints.clear();

	return S_OK;
}

void CMap_Tool::IBLTextureInfo(_bool bActivate, const _tchar* strBrdf, const _tchar* strDiffuse, const _tchar* strSpecular)
{
	if (_tcslen(m_strBrdf) == 0 || _tcslen(m_strDiffuseHDR) == 0 || _tcslen(m_strSpecularHDR) == 0)
	{
		MSG_BOX(L"No Texture Applied");
		bActivate = false;
		return;
	}

	if (bActivate)
	{
		m_IBL.pIrradianceIBLTexture = CTexture::Create(m_pDevice, m_pContext, strDiffuse);
		m_IBL.pSpecularIBLTexture = CTexture::Create(m_pDevice, m_pContext, strSpecular);
		m_IBL.pBRDFTexture = CTexture::Create(m_pDevice, m_pContext, strBrdf);

		m_pGameInstance->Set_Active_IBL(true, &m_IBL);
	}
	else
		m_pGameInstance->Set_Active_IBL(false, &m_IBL);

	m_bNoIBLTexture = false;
	m_bNewIBLTexture = false;
}

HRESULT CMap_Tool::UpdateIBLTexture()
{
	TCHAR wszAbsolutePath[MAX_PATH] = {};
	_wfullpath(wszAbsolutePath, L"../../Resources/SkyBox/IBL_Resources/", MAX_PATH);

	ITEMIDLIST* pidlRoot = nullptr;
	if (FAILED(SHParseDisplayName(wszAbsolutePath, NULL, &pidlRoot, 0, NULL)))
	{
		MSG_BOX(L"Failed to parse display name");
		return E_FAIL;
	}

	BROWSEINFO browseInfo;
	ZeroMemory(&browseInfo, sizeof(BROWSEINFO));
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	browseInfo.hwndOwner = g_hWnd;
	browseInfo.pidlRoot = pidlRoot;

	TCHAR szPath[MAX_PATH];
	LPITEMIDLIST pItemIdList = SHBrowseForFolder(&browseInfo);

	if (pItemIdList != nullptr)
	{
		if (SHGetPathFromIDList(pItemIdList, szPath))
		{
			CoTaskMemFree(pItemIdList);
			CoTaskMemFree(pidlRoot);
		}
		else
		{
			CoTaskMemFree(pItemIdList);
			CoTaskMemFree(pidlRoot);
			MSG_BOX(L"Failed to get folder path");
			return E_FAIL;
		}
	}
	else
	{
		CoTaskMemFree(pidlRoot);
		MSG_BOX(L"Folder Selection Cancelled");
		return E_FAIL;
	}

	m_strBrdf[0] = '\0';
	m_strDiffuseHDR[0] = '\0';
	m_strSpecularHDR[0] = '\0';

	filesystem::path dirPath = szPath;
	filesystem::path basePath = "..\\..\\Resources\\SkyBox\\IBL_Resources\\";

	m_strIBLImage = dirPath.filename().wstring();

	for (const auto& entry : std::filesystem::directory_iterator(dirPath))
	{
		const auto& filePath = entry.path().wstring();
		wstring strRelativePath = basePath.wstring() + filesystem::relative(filePath, basePath).wstring();

		if (filePath.find(L"Brdf") != std::wstring::npos)
			wcscpy_s(m_strBrdf, strRelativePath.c_str());

		else if (filePath.find(L"Diffuse") != std::wstring::npos)
			wcscpy_s(m_strDiffuseHDR, strRelativePath.c_str());

		else if (filePath.find(L"Specular") != std::wstring::npos)
			wcscpy_s(m_strSpecularHDR, strRelativePath.c_str());
	}

	if (m_strBrdf[0] == '\0' || m_strDiffuseHDR[0] == '\0' || m_strSpecularHDR[0] == '\0')
	{
		MSG_BOX(L"One or more required files are missing");
		return E_FAIL;
	}

	m_bNewIBLTexture = true;

	return S_OK;
}

void CMap_Tool::Update_LightInfo()
{
	ImGui::SeparatorText("Diffuse");
	ImGui::DragFloat4("##Diffuse", &m_vDiffuse.x, 0.1f, -100.f, 100.f, "%.2f");
	m_LightDesc.vDiffuse = m_vDiffuse;

	ImGui::SeparatorText("Direction");
	ImGui::DragFloat4("##Direction", &m_vDirection.x, 0.1f, -100.f, 100.f, "%.2f");
	m_LightDesc.vDirection = m_vDirection;

	ImGui::SeparatorText("Light Strength");
	ImGui::DragFloat("##LightStrength ", &m_fLightStrength, 0.01f, -100.f, 100.f, "Light: %.2f");
	m_LightDesc.fLightStrength = m_fLightStrength;

	ImGui::SeparatorText("Ambient Strength");
	ImGui::DragFloat("##AmbientStrength ", &m_fAmbientStrength, 0.01f, -100.f, 100.f, "Ambient: %.2f");
	m_pGameInstance->Set_AmbientStrength(m_fAmbientStrength);
}

void CMap_Tool::Update_ShadowInfo()
{
	_int iFlag = 0;

	ImGui::SeparatorText("Shadow Bias");
	if (ImGui::DragFloat("##ShadowBias", &m_fShadowBias, 0.0000001f, -100.f, 100.f, "%.7f") || m_bLoadMap)
		m_pGameInstance->Set_ShadowBias(m_fShadowBias);

	ImGui::SeparatorText("Shadow Direction");
	iFlag += ImGui::DragFloat4("##ShadowDir", &m_vShadowDir.x, 0.01f, -100.f, 100.f, "%.2f");

	_float4x4 ViewMat, ProjMat;
	static _float3 vScale = { 0.1f, 0.1f, 0.1f };
	static _float3 fRot = { 0.f, 0.f, 0.f };

	ViewMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	ProjMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (!m_bLoadMap)
		ImGuizmo::DecomposeMatrixToComponents(&m_matShadowPos._11, &m_vShadowPos.x, &fRot.x, &vScale.x);

	ImGui::SeparatorText("Shadow Pos");
	iFlag += ImGui::DragFloat4("##ShadowPos", &m_vShadowPos.x, 0.01f, -100.f, 100.f, "%.2f");

	ImGuizmo::RecomposeMatrixFromComponents(&m_vShadowPos.x, &fRot.x, &vScale.x, &m_matShadowPos._11);

	iFlag += ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11,
		m_tGuizmoDesc.CurrentGizmoOperation,
		m_tGuizmoDesc.CurrentGizmoMode,
		(_float*)&m_matShadowPos,
		NULL,
		m_tGuizmoDesc.bUseSnap ? &m_tGuizmoDesc.snap[0] : NULL,
		m_tGuizmoDesc.boundSizing ? m_tGuizmoDesc.bounds : NULL,
		m_tGuizmoDesc.boundSizingSnap ? m_tGuizmoDesc.boundsSnap : NULL);

	if (iFlag || m_bLoadMap)
		m_pGameInstance->Update_ShadowViewProj(XMLoadFloat4(&m_vShadowDir), XMLoadFloat4(&m_vShadowPos));

	m_bLoadMap = false;
}

void CMap_Tool::Save_LightInfo(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &m_LightDesc.vDiffuse, sizeof _float4, &dwByte, nullptr);
	WriteFile(hFile, &m_LightDesc.vDirection, sizeof _float4, &dwByte, nullptr);
	WriteFile(hFile, &m_LightDesc.fLightStrength, sizeof _float, &dwByte, nullptr);
	WriteFile(hFile, &m_fAmbientStrength, sizeof _float, &dwByte, nullptr);
}

void CMap_Tool::Save_ShadowInfo(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &m_fShadowBias, sizeof _float, &dwByte, nullptr);
	WriteFile(hFile, &m_vShadowDir, sizeof _float4, &dwByte, nullptr);
	WriteFile(hFile, &m_vShadowPos, sizeof _float4, &dwByte, nullptr);
}

void CMap_Tool::Save_IBLInfo(HANDLE hFile, _ulong& dwByte)
{
	DWORD iLen1 = static_cast<DWORD>(wcslen(m_strBrdf) + 1);		// +1 for null
	DWORD iLen2 = static_cast<DWORD>(wcslen(m_strDiffuseHDR) + 1);
	DWORD iLen3 = static_cast<DWORD>(wcslen(m_strSpecularHDR) + 1);

	WriteFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, &iLen3, sizeof DWORD, &dwByte, nullptr);
	WriteFile(hFile, m_strBrdf, iLen1 * sizeof(wchar_t), &dwByte, nullptr);
	WriteFile(hFile, m_strDiffuseHDR, iLen2 * sizeof(wchar_t), &dwByte, nullptr);
	WriteFile(hFile, m_strSpecularHDR, iLen3 * sizeof(wchar_t), &dwByte, nullptr);
}

void CMap_Tool::Save_EnvInfo(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &m_bSky, sizeof _bool, &dwByte, nullptr);
	WriteFile(hFile, &m_bWater, sizeof _bool, &dwByte, nullptr);

	if (m_bSky)
		WriteFile(hFile, &m_SkyDesc, sizeof SKY_DESCS, &dwByte, nullptr);

	if (m_bWater)
	{
		WriteFile(hFile, &m_WaterDesc, sizeof WATER_DESCS, &dwByte, nullptr);
		WriteFile(hFile, &m_vWaterPos, sizeof _float4, &dwByte, nullptr);
		WriteFile(hFile, &m_vWaterScale, sizeof _float3, &dwByte, nullptr);
	}
}

void CMap_Tool::Add_Water(WATER_DESCS WaterDesc, _float4 vWaterPos, _float3 vWaterScale)
{
	if (nullptr == m_pWater)
	{
		m_pWater = static_cast<CWater*>(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, L"Water", L"Prototype_Water", &WaterDesc));
		Safe_AddRef(m_pWater);
	}

	m_pWater->m_pTransform->Set_Position(XMLoadFloat4(&vWaterPos));
	m_pWater->m_pTransform->Set_Scale(vWaterScale);
	m_pWater->Set_WaterDesc(WaterDesc);
}

void CMap_Tool::Remove_Water()
{
	if (m_pWater) {
		m_pGameInstance->Clear_Layer(LEVEL_MAPTOOL, L"Water");
		Safe_Release(m_pWater);
	}
}

vector<wstring> CMap_Tool::Find_Model(const wstring& strFilePath)
{
	vector<wstring> vecFileNames;
	wstring strPath = strFilePath + L"/";

	fs::path MeshPath(strPath);
	_tchar wszFilePP[MAX_PATH] = TEXT("");

	if (!lstrcmp(m_wstrDefaultDir.c_str(), TEXT("")))
	{
		GetCurrentDirectory(MAX_PATH, wszFilePP);
		m_wstrDefaultDir.append(wszFilePP);
	}

	SetCurrentDirectory(m_wstrDefaultDir.c_str());

	for (const fs::directory_entry& entry : fs::directory_iterator(MeshPath))
	{
		if (entry.is_directory())
			continue;
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();
			fs::path ext = fileName.extension();
			if (fileName.extension().compare(".dat"))
				continue;

			vecFileNames.push_back(fileTitle);
		}
	}

	return vecFileNames;

}

CMap_Tool* CMap_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMap_Tool* pInstance = new CMap_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMap_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap_Tool::Free()
{
	for (auto& Objects : m_vecObjects)
		Safe_Release(Objects);
	m_vecObjects.clear();

	for (auto& Pair : m_TriggerDesc)
		Safe_Delete(Pair.second);
	m_TriggerDesc.clear();

	for (auto& Triggers : m_vecAddedTriggers)
		Safe_Release(Triggers);
	m_vecAddedTriggers.clear();

	Safe_Release(m_IBL.pIrradianceIBLTexture);
	Safe_Release(m_IBL.pSpecularIBLTexture);
	Safe_Release(m_IBL.pBRDFTexture);
	Safe_Release(m_pWater);

	Safe_Release(m_pModelObject);
	Safe_Release(m_pTestObject);
	Safe_Release(m_pTrigger);
	Safe_Release(m_pTerrain);
	Safe_Release(m_pGrid);
	Safe_Release(m_pGrass_Cube);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}

/*

- Default Keys -

	[ P ] - Enable Picking
	[ C ] - Show Colliders
	[ T ] - Show Terrain
	[ M ] - Show Test Model
	[ I ] - Activate IBL
	[ N ] - Grass_Cube TestModel

	[ Space ] - Enable Shadow

	[ Alt + Up ] - Increase Radius
	[ Alt + Down ] - Decrease Radius

- Adding Models -

	[ Ctlr + Enter ] - Create
	[ Del ] - Delete

- Manipulating Guimo -
	[ Alt + 1 ] - Translation
	[ Alt + 2 ] - Rotation
	[ Alt + 3 ] - Scale

*/
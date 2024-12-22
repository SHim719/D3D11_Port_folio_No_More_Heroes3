#include "Cinema_Tool.h"
#include "CinemaObject.h"
#include "Cinema_Camera.h"

#include "Animation.h"
#include "GameInstance.h"

#include "Effect_Manager_Tool.h"


map<wstring, int> CCinema_Tool::m_ModelCounter;

CCinema_Tool::CCinema_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool_Super()
	, m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCinema_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	m_LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	m_LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Set_Active_Shadow(false);
	m_pGameInstance->Set_Active_Sky(false);

	return S_OK;
}

void CCinema_Tool::Start_Tool()
{
	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 100.f;
	camDesc.fFovy = 70.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	m_pCamera = static_cast<CCinema_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_CinemaCamera", &camDesc));
	m_pGameInstance->Change_MainCamera(m_pCamera);

	m_pCamera->Release();


	TOOL_CINEMATIC_DESCS CinemaDescs;
	CinemaDescs.strActorTag = "Camera";

	m_strModelTags.reserve(30);
	m_strModelTags.emplace_back("Travis");
	m_strModelTags.emplace_back("Travis_Space");
	m_strModelTags.emplace_back("Travis_World");
	m_strModelTags.emplace_back("Travis_Armor");
	m_strModelTags.emplace_back("Mbone");
	m_strModelTags.emplace_back("MrBlackhole");
	m_strModelTags.emplace_back("MrBlackhole_Space");
	m_strModelTags.emplace_back("Treatment");
	m_strModelTags.emplace_back("Fullface");
	m_strModelTags.emplace_back("Tripleput");
	m_strModelTags.emplace_back("SonicJuice");
	m_strModelTags.emplace_back("Travis_Bike");
	m_strModelTags.emplace_back("SmashBros_Damon");
	m_strModelTags.emplace_back("Leopardon");
	m_strModelTags.emplace_back("DestroymanTF");
	m_strModelTags.emplace_back("NPC_Jeane");

}

void CCinema_Tool::Tick(_float fTimeDelta)
{
	Main_Window();
	if (!m_bEditCamera)
		Anim_Window();
	Cinema_Window();

	if (m_bIsPlaying)
		Play_Cinematic(fTimeDelta);

}

void CCinema_Tool::Main_Window()
{
	ImGui::Begin("Main Window", (bool*)0, ImGuiWindowFlags_MenuBar);

	Menu_Bar();
	Tab_Bar();

	ImGui::End();
}

void CCinema_Tool::Menu_Bar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Cinema Data"))
		{
			if (ImGui::MenuItem("Load CinemaData"))
			{
				Load_CinematicData();
			}

			if (ImGui::MenuItem("Save CinemaData"))
			{
				Save_CinematicData();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Map"))
		{
			if (ImGui::MenuItem("Load Map"))
			{
				Load_MapData();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Sound"))
		{
			if (ImGui::MenuItem("Load Sound"))
			{
				Load_SoundData();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Effect"))
		{
			if (ImGui::MenuItem("Load Effect"))
			{
				Load_EffectData();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void CCinema_Tool::Tab_Bar()
{
	if (ImGui::BeginTabBar("Cinema"))
	{
		if (ImGui::BeginTabItem("Actors"))
		{
			Actor_KeyInput();
			Actor_Gizmo();
			Actor_ListBox();
			Actor_Buttons();
			Actor_Info();

			Actor_AddCinemaKey();

			if (!m_CinemaObjects.empty())
				Reset_Transform(m_CinemaObjects[m_iCinemaObjIdx]->Get_Transform()->Get_WorldMatrix());

			ImGui::EndTabItem();
		}

		if (m_bEditCamera = ImGui::BeginTabItem("Camera"))
		{
			Actor_Transform();
			Actor_AddCinemaKey();

			Reset_Transform(m_pCamera->Get_Transform()->Get_WorldMatrix());

			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}

}

void CCinema_Tool::Actor_KeyInput()
{
	if (KEY_DOWN(eKeyCode::Delete) && !m_CinemaObjects.empty())
	{
		auto it = m_CinemaObjects.begin();
		advance(it, m_iCinemaObjIdx);

		(*it)->Set_Destroy(true);
		m_CinemaObjects.erase(it);
		m_iCinemaObjIdx = 0;
	}

}

void CCinema_Tool::Actor_ListBox()
{
	ImGui::SeparatorText("Cinema Actors");

	const _char** szActors = new const _char* [m_CinemaObjects.size()];
	for (size_t i = 0; i < m_CinemaObjects.size(); ++i)
		szActors[i] = m_CinemaObjects[i]->Get_CinemaDesc().strActorTag.c_str();

	if (ImGui::ListBox("##Actor_ListBox", &m_iCinemaObjIdx, szActors, (_int)m_CinemaObjects.size(), 10))
	{
		Reset_Transform(m_CinemaObjects[m_iCinemaObjIdx]->Get_Transform()->Get_WorldMatrix());
	}

	Safe_Delete_Array(szActors);
}

void CCinema_Tool::Actor_Buttons()
{
	ImGui::InputText("ActorTag", m_szActorTag, MAX_PATH);

	if (ImGui::Button("Create"))
		Create_NewActor();

}

void CCinema_Tool::Actor_Info()
{
	ImGui::SeparatorText("Actor Info");

	if (m_iCinemaObjIdx < 0)
		return;

	Actor_Transform();
	Actor_ModelInfo();
}

void CCinema_Tool::Actor_Transform()
{
	if (!m_bEditCamera && m_CinemaObjects.empty())
		return;

	CGameObject* pTransformObj = m_bEditCamera == true ? m_pCamera : static_cast<CGameObject*>(m_CinemaObjects[m_iCinemaObjIdx]);

	if (ImGui::InputFloat3("Position", &m_vPosition.x))
	{
		pTransformObj->Get_Transform()->Set_Position(XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	}

	if (ImGui::InputFloat3("Look", &m_vLook.x))
	{
		pTransformObj->Get_Transform()->LookTo(XMLoadFloat3(&m_vLook));
	}

	if (ImGui::InputFloat3("Scale", &m_vScale.x))
	{
		pTransformObj->Get_Transform()->Set_Scale(m_vScale);
	}

}

void CCinema_Tool::Actor_ModelInfo()
{
	if (m_CinemaObjects.empty())
		return;

	const _char** szModels = new const _char * [m_strModelTags.size()];
	for (size_t i = 0; i < m_strModelTags.size(); ++i)
		szModels[i] = m_strModelTags[i].c_str();

	ImGui::Text("Models ");
	ImGui::SameLine();

	ImGui::Combo("##ModelCombo", &m_iSelModelIdx, szModels, (_int)m_strModelTags.size());

	if (ImGui::Button("Select"))
	{
		CCinemaObject* pObj = static_cast<CCinemaObject*>(m_CinemaObjects[m_iCinemaObjIdx]);
		_int iModelIdx = pObj->Get_ModelIdx();
		if (iModelIdx != m_iSelModelIdx)
		{
			CModel* pModel = static_cast<CModel*>(m_pGameInstance->Clone_Component(GET_CURLEVEL,
				L"Prototype_Model_" + Convert_StrToWStr(m_strModelTags[m_iSelModelIdx])));

			if (nullptr == pModel)
				return;

			pObj->Change_Model(pModel);
			pObj->Set_ModelIdx(m_iSelModelIdx);
		}
	}

	Safe_Delete_Array(szModels);
}

void CCinema_Tool::Actor_AnimListBox()
{
	if (m_bEditCamera || m_CinemaObjects.empty() || m_iCinemaObjIdx >= (_int)m_CinemaObjects.size())
		return;

	ImGui::SeparatorText("Animations");

	CModel* pModel = static_cast<CCinemaObject*>(m_CinemaObjects[m_iCinemaObjIdx])->Get_Model();
	if (nullptr == pModel)
		return;

	const vector<CAnimation*>& Anims = pModel->Get_Animations();

	const _char** szAnimations = new const _char * [Anims.size()];
	for (size_t i = 0; i < Anims.size(); ++i)
		szAnimations[i] = Anims[i]->Get_AnimName().c_str();

	_int iAnimIdx = 0;

	ImGui::PushItemWidth(350);
	if (ImGui::ListBox("##Anim_ListBox", &iAnimIdx, szAnimations, (_int)Anims.size(), 10))
	{
		pModel->Change_Animation((_uint)iAnimIdx);
	}
	ImGui::PopItemWidth();

	Safe_Delete_Array(szAnimations);
}

void CCinema_Tool::Anim_Buttons()
{
	if (m_bEditCamera || m_CinemaObjects.empty() || m_iCinemaObjIdx >= (_int)m_CinemaObjects.size())
		return;

	if (ImGui::Button("Play"))
	{
		CModel* pModel = static_cast<CCinemaObject*>(m_CinemaObjects[m_iCinemaObjIdx])->Get_Model();
		if (nullptr == pModel)
			return;

		pModel->Set_AnimPlay();
	}

	ImGui::SameLine();

	if (ImGui::Button("Stop"))
	{
		CModel* pModel = static_cast<CCinemaObject*>(m_CinemaObjects[m_iCinemaObjIdx])->Get_Model();
		if (nullptr == pModel)
			return;

		pModel->Set_AnimPause();
	}

}

void CCinema_Tool::Cinema_Window()
{
	ImGui::Begin("Cinema Window", (bool*)0);

	ImGui::InputFloat("PlayTime", &m_fPlayTime);
	ImGui::SliderFloat("Now_PlayTime", &m_fPlayTimeAcc, 0.f, m_fPlayTime);
	ImGui::InputFloat("Now_PlayTIme", &m_fPlayTimeAcc);

	if (ImGui::Button("Play"))
	{
		Start_Cinematic();
	}
		
	
	if (ImGui::Button("Stop"))
	{
		End_Cinematic();
	}


	Cinema_ListBox();
	Cinema_FrameDesc();

	ImGui::End();
}

void CCinema_Tool::Cinema_ListBox()
{
	ImGui::SeparatorText("Cinema Key");

	ImGui::Combo("Key Type", (_int*)&m_eKeyToListBox, m_szKeyTypes, KEY_END);

	switch (m_eKeyToListBox)
	{
	case SCALE:
		Scale_ListBox();
		break;
	case LOOK:
		Look_ListBox();
		break;
	case POSITION:
		Position_ListBox();
		break;
	case EVENT:
		Event_ListBox();
		break;
	}
}

void CCinema_Tool::Cinema_FrameDesc()
{
	ImGui::SeparatorText("Cinema FrameDesc");

	if (!m_bEditCamera && m_CinemaObjects.empty())
		return;

	TOOL_CINEMATIC_DESCS& CinemaDesc = m_bEditCamera == true ? m_pCamera->Get_CinemaDesc() : m_CinemaObjects[m_iCinemaObjIdx]->Get_CinemaDesc();

	ImGui::PushItemWidth(80.f);
	ImGui::DragFloat("TimeKey", &m_fNowKeyTime, 0.1f, 0.f, 999999.f);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	
	switch (m_eKeyToListBox)
	{
	case SCALE:
	{
		auto it = CinemaDesc.Scales.begin();
		advance(it, m_iSelectKey);
		if (CinemaDesc.Scales.end() == it)
			return;

		if (ImGui::Button("Edit Key"))
		{
			CinemaDesc.Scales[m_fNowKeyTime] = it->second;
			CinemaDesc.Scales.erase(it);
			m_iSelectKey = 0;
			return;
		}


		ImGui::DragFloat3("Scale", (_float*)&it->second.first, 0.1f, -99999.f, 99999.f);
		ImGui::Checkbox("Lerp", &it->second.second);
		break;
	}	
	case LOOK:
	{
		auto it = CinemaDesc.Looks.begin();
		advance(it, m_iSelectKey);
		if (CinemaDesc.Looks.end() == it)
			return;

		if (ImGui::Button("Edit Key"))
		{
			CinemaDesc.Looks[m_fNowKeyTime] = it->second;
			CinemaDesc.Looks.erase(it);
			m_iSelectKey = 0;
			return;
		}

		ImGui::DragFloat3("Look", (_float*)&it->second.first, 0.1f, -99999.f, 99999.f);
		ImGui::Checkbox("Lerp", &it->second.second);
		break;
	}
		
	case POSITION:
	{
		auto it = CinemaDesc.Positions.begin();
		advance(it, m_iSelectKey);
		if (CinemaDesc.Positions.end() == it)
			return;

		if (ImGui::Button("Edit Key"))
		{
			CinemaDesc.Positions[m_fNowKeyTime] = it->second;
			CinemaDesc.Positions.erase(it);
			m_iSelectKey = 0;
			return;
		}

		ImGui::DragFloat3("Pos", (_float*)&it->second.first, 0.1f, -99999.f, 99999.f);
		ImGui::Checkbox("Lerp", &it->second.second);
		break;
	}
		
	case EVENT:
	{
		auto it = CinemaDesc.Events.begin();
		advance(it, m_iSelectKey);
		if (CinemaDesc.Events.end() == it)
			return;

		if (ImGui::Button("Edit Key"))
		{
			CinemaDesc.Events[m_fNowKeyTime] = it->second;
			CinemaDesc.Events.erase(it);
			m_iSelectKey = 0;
			return;
		}


		ImGui::Combo("Event Type", (_int*)&it->second.eEventType, m_szCinemaEventTypes, CINEMA_EVENT_END);
		ImGui::InputText("Event Param", m_szEventParam, MAX_PATH);

		if (ImGui::Button("Edit Event"))
		{
			switch (it->second.eEventType)
			{
			case CHANGE_ANIMATION:
			{
				CModel* pModel = m_CinemaObjects[m_iCinemaObjIdx]->Get_Model();
				it->second.strParam = to_string(pModel->Get_CurrentAnimIndex());
				it->second.pFunc = bind(&CModel::Change_Animation, pModel, pModel->Get_CurrentAnimIndex(), 0.1f, true);
				break;
			}
				
			case PLAY_SOUND:
			{
				it->second.strParam = string(m_szEventParam);
				it->second.pFunc = bind(&CGameInstance::Play, m_pGameInstance, it->second.strParam, false, 1.f);
				break;
			}
			
			case STOP_SOUND:
			{
				it->second.strParam = string(m_szEventParam);
				it->second.pFunc = bind(&CGameInstance::Stop, m_pGameInstance, it->second.strParam);
				break;
			}
				
			case ACTIVE_EFFECT:
			{
				it->second.strParam = string(m_szEventParam);
				it->second.pFunc = bind(&CEffect_Manager_Tool::Active_Effect, EFFECTMGR, LEVEL_CINEMATOOL, it->second.strParam, m_CinemaObjects[m_iCinemaObjIdx]->Get_CineEffectDesc(), nullptr);
				break;
			}
				
			case INACITVE_EFFECT:
			{
				it->second.strParam = string(m_szEventParam);
				it->second.pFunc = bind(&CEffect_Manager_Tool::End_Effect, EFFECTMGR, LEVEL_CINEMATOOL, it->second.strParam, 0);

				break;
			}
			}
		}
		break;
	}
	}


}

void CCinema_Tool::Scale_ListBox()
{
	if (!m_bEditCamera && m_CinemaObjects.empty())
		return;
	
	TOOL_CINEMATIC_DESCS& CinemaDesc = m_bEditCamera == true ? m_pCamera->Get_CinemaDesc() : m_CinemaObjects[m_iCinemaObjIdx]->Get_CinemaDesc();
	CTransform* pTransform = m_bEditCamera == true ? m_pCamera->Get_Transform() : m_CinemaObjects[m_iCinemaObjIdx]->Get_Transform();

	if (m_iSelectKey >= (_int)CinemaDesc.Scales.size())
		m_iSelectKey = 0;

	vector<string> strScales;
	strScales.resize(CinemaDesc.Scales.size());

	size_t iIdx = 0;
	for (auto it = CinemaDesc.Scales.begin(); it != CinemaDesc.Scales.end(); ++it)
	{
		strScales[iIdx++] = "< " + to_string_with_precision(it->first, 2) + "s, " + to_string_with_precision(it->second.first.x, 2) + " " + to_string_with_precision(it->second.first.y, 2) + " " +
			to_string_with_precision(it->second.first.z, 2) + " >";
	}
		
	const _char** szScales = new const _char * [strScales.size()];
	for (size_t i = 0; i < strScales.size(); ++i)
		szScales[i] = strScales[i].c_str();

	if (ImGui::ListBox("##Scale_ListBox", (_int*)&m_iSelectKey, szScales, (_int)strScales.size(), 5))
	{
		auto it = CinemaDesc.Scales.begin();
		advance(it, m_iSelectKey);

		m_fNowKeyTime = it->first;

		pTransform->Set_Scale(it->second.first);
		Reset_Transform(pTransform->Get_WorldMatrix());
	}

	if (ImGui::Button("Delete") && !CinemaDesc.Scales.empty())
	{
		auto it = CinemaDesc.Scales.begin();
		advance(it, m_iSelectKey);

		CinemaDesc.Scales.erase(it);
	}

	Safe_Delete_Array(szScales);
}

void CCinema_Tool::Look_ListBox()
{
	if (!m_bEditCamera && m_CinemaObjects.empty())
		return;

	TOOL_CINEMATIC_DESCS& CinemaDesc = m_bEditCamera == true ? m_pCamera->Get_CinemaDesc() : m_CinemaObjects[m_iCinemaObjIdx]->Get_CinemaDesc();
	CTransform* pTransform = m_bEditCamera == true ? m_pCamera->Get_Transform() : m_CinemaObjects[m_iCinemaObjIdx]->Get_Transform();

	if (m_iSelectKey >= (_int)CinemaDesc.Looks.size())
		m_iSelectKey = 0;

	vector<string> strLooks;
	strLooks.resize(CinemaDesc.Looks.size());

	size_t iIdx = 0;
	for (auto it = CinemaDesc.Looks.begin(); it != CinemaDesc.Looks.end(); ++it)
	{
		strLooks[iIdx++] = "< " + to_string_with_precision(it->first, 2) + "s, " + to_string_with_precision(it->second.first.x, 2) + " " + to_string_with_precision(it->second.first.y, 2) + " " +
			to_string_with_precision(it->second.first.z, 2) + " >";
	}

	const _char** szLooks = new const _char * [strLooks.size()];
	for (size_t i = 0; i < strLooks.size(); ++i)
		szLooks[i] = strLooks[i].c_str();

	if (ImGui::ListBox("##Look_ListBox", (_int*)&m_iSelectKey, szLooks, (_int)strLooks.size(), 5))
	{
		auto it = CinemaDesc.Looks.begin();
		advance(it, m_iSelectKey);

		m_fNowKeyTime = it->first;

		pTransform->LookTo(XMLoadFloat3(&it->second.first));
		Reset_Transform(pTransform->Get_WorldMatrix());
	}

	if (ImGui::Button("Delete") && !CinemaDesc.Looks.empty())
	{
		auto it = CinemaDesc.Looks.begin();
		advance(it, m_iSelectKey);

		CinemaDesc.Looks.erase(it);
	}

	Safe_Delete_Array(szLooks);

}

void CCinema_Tool::Position_ListBox()
{
	if (!m_bEditCamera && m_CinemaObjects.empty())
		return;

	TOOL_CINEMATIC_DESCS& CinemaDesc = m_bEditCamera == true ? m_pCamera->Get_CinemaDesc() : m_CinemaObjects[m_iCinemaObjIdx]->Get_CinemaDesc();
	CTransform* pTransform = m_bEditCamera == true ? m_pCamera->Get_Transform() : m_CinemaObjects[m_iCinemaObjIdx]->Get_Transform();

	if (m_iSelectKey >= (_int)CinemaDesc.Positions.size())
		m_iSelectKey = 0;

	vector<string> strPositions;
	strPositions.resize(CinemaDesc.Positions.size());

	size_t iIdx = 0;
	for (auto it = CinemaDesc.Positions.begin(); it != CinemaDesc.Positions.end(); ++it)
	{
		strPositions[iIdx++] = "< " + to_string_with_precision(it->first, 2) + "s, " + to_string_with_precision(it->second.first.x, 2) + " " + to_string_with_precision(it->second.first.y, 2) + " " +
			to_string_with_precision(it->second.first.z, 2) + " >";
	}

	const _char** szPositions = new const _char * [strPositions.size()];
	for (size_t i = 0; i < strPositions.size(); ++i)
		szPositions[i] = strPositions[i].c_str();

	if (ImGui::ListBox("##Position_ListBox", (_int*)&m_iSelectKey, szPositions, (_int)strPositions.size(), 5))
	{
		auto it = CinemaDesc.Positions.begin();
		advance(it, m_iSelectKey);

		m_fNowKeyTime = it->first;

		_vector vPosition = XMLoadFloat3(&it->second.first);
		vPosition.m128_f32[3] = 1.f;
		pTransform->Set_Position(vPosition);
		Reset_Transform(pTransform->Get_WorldMatrix());
	}

	if (ImGui::Button("Delete") && !CinemaDesc.Positions.empty())
	{
		auto it = CinemaDesc.Positions.begin();
		advance(it, m_iSelectKey);

		CinemaDesc.Positions.erase(it);
	}

	Safe_Delete_Array(szPositions);

}

void CCinema_Tool::Event_ListBox()
{
	if (!m_bEditCamera && m_CinemaObjects.empty())
		return;

	TOOL_CINEMATIC_DESCS& CinemaDesc = m_bEditCamera == true ? m_pCamera->Get_CinemaDesc() : m_CinemaObjects[m_iCinemaObjIdx]->Get_CinemaDesc();

	if (m_iSelectKey >= (_int)CinemaDesc.Events.size())
		m_iSelectKey = 0;

	vector<string> strEvents;
	strEvents.resize(CinemaDesc.Events.size());

	size_t iIdx = 0;
	for (auto it = CinemaDesc.Events.begin(); it != CinemaDesc.Events.end(); ++it)
	{
		strEvents[iIdx] = "< " + to_string_with_precision(it->first, 2) + "s, ";

		switch (it->second.eEventType)
		{
		case CHANGE_ANIMATION:
			strEvents[iIdx] += "Change Anim >";
			break;
		case PLAY_SOUND:
			strEvents[iIdx] += "Play Sound >";
			break;
		case STOP_SOUND:
			strEvents[iIdx] += "Stop Sound >";
			break;
		case ACTIVE_EFFECT:
			strEvents[iIdx] += "Active Effect >";
			break;
		case INACITVE_EFFECT:
			strEvents[iIdx] += "Inactive Effect >";
			break;
		case ETC:
			strEvents[iIdx] += "Etc >";
			break;
		}

		iIdx += 1;
	}

	const _char** szEvents = new const _char * [strEvents.size()];
	for (size_t i = 0; i < strEvents.size(); ++i)
		szEvents[i] = strEvents[i].c_str();

	if (ImGui::ListBox("#Event_ListBox", (_int*)&m_iSelectKey, szEvents, (_int)strEvents.size(), 5))
	{
		auto it = CinemaDesc.Events.begin();
		advance(it, m_iSelectKey);
		strcpy_s(m_szEventParam, it->second.strParam.c_str());

		m_fNowKeyTime = it->first;
	}

	if (ImGui::Button("Delete") && !CinemaDesc.Events.empty())
	{
		auto it = CinemaDesc.Events.begin();
		advance(it, m_iSelectKey);

		CinemaDesc.Events.erase(it);
	}

	Safe_Delete_Array(szEvents);

}

void CCinema_Tool::Play_Cinematic(_float fTimeDelta)
{
	m_fPlayTimeAcc += fTimeDelta;
	if (m_fPlayTimeAcc >= m_fPlayTime)
		m_bIsPlaying = false;

	m_pCamera->Update_PlayTime(m_fPlayTimeAcc);

	for (CCinemaObject*& pCinemaObject : m_CinemaObjects)
		pCinemaObject->Update_PlayTime(m_fPlayTimeAcc);

}

void CCinema_Tool::Start_Cinematic()
{
	m_bIsPlaying = true;

	m_pCamera->Play_Cinematic();
	m_pCamera->Update_PlayTime(m_fPlayTimeAcc);

	for (CCinemaObject*& pCinemaObject : m_CinemaObjects)
	{
		pCinemaObject->Play_Cinematic();
		pCinemaObject->Update_PlayTime(m_fPlayTimeAcc);
	}

	EFFECTMGR->End_AllEffects(LEVEL_CINEMATOOL);
	
}

void CCinema_Tool::End_Cinematic()
{
	m_bIsPlaying = false;

	m_pCamera->End_Cinematic();

	for (CCinemaObject*& pCinemaObject : m_CinemaObjects)
	{
		pCinemaObject->End_Cinematic();
	}
}

HRESULT CCinema_Tool::Save_CinematicData()
{
	_tchar szAbsolutePath[MAX_PATH] = {};
	_wfullpath(szAbsolutePath, L"../../Resources/CinematicData/", MAX_PATH);
	_tchar szFullPath[20000] = {};
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFullPath;
	ofn.nMaxFile = sizeof(szFullPath);
	ofn.lpstrFilter = L"*.json";
	ofn.lpstrInitialDir = szAbsolutePath;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn))
	{
		ofstream fout(szFullPath);

		json jsonData;

		jsonData["PlayTime"] = m_fPlayTime;

		TOOL_CINEMATIC_DESCS& CamCinemaDesc = m_pCamera->Get_CinemaDesc();

		for (auto it = CamCinemaDesc.Positions.begin(); it != CamCinemaDesc.Positions.end(); ++it)
		{
			jsonData["Camera Desc"]["Positions"]["Time"].push_back(it->first);
			jsonData["Camera Desc"]["Positions"]["Position"].push_back({ it->second.first.x, it->second.first.y, it->second.first.z });
			jsonData["Camera Desc"]["Positions"]["Lerp?"].push_back(it->second.second);
		}

		for (auto it = CamCinemaDesc.Looks.begin(); it != CamCinemaDesc.Looks.end(); ++it)
		{
			jsonData["Camera Desc"]["Looks"]["Time"].push_back(it->first);
			jsonData["Camera Desc"]["Looks"]["Look"].push_back({ it->second.first.x, it->second.first.y, it->second.first.z });
			jsonData["Camera Desc"]["Looks"]["Lerp?"].push_back(it->second.second);
		}

		for (auto it = CamCinemaDesc.Events.begin(); it != CamCinemaDesc.Events.end(); ++it)
		{
			jsonData["Camera Desc"]["Events"]["Time"].push_back(it->first);
			jsonData["Camera Desc"]["Events"]["EventType"].push_back(it->second.eEventType);
			jsonData["Camera Desc"]["Events"]["Param"].push_back(it->second.strParam);
		}

		for (CCinemaObject*& pCinemaObj : m_CinemaObjects)
		{
			TOOL_CINEMATIC_DESCS& CinemaDesc = pCinemaObj->Get_CinemaDesc();

			jsonData["ActorTag"].push_back(CinemaDesc.strActorTag);

			jsonData[CinemaDesc.strActorTag]["ModelIndex"] = pCinemaObj->Get_ModelIdx();

			for (auto it = CinemaDesc.Positions.begin(); it != CinemaDesc.Positions.end(); ++it)
			{
				jsonData[CinemaDesc.strActorTag]["Positions"]["Time"].push_back(it->first);
				jsonData[CinemaDesc.strActorTag]["Positions"]["Position"].push_back({ it->second.first.x, it->second.first.y, it->second.first.z });
				jsonData[CinemaDesc.strActorTag]["Positions"]["Lerp?"].push_back(it->second.second);
			}

			for (auto it = CinemaDesc.Looks.begin(); it != CinemaDesc.Looks.end(); ++it)
			{
				jsonData[CinemaDesc.strActorTag]["Looks"]["Time"].push_back(it->first);
				jsonData[CinemaDesc.strActorTag]["Looks"]["Look"].push_back({ it->second.first.x, it->second.first.y, it->second.first.z });
				jsonData[CinemaDesc.strActorTag]["Looks"]["Lerp?"].push_back(it->second.second);
			}

			for (auto it = CinemaDesc.Scales.begin(); it != CinemaDesc.Scales.end(); ++it)
			{
				jsonData[CinemaDesc.strActorTag]["Scales"]["Time"].push_back(it->first);
				jsonData[CinemaDesc.strActorTag]["Scales"]["Scale"].push_back({ it->second.first.x, it->second.first.y, it->second.first.z });
				jsonData[CinemaDesc.strActorTag]["Scales"]["Lerp?"].push_back(it->second.second);
			}

			for (auto it = CinemaDesc.Events.begin(); it != CinemaDesc.Events.end(); ++it)
			{
				jsonData[CinemaDesc.strActorTag]["Events"]["Time"].push_back(it->first);
				jsonData[CinemaDesc.strActorTag]["Events"]["EventType"].push_back(it->second.eEventType);
				jsonData[CinemaDesc.strActorTag]["Events"]["Param"].push_back(it->second.strParam);
			}
		}

		fout << jsonData.dump(0);
		fout.close();
	}

	return S_OK;
}

HRESULT CCinema_Tool::Load_CinematicData()
{
	for (auto& pCinemaObj : m_CinemaObjects)
		pCinemaObj->Set_Destroy(true);

	m_CinemaObjects.clear();

	_tchar szAbsolutePath[MAX_PATH] = {};
	_wfullpath(szAbsolutePath, L"../../Resources/CinematicData/", MAX_PATH);
	_tchar szFullPath[20000] = {};
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFullPath;
	ofn.nMaxFile = sizeof(szFullPath);
	ofn.lpstrFilter = L"*.json";
	ofn.lpstrInitialDir = szAbsolutePath;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn))
	{
		ifstream fin(szFullPath);

		json jsonData;

		fin >> jsonData;
		fin.close();

		m_fPlayTime = jsonData["PlayTime"];

		TOOL_CINEMATIC_DESCS CamCinemaDesc;

		const auto& CamPositionData = jsonData["Camera Desc"]["Positions"];
		if (!CamPositionData.is_null())
		{
			for (size_t i = 0; i < CamPositionData["Time"].size(); ++i)
			{
				_float fTime = CamPositionData["Time"][i];
				_float3 vPosition;
				memcpy(&vPosition, CamPositionData["Position"][i].get<array<_float, 3>>().data(), sizeof(_float3));
				_bool bLerp = CamPositionData["Lerp?"][i];

				CamCinemaDesc.Positions.emplace(fTime, make_pair(vPosition, bLerp));
			}
		}

		const auto& CamLookData = jsonData["Camera Desc"]["Looks"];

		if (!CamLookData.is_null())
		{
			for (size_t i = 0; i < CamLookData["Time"].size(); ++i)
			{
				_float fTime = CamLookData["Time"][i];
				_float3 vLook;
				memcpy(&vLook, CamLookData["Look"][i].get<array<_float, 3>>().data(), sizeof(_float3));
				_bool bLerp = CamLookData["Lerp?"][i];

				CamCinemaDesc.Looks.emplace(fTime, make_pair(vLook, bLerp));
			}

		}

		const auto& CamEventData = jsonData["Camera Desc"]["Events"];

		if (!CamEventData.is_null())
		{
			for (size_t i = 0; i < CamEventData["Time"].size(); ++i)
			{
				CINE_EVENT CineEvent;

				_float fTime = CamEventData["Time"][i];
				CineEvent.eEventType = CamEventData["EventType"][i];
				CineEvent.strParam = CamEventData["Param"][i];

				CamCinemaDesc.Events.emplace(fTime, CineEvent);
			}

		}

		m_pCamera->Set_CinemaDesc(CamCinemaDesc);


		for (const auto& ActorTag : jsonData["ActorTag"])
		{
			TOOL_CINEMATIC_DESCS CinemaDesc;

			CinemaDesc.strActorTag = ActorTag;

			const auto& PositionData = jsonData[CinemaDesc.strActorTag]["Positions"];

			if (!PositionData.is_null())
			{
				for (size_t i = 0; i < PositionData["Time"].size(); ++i)
				{
					_float fTime = PositionData["Time"][i];
					_float3 vPosition;
					memcpy(&vPosition, PositionData["Position"][i].get<array<_float, 3>>().data(), sizeof(_float3));
					_bool bLerp = PositionData["Lerp?"][i];

					CinemaDesc.Positions.emplace(fTime, make_pair(vPosition, bLerp));
				}
			}

			const auto& LookData = jsonData[CinemaDesc.strActorTag]["Looks"];

			if (!LookData.is_null())
			{
				for (size_t i = 0; i < LookData["Time"].size(); ++i)
				{
					_float fTime = LookData["Time"][i];
					_float3 vLook;
					memcpy(&vLook, LookData["Look"][i].get<array<_float, 3>>().data(), sizeof(_float3));
					_bool bLerp = LookData["Lerp?"][i];

					CinemaDesc.Looks.emplace(fTime, make_pair(vLook, bLerp));
				}
			}


			const auto& ScaleData = jsonData[CinemaDesc.strActorTag]["Scales"];

			if (!ScaleData.is_null())
			{
				for (size_t i = 0; i < ScaleData["Time"].size(); ++i)
				{
					_float fTime = ScaleData["Time"][i];
					_float3 vScale;
					memcpy(&vScale, ScaleData["Scale"][i].get<array<_float, 3>>().data(), sizeof(_float3));
					_bool bLerp = ScaleData["Lerp?"][i];

					CinemaDesc.Scales.emplace(fTime, make_pair(vScale, bLerp));
				}
			}


			CCinemaObject* pCinemaObj = static_cast<CCinemaObject*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Object", L"Prototype_CinemaObject"));


			_int iModelIdx = jsonData[CinemaDesc.strActorTag]["ModelIndex"];
			if (iModelIdx >= 0)
			{
				CModel* pModel = static_cast<CModel*>(m_pGameInstance->Clone_Component(GET_CURLEVEL,
					L"Prototype_Model_" + Convert_StrToWStr(m_strModelTags[iModelIdx])));

				if (nullptr == pModel)
					return E_FAIL;

				pCinemaObj->Change_Model(pModel);
				pCinemaObj->Set_ModelIdx(iModelIdx);
			}


			const auto& EventData = jsonData[CinemaDesc.strActorTag]["Events"];

			if (!EventData.is_null())
			{
				for (size_t i = 0; i < EventData["Time"].size(); ++i)
				{
					CINE_EVENT CineEvent;

					_float fTime = EventData["Time"][i];
					CineEvent.eEventType = EventData["EventType"][i];
					CineEvent.strParam = EventData["Param"][i];

					switch (CineEvent.eEventType)
					{
					case CHANGE_ANIMATION:
					{
						CModel* pModel = pCinemaObj->Get_Model();
						CineEvent.pFunc = bind(&CModel::Change_Animation, pModel, stoi(CineEvent.strParam), 0.1f, true);
						break;
					}

					case PLAY_SOUND:
					{
						CineEvent.pFunc = bind(&CGameInstance::Play, m_pGameInstance, CineEvent.strParam, false, 1.f);
						break;
					}

					case STOP_SOUND:
					{
						CineEvent.pFunc = bind(&CGameInstance::Stop, m_pGameInstance, CineEvent.strParam);
						break;
					}

					case ACTIVE_EFFECT:
					{
						CineEvent.pFunc = bind(&CEffect_Manager_Tool::Active_Effect, EFFECTMGR, LEVEL_CINEMATOOL, CineEvent.strParam, pCinemaObj->Get_CineEffectDesc(), nullptr);
						break;
					}

					case INACITVE_EFFECT:
					{
						CineEvent.pFunc = bind(&CEffect_Manager_Tool::End_Effect, EFFECTMGR, LEVEL_CINEMATOOL, CineEvent.strParam, 0);
						break;
					}
					}

					CinemaDesc.Events.emplace(fTime, CineEvent);
				}
			}
			m_CinemaObjects.emplace_back(pCinemaObj);
			pCinemaObj->Set_CinemaDesc(CinemaDesc);
		}
		
	}

	return S_OK;
}

HRESULT CCinema_Tool::Load_MapData()
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
		return E_FAIL;
	}

	DWORD dwByte = { 0 };
	HANDLE hFile = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX(L"Can't Open File To Load");
		return E_FAIL;
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

	return S_OK;
}

HRESULT CCinema_Tool::Load_SoundData()
{
	wstring wstrFolderPath = Get_FolderPath(wstring(L"../../Resources/Sound/"));
	if (wstrFolderPath.empty())
		return S_OK;

	fs::path folderPath(wstrFolderPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_directory())
			continue;

		string ext = entry.path().extension().generic_string();

		if (ext != ".wav" && ext != ".mp3")
			continue;

		string strPath = entry.path().generic_string();

		m_pGameInstance->Create_Sound(strPath, entry.path().stem().generic_string());
	}


	return S_OK;
}

HRESULT CCinema_Tool::Load_EffectData()
{
	wstring wstrFolderPath = Get_FolderPath(wstring(L"../../Resources/Effect/SaveData/"));
	if (wstrFolderPath.empty())
		return S_OK;

	fs::path folderPath(wstrFolderPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().generic_string() != ".json")
			continue;

		wstring wstrPath = entry.path().generic_wstring();

		if (FAILED(EFFECTMGR->Effect_Loader(LEVEL_CINEMATOOL, wstrPath, m_pDevice, m_pContext)))
			return E_FAIL;
	}

	return S_OK;
}

wstring CCinema_Tool::Get_FolderPath(const wstring& wstrInitPath)
{
	_tchar szAbsolutePath[MAX_PATH] = {};
	_wfullpath(szAbsolutePath, wstrInitPath.c_str(), MAX_PATH);

	ITEMIDLIST* pidlRoot = nullptr;
	if (!SUCCEEDED(SHParseDisplayName(szAbsolutePath, NULL, &pidlRoot, 0, NULL))) {
		pidlRoot = nullptr;
	}

	BROWSEINFO browseInfo;
	ZeroMemory(&browseInfo, sizeof(BROWSEINFO));
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	browseInfo.hwndOwner = g_hWnd;
	browseInfo.pidlRoot = pidlRoot;

	LPITEMIDLIST pItemIdList = SHBrowseForFolder(&browseInfo);
	if (0 != pItemIdList)
	{
		_tchar szPath[MAX_PATH];
		if (SHGetPathFromIDList(pItemIdList, szPath))
		{
			CoTaskMemFree(pItemIdList);
			return wstring(szPath);
		}
		CoTaskMemFree(pItemIdList);
	}

	return L"";
}

void CCinema_Tool::Update_LightInfo()
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

void CCinema_Tool::Update_ShadowInfo()
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

	if (iFlag || m_bLoadMap)
		m_pGameInstance->Update_ShadowViewProj(XMLoadFloat4(&m_vShadowDir), XMLoadFloat4(&m_vShadowPos));

	m_bLoadMap = false;
}

void CCinema_Tool::Add_Water(WATER_DESCS WaterDesc, _float4 vWaterPos, _float3 vWaterScale)
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

void CCinema_Tool::Add_SelectedModels(const wstring& strModel, const wstring& strLayer)
{
	pair<wstring, wstring> NameAndLayer = { strModel, strLayer };
	vecSelectedModels.push_back(NameAndLayer);
}

_bool CCinema_Tool::NaturalSortCompare(const wstring& a, const wstring& b)
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

void CCinema_Tool::Sort_SelectedModels()
{
	sort(vecSelectedModels.begin(), vecSelectedModels.end(),
		[this](const pair<wstring, wstring>& a, const pair<wstring, wstring>& b) -> bool {
			return NaturalSortCompare(a.first, b.first);
		}
	);
}


void CCinema_Tool::Actor_Gizmo()
{
	if (m_CinemaObjects.empty() || m_iCinemaObjIdx < 0)
		return;

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4 ViewMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

#pragma region InputKey
	if (false == m_pGameInstance->GetKey(eKeyCode::RButton))
	{
		if (m_pGameInstance->GetKeyDown(eKeyCode::W))
		{
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
			m_tGizmoDesc.snap[0] = 0.5f;
		}
		if (m_pGameInstance->GetKeyDown(eKeyCode::E))
		{
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
			m_tGizmoDesc.snap[0] = To_Radian(15.f);
		}
		if (m_pGameInstance->GetKeyDown(eKeyCode::R))
		{
			m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
			m_tGizmoDesc.snap[0] = 0.5f;
		}

		if (m_pGameInstance->GetKeyDown(eKeyCode::T))
			m_tGizmoDesc.CurrentGizmoMode = m_tGizmoDesc.CurrentGizmoMode == ImGuizmo::LOCAL ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

		if (m_pGameInstance->GetKeyDown(eKeyCode::Y))
			m_tGizmoDesc.bUseSnap = !m_tGizmoDesc.bUseSnap;
	}
#pragma endregion 

	_float4x4 WorldMatrix = m_CinemaObjects[m_iCinemaObjIdx]->Get_Transform()->Get_WorldFloat4x4();
	if (ImGuizmo::Manipulate(*ViewMatrix.m, *ProjMatrix.m
		, m_tGizmoDesc.CurrentGizmoOperation
		, m_tGizmoDesc.CurrentGizmoMode
		, *WorldMatrix.m, NULL, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
		, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
	{
		m_CinemaObjects[m_iCinemaObjIdx]->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
		Reset_Transform(XMLoadFloat4x4(&WorldMatrix));
	}

}

void CCinema_Tool::Actor_AddCinemaKey()
{
	ImGui::SeparatorText("Cinema Key");

	//ImGui::Combo("##EventLists", &m_iSelEventName, szEventNames, (_int)m_strEventNames.size());

	ImGui::Combo("##KeyType", (_int*)&m_eKeyToAdd, m_szKeyTypes, KEY_END);

	if (ImGui::Button("Add Key"))
	{
		CGameObject* pObj = m_bEditCamera == true ? m_pCamera : static_cast<CGameObject*>(m_CinemaObjects[m_iCinemaObjIdx]);
		TOOL_CINEMATIC_DESCS& Desc = m_bEditCamera == true ? m_pCamera->Get_CinemaDesc() : m_CinemaObjects[m_iCinemaObjIdx]->Get_CinemaDesc();

		switch (m_eKeyToAdd)
		{
		case SCALE:
		{
			_float3 fScale = pObj->Get_Transform()->Get_Scale();

			Desc.Scales.erase(m_fPlayTimeAcc); // ���� ������ Ű ������ �ִٸ� �׳� �����ش�

			Desc.Scales.insert({ m_fPlayTimeAcc, { fScale, true } });

			break;
		}
		case LOOK:
		{
			_float3 vLook;
			XMStoreFloat3(&vLook, pObj->Get_Transform()->Get_Look());

			Desc.Looks.erase(m_fPlayTimeAcc); // ���� ������ Ű ������ �ִٸ� �׳� �����ش�

			Desc.Looks.insert({ m_fPlayTimeAcc, { vLook, true } });

			break;
		}
		case POSITION:
		{
			_float3 vPosition;
			XMStoreFloat3(&vPosition, pObj->Get_Transform()->Get_Position());

			Desc.Positions.erase(m_fPlayTimeAcc); // ���� ������ Ű ������ �ִٸ� �׳� �����ش�

			Desc.Positions.insert({ m_fPlayTimeAcc, { vPosition, true } });

			break;
		}
		case EVENT:
		{
			
			CINE_EVENT Event;

			Desc.Events.erase(m_fPlayTimeAcc); // ���� ������ Ű ������ �ִٸ� �׳� �����ش�

			Desc.Events.insert({ m_fPlayTimeAcc, Event});
			break;
		}
		}
	}

}


void CCinema_Tool::Anim_Window()
{
	ImGui::Begin("Anim Window", (bool*)0);
	Actor_AnimListBox();
	Anim_Buttons();
	ImGui::End();
}


void CCinema_Tool::Create_NewActor()
{
	if (0 == strlen(m_szActorTag))
		return;

	CCinemaObject* pObject = static_cast<CCinemaObject*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Obj", L"Prototype_CinemaObject"));
	if (pObject == nullptr)
		return;

	TOOL_CINEMATIC_DESCS CinemaDescs;
	CinemaDescs.strActorTag = string(m_szActorTag);

	pObject->Set_CinemaDesc(CinemaDescs);

	m_CinemaObjects.emplace_back(pObject);
}



void CCinema_Tool::Reset_Transform(_fmatrix WorldMatrix)
{
	_vector vScale, vPosition, vQuaternion;
	XMMatrixDecompose(&vScale, &vQuaternion, &vPosition, WorldMatrix);
	

	XMStoreFloat3(&m_vScale, vScale);
	XMStoreFloat3(&m_vPosition, vPosition);
	XMStoreFloat3(&m_vLook, XMVector3Normalize(WorldMatrix.r[2]));

}

CCinema_Tool* CCinema_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinema_Tool* pInstance = new CCinema_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CCinema_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCinema_Tool::Free()
{
	for (auto& Objects : m_vecObjects)
		Safe_Release(Objects);
	m_vecObjects.clear();

	Safe_Release(m_pModelObject);
	Safe_Release(m_pWater);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}

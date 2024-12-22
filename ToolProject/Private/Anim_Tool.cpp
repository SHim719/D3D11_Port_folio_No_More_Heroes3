#include "Anim_Tool.h"

#include "Animation.h"
#include "Bone.h"

#include "ToolAnimObj.h"
#include "ToolColliderObj.h"
#include "GameInstance.h"
#include "Effect_Tool.h"



CAnim_Tool::CAnim_Tool()
	: CTool_Super()
{
}

HRESULT CAnim_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_strModelLists.reserve(30);
	m_strModelLists.emplace_back("Travis");
	m_strModelLists.emplace_back("Travis_Space");
	m_strModelLists.emplace_back("Travis_World");
	m_strModelLists.emplace_back("Travis_Armor");
	m_strModelLists.emplace_back("Mbone");
	m_strModelLists.emplace_back("MrBlackhole_ArmL");
	m_strModelLists.emplace_back("MrBlackhole_ArmR");
	m_strModelLists.emplace_back("MrBlackhole");
	m_strModelLists.emplace_back("MrBlackhole_Space");
	m_strModelLists.emplace_back("Treatment");
	m_strModelLists.emplace_back("Fullface");
	m_strModelLists.emplace_back("Tripleput");
	m_strModelLists.emplace_back("SonicJuice");
	m_strModelLists.emplace_back("Travis_Bike");
	m_strModelLists.emplace_back("SmashBros_Damon");
	m_strModelLists.emplace_back("Leopardon");
	m_strModelLists.emplace_back("DestroymanTF");

	m_pEffectTool = CEffect_Tool::Create();
	m_pEffectTool->Set_CurLevel(LEVEL_ANIMTOOL);
	return S_OK;
}

void CAnim_Tool::Start_Tool()
{
	CCamera::CAMERADESC camDesc{};
	camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	camDesc.fNear = 0.1f;
	camDesc.fFar = 1000.f;
	camDesc.fFovy = 70.f;
	camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

	m_pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavNoCaptureKeyboard;     // Enable Keyboard Controls

	ImGui::StyleColorsClassic();

	io.Fonts->AddFontFromFileTTF("../../Resources/Font/Quicksand-Medium.ttf", 12.f, nullptr);

	m_pGameInstance->Change_MainCamera(m_pCamera);
}

void CAnim_Tool::Tick(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	Main_Window();
	KeyFrame_Window();
	Check_KeyFrameSound();
	m_pEffectTool->EffectTick(fTimeDelta);
	m_pEffectTool->EffectLateTick(fTimeDelta);
}

HRESULT CAnim_Tool::Load_KeyFrameNames(_int iSelModelIdx)
{
	string strFullPath;

	switch (iSelModelIdx)
	{
	case CToolAnimObj::TRAVIS:
		strFullPath = "../../Resources/KeyFrame/Travis/KeyFrames_Travis.txt";
		break;
	case CToolAnimObj::TRAVIS_SPACE:
		strFullPath = "../../Resources/KeyFrame/Travis_Space/KeyFrames_Travis_Space.txt";
		break;
	case CToolAnimObj::TRAVIS_WORLD:
		strFullPath = "../../Resources/KeyFrame/Travis_World/KeyFrames_Travis_World.txt";
		break;
	case CToolAnimObj::TRAVIS_ARMOR:
		strFullPath = "../../Resources/KeyFrame/Travis_Armor/KeyFrames_Travis_Armor.txt";
		break;
	case CToolAnimObj::MBONE:
		strFullPath = "../../Resources/KeyFrame/Mbone/Mbone_KeyFrameFunc.txt";
		break;
	case CToolAnimObj::MRBLACKHOLE_ARML:
		strFullPath = "../../Resources/KeyFrame/MrBlackhole/KeyFrame_MrBlackholeArmL.txt";
		break;
	case CToolAnimObj::MRBLACKHOLE_ARMR:
		strFullPath = "../../Resources/KeyFrame/MrBlackhole/KeyFrame_MrBlackholeArmR.txt";
		break;
	case CToolAnimObj::MRBLACKHOLE:
		strFullPath = "../../Resources/KeyFrame/MrBlackhole/KeyFrame_MrBlackhole.txt";
		break;
	case CToolAnimObj::MRBLACKHOLE_SPACE:
		strFullPath = "../../Resources/KeyFrame/MrBlackhole/KeyFrame_MrBlackhole_Space.txt";
		break;
	case CToolAnimObj::TREATMENT:
		strFullPath = "../../Resources/KeyFrame/Treatment/KeyFrameFunc_Treatment.txt";
		break;
	case CToolAnimObj::FULLFACE:
		strFullPath = "../../Resources/KeyFrame/Fullface/Fullface_KeyFrameFunc.txt";
		break;
	case CToolAnimObj::TRIPLEPUT:
		strFullPath = "../../Resources/KeyFrame/Tripleput/Tripleput_KeyFrameFunc.txt";
		break;
	case CToolAnimObj::SONICJUICE:
		strFullPath = "../../Resources/KeyFrame/SonicJuice/SonicJuice_KeyFrameFunc.txt";
		break;
	case CToolAnimObj::DAMON:
		strFullPath = "../../Resources/KeyFrame/SmashBros_Damon/KeyFrames_SmashBros_Damon.txt";
		break;
	case CToolAnimObj::LEOPARDON:
		strFullPath = "../../Resources/KeyFrame/Leopardon/Leopardon_KeyFrameFunc.txt";
		break;
	case CToolAnimObj::DESTROYMANTF:
		strFullPath = "../../Resources/KeyFrame/DestroymanTF/DestroymanTF_KeyFrameFunc.txt";
		break;
	}

	ifstream fin;
	fin.open(strFullPath);

	if (!fin.is_open())
		return S_OK;

	m_strEventNames.clear();
	string strKeyFrame = "";
	while (getline(fin, strKeyFrame))
	{
		m_strEventNames.emplace_back(strKeyFrame);
	}

	fin.close();

	return S_OK;
}

HRESULT CAnim_Tool::Load_KeyFrames()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	if (FAILED(Load_KeyFrameData()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnim_Tool::Load_KeyFrameData()
{
	_tchar szAbsolutePath[MAX_PATH] = {};
	_wfullpath(szAbsolutePath, L"../../Resources/KeyFrame/", MAX_PATH);

	wstring wstrFolderPath = Get_FolderPath(szAbsolutePath);

	if (L"" == wstrFolderPath)
		return E_FAIL;

	fs::path folderPath(wstrFolderPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().generic_string() != ".json")
			continue;

		wstring wstrPath = entry.path().generic_wstring();

		ifstream fin(wstrPath);

		if (!fin.is_open())
			return E_FAIL;

		json jsonData;
		fin >> jsonData;
		fin.close();

		string strAnimationName = jsonData["Animation Name"];

		_int iKeyFrame = 0;

		for (const auto& KeyFrame : jsonData["KeyFrames"])
		{
			CKeyFrameEvent::KEYFRAME_DESC KeyFrameDesc{};

			iKeyFrame = KeyFrame["KeyFrame_Index"];
			KeyFrameDesc.eKeyFrameType = KeyFrame["KeyFrame_Type"];
			KeyFrameDesc.strEventName = KeyFrame["KeyFrame_Name"];
			KeyFrameDesc.fVolume = KeyFrame["Volume"];

			size_t iAnimIndex = 0;
			for (; iAnimIndex < m_strAnimations.size(); ++iAnimIndex)
			{
				if (m_strAnimations[iAnimIndex] == strAnimationName)
					break;
			}

			if (m_strAnimations.size() == iAnimIndex)
				return E_FAIL;

			m_KeyFrameEvents[iAnimIndex].emplace_back(iKeyFrame, KeyFrameDesc);
		}

	}

	return S_OK;
}

HRESULT CAnim_Tool::Load_OldKeyFrameData()
{
	_tchar szAbsolutePath[MAX_PATH] = {};
	_wfullpath(szAbsolutePath, L"../../Resources/KeyFrame/", MAX_PATH);

	wstring wstrFolderPath = Get_FolderPath(szAbsolutePath);

	if (L"" == wstrFolderPath)
		return E_FAIL;

	fs::path folderPath(wstrFolderPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().generic_string() != ".dat")
			continue;

		wstring wstrPath = entry.path().generic_wstring();

		ifstream fin(wstrPath, ios::binary);

		if (!fin.is_open())
			return E_FAIL;

		_int iAnimNameLength = 0;
		_char szAnimName[MAX_PATH] = "";
		_int iKeyFrame = 0;
		_int iEventLength = 0;
		_char szEvent[MAX_PATH] = "";

		fin.read((_char*)&iAnimNameLength, sizeof(_int));
		fin.read(szAnimName, iAnimNameLength);

		while (true)
		{
			CKeyFrameEvent::KEYFRAME_DESC KeyFrameDesc{};

			memset(szEvent, 0, MAX_PATH);
			fin.read((_char*)&iKeyFrame, sizeof(_int));
			fin.read((_char*)&KeyFrameDesc.eKeyFrameType, sizeof(CKeyFrameEvent::KEYFRAMETYPE));
			fin.read((_char*)&iEventLength, sizeof(_int));
			fin.read(szEvent, iEventLength);
			fin.read((_char*)&KeyFrameDesc.fVolume, sizeof(_float));
			if (fin.eof())
				break;

			size_t iAnimIndex = 0;
			for (; iAnimIndex < m_strAnimations.size(); ++iAnimIndex)
			{
				if (m_strAnimations[iAnimIndex] == szAnimName)
					break;
			}

			if (m_strAnimations.size() == iAnimIndex)
				return E_FAIL;

			KeyFrameDesc.strEventName = string(szEvent);

			m_KeyFrameEvents[iAnimIndex].emplace_back(iKeyFrame, KeyFrameDesc);
		}

		//while (true)
		//{
		//   memset(szEvent, 0, MAX_PATH);
		//   fin.read((_char*)&iKeyFrame, sizeof(_int));
		//   fin.read((_char*)&iEventLength, sizeof(_int));
		//   fin.read(szEvent, iEventLength);
		//   if (fin.eof())
		//      break;
		//
		//   size_t iAnimIndex = 0;
		//   for (; iAnimIndex < m_strAnimations.size(); ++iAnimIndex)
		//   {
		//      if (m_strAnimations[iAnimIndex] == szAnimName)
		//         break;
		//   }
		//
		//   if (m_strAnimations.size() == iAnimIndex)
		//      return E_FAIL;
		//
		//   CKeyFrameEvent::KEYFRAME_DESC KeyFrameDesc{};
		//   KeyFrameDesc.eKeyFrameType = CKeyFrameEvent::NORMAL;
		//   KeyFrameDesc.strEventName = string(szEvent);
		//   m_KeyFrameEvents[iAnimIndex].emplace_back(iKeyFrame, KeyFrameDesc);
		//}
	}

	return S_OK;
}

HRESULT CAnim_Tool::Save_KeyFrameData()
{
	_tchar szAbsolutePath[MAX_PATH] = {};
	_wfullpath(szAbsolutePath, L"../../Resources/KeyFrame/", MAX_PATH);
	wstring wstrFolderPath = Get_FolderPath(szAbsolutePath);

	for (size_t i = 0; i < m_strAnimations.size(); ++i)
	{
		if (m_KeyFrameEvents[i].empty())
			continue;

		// Temporary code to remove deduplication.
		//vector<pair<_int, CKeyFrameEvent::KEYFRAME_DESC>> Temp = m_KeyFrameEvents[i];
		//sort(Temp.begin(), Temp.end(), [&](pair<_int, CKeyFrameEvent::KEYFRAME_DESC> Src, pair<_int, CKeyFrameEvent::KEYFRAME_DESC> Dst)->_bool {
		//	if (Src.first < Dst.first)
		//		return true;
		//	return false;
		//	});
		//vector<pair<_int, CKeyFrameEvent::KEYFRAME_DESC>> Temp2;

		//for (auto& iter_temp : Temp) {
		//	if (Temp2.empty()) {
		//		Temp2.push_back(iter_temp);
		//	}
		//	else {
		//		_bool bResult = true;

		//		for (auto& iter_origin : Temp2) {
		//			if (iter_temp.first == iter_origin.first &&
		//				iter_temp.second.strEventName == iter_origin.second.strEventName &&
		//				iter_temp.second.eKeyFrameType == iter_origin.second.eKeyFrameType /* &&
		//				iter_temp.second.fVolume == iter_origin.second.fVolume*/) {
		//				bResult = false;
		//				break;
		//			}
		//		}

		//		if (bResult)
		//			Temp2.push_back(iter_temp);
		//	}
		//}

		wstring wstrPath = wstrFolderPath + L"\\" + Convert_StrToWStr(m_strAnimations[i]) + L".json";

		ofstream fout(wstrPath);

		if (!fout.is_open())
			return E_FAIL;

		json jsonData;

		//_int iAnimNameLength = (_int)m_strAnimations[i].length();
		_int iEventLength = 0;

		//fout.write((_char*)&iAnimNameLength, sizeof(_int));
		//fout.write(m_strAnimations[i].c_str(), iAnimNameLength);

		jsonData["Animation Name"] = m_strAnimations[i];

		size_t iIndex = 0;
		//for (auto& Pair : Temp2)
		for (auto& Pair : m_KeyFrameEvents[i])
		{
			iEventLength = (_int)Pair.second.strEventName.length();
			//fout.write((_char*)&Pair.first, sizeof(_int));
			//fout.write((_char*)&Pair.second.eKeyFrameType, sizeof(CKeyFrameEvent::KEYFRAMETYPE));
			//fout.write((_char*)&iEventLength, sizeof(_int));
			//fout.write(Pair.second.strEventName.c_str(), iEventLength);
			//fout.write((_char*)&Pair.second.fVolume, sizeof(_float));

			jsonData["KeyFrames"].push_back({
				{ "KeyFrame_Index", Pair.first },
				{ "KeyFrame_Type", Pair.second.eKeyFrameType},
				{ "KeyFrame_Name", Pair.second.strEventName },
				{ "Volume", Pair.second.fVolume }
				});
			iIndex++;
		}
		fout << jsonData.dump(4);
		fout.close();

	}



	return S_OK;
}

HRESULT CAnim_Tool::Load_Sounds()
{
	_tchar szAbsolutePath[MAX_PATH] = {};
	_wfullpath(szAbsolutePath, L"../../Resources/Sound/", MAX_PATH);

	wstring wstrFolderPath = Get_FolderPath(szAbsolutePath);
	if (L"" == wstrFolderPath)
		return E_FAIL;

	fs::path folderPath(wstrFolderPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_directory())
			continue;

		fs::path fileName = entry.path().filename();
		fs::path ext = fileName.extension();
		fs::path fileTitle = fileName.stem();

		if (ext.generic_string() != ".wav" && ext.generic_string() != ".mp3")
			continue;

		m_pGameInstance->Create_Sound(entry.path().generic_string(), fileTitle.generic_string());

		m_strSounds.emplace_back(fileTitle.generic_string());
	}
	return S_OK;
}

wstring CAnim_Tool::Get_FolderPath(const _tchar* szInitialPath)
{
	ITEMIDLIST* pidlRoot = nullptr;
	if (!SUCCEEDED(SHParseDisplayName(szInitialPath, NULL, &pidlRoot, 0, NULL))) {
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
		_tchar szPath[1000];
		if (SHGetPathFromIDList(pItemIdList, szPath))
		{
			CoTaskMemFree(pItemIdList);
			return wstring(szPath);
		}
		CoTaskMemFree(pItemIdList);
	}

	return L"";
}

void CAnim_Tool::Main_Window()
{
	ImGui::Begin("Main Window", (bool*)0, ImGuiWindowFlags_MenuBar);
	Menu_Bar();
	Tab_Bar();
	ImGui::End();
}

void CAnim_Tool::Menu_Bar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("KeyFrame"))
		{
			if (ImGui::MenuItem("Load KeyFrame"))
			{
				Load_KeyFrames();
			}

			if (ImGui::MenuItem("Save KeyFrame"))
			{
				Save_KeyFrameData();
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Sound"))
		{
			if (ImGui::MenuItem("Load Sound"))
			{
				Load_Sounds();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

}

void CAnim_Tool::Tab_Bar()
{
	if (ImGui::BeginTabBar("Models"))
	{
		if (ImGui::BeginTabItem("Anim"))
		{
			Model_ListBox();
			Model_Button();

			Anim_ListBox();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Collider"))
		{
			Bone_ListBox();
			Collider_ListBox();
			ColliderType_CheckBox();
			Collider_Buttons();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Effect"))
		{
			m_pEffectTool->Tick(m_fTimeDelta);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	m_pEffectTool->LateTick(m_fTimeDelta);
}

void CAnim_Tool::Model_ListBox()
{
	ImGui::SeparatorText("Model");
	const char** szModels = new const char* [m_strModelLists.size()];

	for (_int i = 0; i < (_int)m_strModelLists.size(); ++i)
		szModels[i] = m_strModelLists[i].c_str();

	ImGui::PushItemWidth(210);
	ImGui::ListBox("##Model_ListBox", &m_iSelModelIdx, szModels, (_int)m_strModelLists.size(), 5);
	ImGui::PopItemWidth();

	Safe_Delete_Array(szModels);
}

void CAnim_Tool::Model_Button()
{
	if (ImGui::Button("Load Model"))
	{
		if (m_pAnimObj)
		{
			m_pAnimObj->Set_Destroy(true);
			m_iSelAnimIdx = 0;
		}

		m_pAnimObj = static_cast<CToolAnimObj*>(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"ToolObject", L"Prototype_ToolAnimObj", &m_iSelModelIdx));
		m_pModel = m_pAnimObj->Get_Model();
		if (nullptr != m_pEffectTool)
			m_pEffectTool->Set_AnimObject(m_pAnimObj);

		Load_KeyFrameNames(m_iSelModelIdx);

		m_strAnimations.clear();
		m_strAnimations.shrink_to_fit();
		auto Anims = m_pModel->Get_Animations();
		m_strAnimations.reserve(Anims.size());
		for (auto pAnim : Anims)
			m_strAnimations.emplace_back(pAnim->Get_AnimName());

		m_KeyFrameEvents.clear();
		m_KeyFrameEvents.shrink_to_fit();
		m_KeyFrameEvents.resize(Anims.size());

		auto& Bones = m_pModel->Get_Bones();

		Safe_Delete_Array(m_szBoneNames);

		m_szBoneNames = new const _char * [Bones.size()];
		for (size_t i = 0; i < Bones.size(); ++i)
			m_szBoneNames[i] = Bones[i]->Get_Name();

		m_iNumBones = (_int)Bones.size();
	}
}

void CAnim_Tool::Anim_ListBox()
{
	ImGui::SeparatorText("Animation");

	m_fAnimListBoxStartCursorY = ImGui::GetCursorPosY();

	const char** szAnimations = new const char* [m_strAnimations.size()];

	for (_int i = 0; i < (_int)m_strAnimations.size(); ++i)
		szAnimations[i] = m_strAnimations[i].c_str();
	ImGui::PushItemWidth(400);
	if (ImGui::ListBox("##Anim_ListBox", &m_iSelAnimIdx, szAnimations, (_int)m_strAnimations.size(), 20))
	{
		m_pModel->Change_Animation(m_iSelAnimIdx);
		m_iSelKeyFrameIdx = 0;
	}

	ImGui::PopItemWidth();
	Safe_Delete_Array(szAnimations);
}

void CAnim_Tool::Anim_Buttons()
{
	if (ImGui::Button("Play") && m_pModel)
		m_pModel->Set_AnimPlay();

	ImGui::SameLine();
	if (ImGui::Button("Stop") && m_pModel)
		m_pModel->Set_AnimPause();
}

void CAnim_Tool::KeyFrame_Window()
{
	if (nullptr == m_pModel)
		return;

	auto& Animations = m_pModel->Get_Animations();

	_int iNowKeyFrame = (_int)Animations[m_iSelAnimIdx]->Get_NowKeyFrame();
	_int iNumKeyFrames = (_int)Animations[m_iSelAnimIdx]->Get_NumKeyFrames();

	ImGui::Begin("KeyFrame", (bool*)0);

	ImGui::Text("Now Animaton: %s", m_strAnimations[m_iSelAnimIdx].c_str());
	ImGui::Text("Now KeyFrame: %d", iNowKeyFrame);

	if (ImGui::SliderInt("##KeyFrameController", &iNowKeyFrame, 0, iNumKeyFrames))
		Animations[m_iSelAnimIdx]->Set_CurrentKeyFrames((_uint)iNowKeyFrame);

	if (ImGui::Button("U"))
		Animations[m_iSelAnimIdx]->Set_CurrentKeyFrames((_uint)(++iNowKeyFrame));

	ImGui::SameLine();

	if (ImGui::Button("D"))
		Animations[m_iSelAnimIdx]->Set_CurrentKeyFrames((_uint)(iNowKeyFrame = iNowKeyFrame == 0 ? 0 : --iNowKeyFrame));

	Anim_Buttons();

	if (nullptr != m_pEffectTool)
	{
		if (ImGui::Button("Bind Single Effect"))
			m_pEffectTool->Bind_KeyFrame_Effect(true, (_uint)iNowKeyFrame);

		ImGui::SameLine();
		if (ImGui::Button("Bind Group Effect"))
			m_pEffectTool->Bind_KeyFrame_Effect(false, (_uint)iNowKeyFrame);

		ImGui::Text("Current Binded Frame : %d", m_pEffectTool->Get_Current_KeyFrame());

		m_pEffectTool->Start_KeyFrame_Effect((_uint)iNowKeyFrame);
	}
	if (ImGui::BeginTabBar("Event"))
	{
		if (ImGui::BeginTabItem("Event"))
		{
			ImGui::SeparatorText("Event");
			KeyFrameEvent_ComboBox();
			KeyFrameEvent_ListBox();
			KeyFrameEvent_Button();
			ImGui::EndTabItem();

			ImGui::SameLine();
			
			if(ImGui::Button("Update_Event"))
			{
				Load_KeyFrameNames(m_iSelModelIdx);
			}
		}

		if (ImGui::BeginTabItem("Sound"))
		{
			Sound_ListBox();
			KeyFrameEvent_ListBox();
			Sound_Buttons();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	ImGui::End();
}

void CAnim_Tool::KeyFrameEvent_ComboBox()
{
	const char** szEventNames = new const char* [m_strEventNames.size()];

	for (_int i = 0; i < (_int)m_strEventNames.size(); ++i)
		szEventNames[i] = m_strEventNames[i].c_str();

	ImGui::PushItemWidth(180);
	ImGui::Combo("##EventLists", &m_iSelEventName, szEventNames, (_int)m_strEventNames.size());
	ImGui::PopItemWidth();

	Safe_Delete_Array(szEventNames);
}

void CAnim_Tool::KeyFrameEvent_ListBox()
{
	ImGui::SeparatorText("Now KeyFrames");

	string* strKeyFrames = new string[m_KeyFrameEvents[m_iSelAnimIdx].size()];
	for (size_t i = 0; i < m_KeyFrameEvents[m_iSelAnimIdx].size(); ++i)
		strKeyFrames[i] = "<" + to_string(m_KeyFrameEvents[m_iSelAnimIdx][i].first) + " , " + m_KeyFrameEvents[m_iSelAnimIdx][i].second.strEventName + ">";

	const _char** szKeyFrames = new const _char * [m_KeyFrameEvents[m_iSelAnimIdx].size()];
	for (size_t i = 0; i < m_KeyFrameEvents[m_iSelAnimIdx].size(); ++i)
		szKeyFrames[i] = strKeyFrames[i].c_str();

	ImGui::PushItemWidth(300);
	ImGui::ListBox("##KeyFrame_ListBox", &m_iSelKeyFrameIdx, szKeyFrames, (_int)m_KeyFrameEvents[m_iSelAnimIdx].size(), 10);
	ImGui::PopItemWidth();


	Safe_Delete_Array(strKeyFrames);
	Safe_Delete_Array(szKeyFrames);
}

void CAnim_Tool::KeyFrameEvent_Button()
{
	if (ImGui::Button("Add"))
	{
		auto Anims = m_pModel->Get_Animations();
		_int iNowKeyFrame = (_int)Anims[m_iSelAnimIdx]->Get_NowKeyFrame();
		CKeyFrameEvent::KEYFRAME_DESC KeyFrameDesc{};
		KeyFrameDesc.eKeyFrameType = CKeyFrameEvent::NORMAL;
		KeyFrameDesc.strEventName = m_strEventNames[m_iSelEventName];

		m_KeyFrameEvents[m_iSelAnimIdx].emplace_back(iNowKeyFrame, KeyFrameDesc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		m_KeyFrameEvents[m_iSelAnimIdx].erase(m_KeyFrameEvents[m_iSelAnimIdx].begin() + m_iSelKeyFrameIdx);
		m_iSelKeyFrameIdx = 0;
	}
}

void CAnim_Tool::Sound_ListBox()
{
	if (0 == m_strSounds.size())
		return;

	ImGui::SeparatorText("Sounds");

	const char** szSoundNames = new const char* [m_strSounds.size()];

	for (_int i = 0; i < (_int)m_strSounds.size(); ++i)
		szSoundNames[i] = m_strSounds[i].c_str();

	ImGui::PushItemWidth(250);
	ImGui::ListBox("##Sound_ListBox", &m_iSelSoundIdx, szSoundNames, (_int)m_strSounds.size(), 10);
	ImGui::PopItemWidth();

	Safe_Delete_Array(szSoundNames);

	if (m_KeyFrameEvents[m_iSelAnimIdx].size())
	{
		ImGui::SameLine();
		ImGui::Text("Volume: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(50);
		ImGui::InputFloat("##Volume", &m_KeyFrameEvents[m_iSelAnimIdx][m_iSelKeyFrameIdx].second.fVolume);
		ImGui::PopItemWidth();
	}

}

void CAnim_Tool::Sound_Buttons()
{
	if (ImGui::Button("Add"))
	{
		auto Anims = m_pModel->Get_Animations();
		_int iNowKeyFrame = (_int)Anims[m_iSelAnimIdx]->Get_NowKeyFrame();
		CKeyFrameEvent::KEYFRAME_DESC KeyFrameDesc{};
		KeyFrameDesc.eKeyFrameType = CKeyFrameEvent::PLAY_SOUND;
		KeyFrameDesc.strEventName = m_strSounds[m_iSelSoundIdx];

		m_KeyFrameEvents[m_iSelAnimIdx].emplace_back(iNowKeyFrame, KeyFrameDesc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		m_KeyFrameEvents[m_iSelAnimIdx].erase(m_KeyFrameEvents[m_iSelAnimIdx].begin() + m_iSelKeyFrameIdx);
		m_iSelKeyFrameIdx = 0;
	}
}

void CAnim_Tool::Check_KeyFrameSound()
{
	if (nullptr == m_pModel || false == m_pModel->Is_Playing())
		return;

	m_iCurKeyFrame = m_pModel->Get_NowAnimKeyFrame();
	if (m_iCurKeyFrame != m_iPrevKeyFrame)
	{
		m_iPrevKeyFrame = m_iCurKeyFrame;
		for (auto& Pair : m_KeyFrameEvents[m_iSelAnimIdx])
		{
			if (Pair.first == m_iCurKeyFrame && CKeyFrameEvent::KEYFRAMETYPE::PLAY_SOUND == Pair.second.eKeyFrameType)
			{
				PLAY_SOUND(Pair.second.strEventName, false, Pair.second.fVolume);
			}
		}
	}
}


void CAnim_Tool::Bone_ListBox()
{
	if (nullptr == m_pAnimObj)
		return;

	ImGui::SeparatorText("Bones");
	ImGui::PushItemWidth(150);
	if (ImGui::ListBox("##Bone_ListBox", &m_iSelBoneIdx, m_szBoneNames, m_iNumBones, 20))
	{
		if (m_Colliders.empty())
			return;
		m_Colliders[m_iSelColliderIdx]->Set_AttachBone(m_szBoneNames[m_iSelBoneIdx]);
	}

	if (nullptr != m_pEffectTool)
	{
		ImGui::SameLine();
		if (ImGui::Button("Select Effect Bone"))
		{
			m_pEffectTool->Set_BoneName(m_szBoneNames[m_iSelBoneIdx]);
		}

		ImGui::SameLine();
		if (ImGui::Button("Select Effect Target Bone"))
		{
			m_pEffectTool->Set_TargetBoneName(m_szBoneNames[m_iSelBoneIdx]);
		}
	}
}

void CAnim_Tool::Collider_ListBox()
{
	ImGui::SeparatorText("Collider");

	const _char** szColliders = new const _char * [m_Colliders.size()];

	for (size_t i = 0; i < m_strColliders.size(); ++i)
		szColliders[i] = m_strColliders[i].c_str();

	ImGui::PushItemWidth(150);
	ImGui::ListBox("##Collider_ListBox", &m_iSelColliderIdx, szColliders, (_int)m_Colliders.size(), 5);

	Safe_Delete_Array(szColliders);
}

void CAnim_Tool::ColliderType_CheckBox()
{
	if (ImGui::Checkbox("AABB", &m_bColliderTypes[0]))
	{
		m_bColliderTypes[1] = false;
		m_bColliderTypes[2] = false;
		m_iTypeIdx = 0;
	}

	if (ImGui::Checkbox("OBB", &m_bColliderTypes[1]))
	{
		m_bColliderTypes[0] = false;
		m_bColliderTypes[2] = false;
		m_iTypeIdx = 1;
	}

	if (ImGui::Checkbox("Sphere", &m_bColliderTypes[2]))
	{
		m_bColliderTypes[0] = false;
		m_bColliderTypes[1] = false;
		m_iTypeIdx = 2;
	}
}

void CAnim_Tool::Collider_Buttons()
{
	ImGui::InputFloat3("Center", &m_ColliderDesc.vCenter.x);
	ImGui::InputFloat3("Size", &m_ColliderDesc.vSize.x);
	ImGui::InputFloat3("Rotation", &m_ColliderDesc.vRotation.x);

	if (ImGui::Button("Add Collider"))
	{
		CCollider::COLLIDERDESC desc;
		desc.vCenter = m_ColliderDesc.vCenter;
		desc.vSize = m_ColliderDesc.vSize;
		desc.vRotation.x = To_Radian(m_ColliderDesc.vRotation.x);
		desc.vRotation.y = To_Radian(m_ColliderDesc.vRotation.y);
		desc.vRotation.z = To_Radian(m_ColliderDesc.vRotation.z);
		desc.pOwner = m_pAnimObj;
		desc.eType = (CCollider::ColliderType)m_iTypeIdx;
		desc.bActive = true;

		CToolColliderObj* pObj = static_cast<CToolColliderObj*>(m_pGameInstance->Add_Clone(LEVEL_STATIC, L"ToolObject", L"Prototype_ToolColliderObj", &desc));
		m_Colliders.push_back(pObj);

		m_strColliders.push_back("Collider");
	}

	if (ImGui::Button("Edit Collider"))
	{
		if (m_Colliders.empty())
			return;

		CCollider::COLLIDERDESC desc;
		desc.vCenter = m_ColliderDesc.vCenter;
		desc.vSize = m_ColliderDesc.vSize;
		desc.vRotation.x = To_Radian(m_ColliderDesc.vRotation.x);
		desc.vRotation.y = To_Radian(m_ColliderDesc.vRotation.y);
		desc.vRotation.z = To_Radian(m_ColliderDesc.vRotation.z);

		m_Colliders[m_iSelColliderIdx]->Remake_Collider(&desc);
	}
}

CAnim_Tool* CAnim_Tool::Create()
{
	CAnim_Tool* pInstance = new CAnim_Tool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnim_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnim_Tool::Free()
{
	__super::Free();

	Safe_Delete_Array(m_szBoneNames);

	Safe_Release(m_pEffectTool);
}

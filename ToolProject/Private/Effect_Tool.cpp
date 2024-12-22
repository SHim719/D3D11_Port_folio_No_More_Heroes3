#include "Effect_Tool.h"
#include "Effect_Mesh_Tool.h"
#include "Effect_Particle_Tool.h"
#include "Effect_Trail_Tool.h"
#include "Effect_Group_Tool.h"
#include "Anim_Tool.h"
#include "ToolAnimObj.h"
#include "Bone.h"

using namespace HR;

CEffect_Tool::CEffect_Tool()
	:CTool_Super()
{
}

HRESULT CEffect_Tool::Initialize()
{
	m_eLevel = LEVEL_EFFECTTOOL;
	return S_OK;
}

void CEffect_Tool::Start_Tool()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavNoCaptureKeyboard;     // Enable Keyboard Controls

	ImGui::StyleColorsClassic();

	io.Fonts->AddFontFromFileTTF("../../Resources/Font/Quicksand-Medium.ttf", 16.f, nullptr);
}



HRESULT CEffect_Tool::MakeTextureVector(const wstring& Path)
{
	wstring wstrTexPath(Path);

	fs::path TexturePath(wstrTexPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(TexturePath))
	{
		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));

			if (FAILED(MakeTextureVector(wstrPath.c_str())))
				return E_FAIL;
		}
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();

			if (fileName.extension().compare(".dds"))
				continue;
			wstring strTmpfullPath = fullPath.parent_path();
			size_t iPos = strTmpfullPath.find_last_of(TEXT("/"));
			strTmpfullPath = strTmpfullPath.substr(iPos + 1);


			auto Pair = m_TexObj.find(strTmpfullPath);

			if (m_TexObj.end() == Pair)
			{
				vector<CTexture*> Vec;
				Vec.emplace_back(dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(m_eLevel, fileTitle.c_str())));

				m_TexObj.emplace(strTmpfullPath, Vec);
			}
			else
				Pair->second.emplace_back(dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(m_eLevel, fileTitle.c_str())));


			auto Pair2 = m_TexName.find(strTmpfullPath);

			if (m_TexName.end() == Pair2)
			{
				vector<wstring> Vec;
				Vec.emplace_back(fileTitle);

				m_TexName.emplace(strTmpfullPath, Vec);
			}
			else
				Pair2->second.emplace_back(fileTitle);
		}
	}

	return S_OK;
}

HRESULT CEffect_Tool::MakeMeshVector(const wstring& Path)
{
	wstring wstrMeshPath(Path);

	fs::path MeshPath(wstrMeshPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(MeshPath))
	{
		if (entry.is_directory())
		{
			// / 하나가 빠짐 붙여서 넣기.
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));
			if (FAILED(MakeMeshVector(wstrPath.c_str())))
				return E_FAIL;
		}
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();
			fs::path ext = fileName.extension();
			if (fileName.extension().compare(".dat"))
				continue;

			wstring strTmpfullPath = fullPath.parent_path();
			size_t iPos = strTmpfullPath.find_last_of(TEXT("/"));
			strTmpfullPath = strTmpfullPath.substr(iPos + 1);

			auto Pair = m_ModelNames.find(strTmpfullPath);

			if (Pair == m_ModelNames.end())
			{
				vector<wstring> Vec;

				Vec.emplace_back(fileTitle);
				m_ModelNames.emplace(strTmpfullPath, Vec);
			}
			else
				Pair->second.emplace_back(fileTitle);
		}
	}
	return S_OK;
}

HRESULT CEffect_Tool::MakeSavedEffectVector(const wstring& Path)
{
	wstring wstrTexPath(Path);

	fs::path TexturePath(wstrTexPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(TexturePath))
	{
		if (entry.is_directory())
			continue;
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();

			if (fileName.extension().compare(".json"))
				continue;
			wstring strTmpfullPath = fullPath.parent_path();
			size_t iPos = strTmpfullPath.find_last_of(TEXT("/"));
			strTmpfullPath = strTmpfullPath.substr(iPos + 1);

			// 파일 이름 벡터
			m_SaveFiles.emplace_back(fileName.string());
		}
	}
	return S_OK;
}



// ***********************
// *					 *
// *		Update		 *
// *					 *
// ***********************
void CEffect_Tool::Tick(_float fTimeDelta)
{

	//ImGui::Begin("Effect Tool", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	// First Setting
	CreateEffect();

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	if (true == m_bRenderGroupRadio || false == m_MeshEffects.empty() || false == m_ParticleEffects.empty() || false == m_TrailEffects.empty())
	{
		if (ImGui::RadioButton("Single Effect", &m_iGroupID, 0))
		{
			m_iEffctIdx = 0;
			m_iGroupIdx = 0;
			Safe_Release(m_pSelectedEffect);

		}

		ImGui::SameLine();
		if (ImGui::RadioButton("Group Effect", &m_iGroupID, 1))
		{
			m_iEffctIdx = 0;
			m_iGroupIdx = 0;
			Safe_Release(m_pSelectedEffect);
		}

		if (1 == m_iGroupID)
		{
			if (ImGui::Button("Reset Group Effect") || KEY_DOWN(eKeyCode::Space))
				m_pSelectedGroup->Start_Effect(nullptr);

		}
	}


	if (0 == m_iGroupID)
	{
		switch (m_eCurrent_Particle_Type)
		{
		case TYPE_MESH:
			ShowMeshEffectList();
			break;
		case TYPE_PARTICLE:
			ShowParticleEffectList();
			break;
		case TYPE_TRAIL:
			ShowTrailEffectList();
			break;
		}


	}
	else
	{
		ShowGroupList();
	}

	if (nullptr != m_pSelectedEffect)
	{
		CEffect::EFFECT_TYPE eType = m_pSelectedEffect->Get_EffetType();
		switch (eType)
		{
		case CEffect::PARTICLE:
			UpdateParticle();
			break;

		case CEffect::MESH:
			UpdateMesh();
			break;

		case CEffect::TRAIL:
			UpdateTrail();
			break;
		}
	}


	ImGui::Separator();

	//ImGui::End();
}

void CEffect_Tool::LateTick(_float fTimeDelta)
{

}

void CEffect_Tool::CreateEffect()
{
	if (false == m_bStart)
	{
		if (ImGui::Button("Setting Default Paths"))
		{

			if (FAILED(MakeTextureVector(wszTexturePath)))
			{
				MSG_BOX(TEXT("FAIL - CEffect_Tool -> Start_Tool"));
			}

			for (auto& Pair : m_TexObj)
			{
				auto TexPair = m_Textures.find(Pair.first);

				if (m_Textures.end() == TexPair)
				{
					vector<ID3D11ShaderResourceView*> Vec;
					for (auto& iter : Pair.second)
						iter->Get_Textures(&Vec);

					m_Textures.emplace(Pair.first, Vec);
				}
				else
				{
					for (auto& iter : Pair.second)
						iter->Get_Textures(&TexPair->second);
				}
			}

			if (FAILED(MakeMeshVector(wszMeshPath)))
			{
				MSG_BOX(TEXT("FAIL - CEffect_Tool -> Start_Tool"));
			}

			if (FAILED(MakeSavedEffectVector(wszSaveFilePath)))
			{
				MSG_BOX(TEXT("FAIL - CEffect_Tool -> Start_Tool"));
			}

			m_bStart = true;
		}
	}
	else
	{
		ImGui::SeparatorText("Effect Tool");

		if (false == m_EffectGroup.empty())
			m_bRenderGroupRadio = true;

		if (ImGui::RadioButton("MESH", &m_iTypeID, 0))
		{
			Safe_Release(m_pSelectedEffect);
			m_iGroupID = 0;
			m_iEffctIdx = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("PARTICLE", &m_iTypeID, 1))
		{
			Safe_Release(m_pSelectedEffect);
			m_iGroupID = 0;
			m_iEffctIdx = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("TRAIL", &m_iTypeID, 2))
		{
			Safe_Release(m_pSelectedEffect);
			m_iGroupID = 0;
			m_iEffctIdx = 0;
		}

		switch (m_iTypeID)
		{
		case 0:
			m_eCurrent_Particle_Type = TYPE_MESH;
			break;
		case 1:
			m_eCurrent_Particle_Type = TYPE_PARTICLE;
			break;
		case 2:
			m_eCurrent_Particle_Type = TYPE_TRAIL;
			break;
		}

		if (ImGui::Button("Create Effect"))
		{
			m_iGroupID = 0;
			if (nullptr != m_pSelectedEffect)
				Safe_Release(m_pSelectedEffect);
			CEffect* pEffect = { nullptr };
			switch (m_eCurrent_Particle_Type)
			{
			case TYPE_MESH:
				pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MeshEffect")));
				break;
			case TYPE_PARTICLE:
				pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleEffect")));
				break;
			case TYPE_TRAIL:
				pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TrailEffect")));
				break;
			}

			if (nullptr == pEffect)
				MSG_BOX(TEXT("FAILED TO CREATE DEFAULT PARTICLE"));
			else
			{
				m_bChangeEffect = true;
				Chage_SelectEffect(pEffect);
				switch (m_eCurrent_Particle_Type)
				{
				case TYPE_MESH:
					m_MeshEffects.emplace_back(m_pSelectedEffect);
					break;
				case TYPE_PARTICLE:
					m_ParticleEffects.emplace_back(m_pSelectedEffect);
					break;
				case TYPE_TRAIL:
					m_TrailEffects.emplace_back(m_pSelectedEffect);
					break;
				}
			}
			m_bRenderGroupRadio = true;

			strcpy_s(m_szParticleName, MAX_PATH, "");
			strcpy_s(m_szChangeName, MAX_PATH, "");
			strcpy_s(m_szParticleGroupName, MAX_PATH, "");
			strcpy_s(m_szChangeGroupName, MAX_PATH, "");
			strcpy_s(m_szChangeBoneName, MAX_PATH, "");
		}

		if (ImGui::Button("Load Saved Effect"))
			m_bLoadWindow = true;
		ImGui::SameLine();
		if (ImGui::Button("Update_SaveFile"))
		{
			if (IDYES == MessageBox(g_hWnd, TEXT("Update?"), TEXT("Check"), MB_ICONQUESTION | MB_YESNO))
			{
				SaveFileUpdateFunc(wszUpdateSaveFileDir);
			}
			//m_eLoadType = FILEUPDATE;
		}


		// *********** LOAD *************
		if (true == m_bLoadWindow)
		{
			ImGui::Begin("Load Effect", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

			const _char** szFileNames = new const _char * [m_SaveFiles.size()];
			for (size_t i = 0; i < m_SaveFiles.size(); ++i)
				szFileNames[i] = m_SaveFiles[i].c_str();
			ImGui::PushItemWidth(300.f);
			ImGui::ListBox("##File_ListBox", &m_iLoadFileIdx, szFileNames, (_int)m_SaveFiles.size(), 10);

			Safe_Delete_Array(szFileNames);

			if (ImGui::Button("Load File"))
			{
				wstring wstrFileName = wszSaveFilePath + Convert_StrToWStr(m_SaveFiles[m_iLoadFileIdx]);
				LoadEffect(wstrFileName);

				m_bLoadWindow = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Update File List"))
			{
				m_SaveFiles.clear();
				if (FAILED(MakeSavedEffectVector(wszSaveFilePath)))
				{
					MSG_BOX(TEXT("FAIL - CEffect_Tool -> Start_Tool"));
				}
			}


			if (KEY_DOWN(eKeyCode::ESC))
				m_bLoadWindow = false;

			ImGui::End();
		}
	}
}

void CEffect_Tool::UpdateParticle()
{
	if (CEffect::PARTICLE != m_pSelectedEffect->Get_EffetType())
		return;

	// Num Instance
	ImGui::PushItemWidth(150.f);
	_int iStep = 1;
	ImGui::InputScalar("Instance Nunber", ImGuiDataType_S32, &m_tSelected_Particle_Desc.iNumInstance, &iStep, NULL, "%d");
	ImGui::PopItemWidth();


	EffectTagSetting();

	if (ImGui::CollapsingHeader("Time"))
	{
		// Total Play Time
		Set_Total_Time();

		// Delay
		Set_Start_Delay();

		Set_Particle_Stop();
	}

	if (ImGui::CollapsingHeader("Loop, Shader##Particle"))
	{

		// Loop
		ImGui::Checkbox("Effect Loop", &m_tSelected_Common_Desc.bEffectLoop);
		ImGui::SameLine();
		ImGui::Checkbox("Particle Loop", &m_tSelected_Particle_Desc.bParticleLoop);
		ImGui::SameLine();

		// Pass
		ImGui::PushItemWidth(90.f);
		ImGui::InputInt("Pass Index", &m_tSelected_Common_Desc.iPassIdx);

		// Mode
		switch (m_tSelected_Particle_Desc.eParticleMode)
		{
		case (_uint)CEffect_Paticle_Tool::BILLBOARD:
			m_iModeID = 0;
			break;

		case (_uint)CEffect_Paticle_Tool::STRETCHED_BILLBOARD:
			m_iModeID = 1;
			break;

		case (_uint)CEffect_Paticle_Tool::NONE:
			m_iModeID = 2;
			break;
		}

		ImGui::RadioButton("BILLBOARD", &m_iModeID, 0);
		ImGui::SameLine();
		ImGui::RadioButton("STRETCHED_BILLBOARD", &m_iModeID, 1);
		ImGui::SameLine();
		ImGui::RadioButton("NONE", &m_iModeID, 2);

		switch (m_iModeID)
		{
		case 0:
			m_tSelected_Particle_Desc.eParticleMode = (_uint)CEffect_Paticle_Tool::BILLBOARD;
			break;

		case 1:
			m_tSelected_Particle_Desc.eParticleMode = (_uint)CEffect_Paticle_Tool::STRETCHED_BILLBOARD;
			break;

		case 2:
			m_tSelected_Particle_Desc.eParticleMode = (_uint)CEffect_Paticle_Tool::NONE;
			break;
		}



		if (0 == m_tSelected_Particle_Desc.eParticleMode)
		{
			ImGui::Checkbox("Billboard Without Up", &m_tSelected_Particle_Desc.bBillboardWithoutUp);
		}
		else
		{
			_float fFactor = { 0.01f };
			ImGui::InputScalar("Stretch Factor", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fStretchFactor, &fFactor, NULL, "%.4f");

			m_iStretchType = true == m_tSelected_Particle_Desc.bXStretch ? 0 : 1;
			ImGui::RadioButton("X Stretch", &m_iStretchType, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Y Stretch", &m_iStretchType, 1);

			if (0 == m_iStretchType)
			{
				m_tSelected_Particle_Desc.bXStretch = true;
				m_tSelected_Particle_Desc.bYStretch = false;
			}
			else
			{
				m_tSelected_Particle_Desc.bXStretch = false;
				m_tSelected_Particle_Desc.bYStretch = true;
			}

		}

		// RenderGroup
		Set_RenderGroup();
	}



	//Start Position
	Set_Start_Position();

	// Life
	Set_LifeTime();

	// Speed
	Set_SpeedInfo();

	// Size
	if (ImGui::CollapsingHeader("SIZE"))
	{
		_float fStep = 0.01f;
		ImGui::PushItemWidth(80.f);

		ImGui::Checkbox("Set Square", &m_bisSquare); ImGui::SameLine();
		ImGui::Checkbox("Enable Lerp Size##Size", &m_tSelected_Common_Desc.bLerpSize);

		if (true == m_bisSquare)
		{
			m_tSelected_Particle_Desc.bisSquare = true;
			if (ImGui::InputScalar("Start Size Min", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vStartSizeMin.x, &fStep, NULL, "%.3f"))
				MINMAX(m_tSelected_Particle_Desc.vStartSizeMin.x, 0.f, 999.f);
			m_tSelected_Particle_Desc.vStartSizeMin.y = m_tSelected_Particle_Desc.vStartSizeMin.x;

			if (ImGui::InputScalar("Start Size Max", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vStartSizeMax.x, &fStep, NULL, "%.3f"))
				MINMAX(m_tSelected_Particle_Desc.vStartSizeMax.x, 0.f, 999.f);
			m_tSelected_Particle_Desc.vStartSizeMax.y = m_tSelected_Particle_Desc.vStartSizeMax.x;
		}
		else
		{
			m_tSelected_Particle_Desc.bisSquare = false;
			if (ImGui::InputScalar("##Particle Start Size Min", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vStartSizeMin.x, &fStep, NULL, "%.3f"))
				MINMAX(m_tSelected_Particle_Desc.vStartSizeMin.x, 0.f, 999.f);

			ImGui::SameLine();
			if (ImGui::InputScalar("Start Size Min", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vStartSizeMin.y, &fStep, NULL, "%.3f"))
				MINMAX(m_tSelected_Particle_Desc.vStartSizeMin.y, 0.f, 999.f);

			if (ImGui::InputScalar("##Particle Start Size Max", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vStartSizeMax.x, &fStep, NULL, "%.3f"))
				MINMAX(m_tSelected_Particle_Desc.vStartSizeMax.x, 0.f, 999.f);

			ImGui::SameLine();
			if (ImGui::InputScalar("Start Size Max", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vStartSizeMax.y, &fStep, NULL, "%.3f"))
				MINMAX(m_tSelected_Particle_Desc.vStartSizeMax.y, 0.f, 999.f);
		}

		if (ImGui::InputScalar("##Size Over LifeTime X", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vSizeOverLife.x, &fStep, NULL, "%.3f"))
			MINMAX(m_tSelected_Particle_Desc.vSizeOverLife.x, -10.f, 999.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("Size Over LifeTime", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vSizeOverLife.y, &fStep, NULL, "%.3f"))
			MINMAX(m_tSelected_Particle_Desc.vSizeOverLife.y, -10.f, 999.f);

	}

	// Color
	Set_Color();


	Set_Light();

	// Rotation
	Set_Rotation();

	// Power
	Set_Power();

	// Dissolve
	Set_Dissolve();

	// Texture
	Set_Textures();

	// Attach
	Set_AttachDesc();

	// TargetPos
	Set_TargetPos();

	// Update
	if (ImGui::Button("Update Particle") || m_pGameInstance->GetKeyDown(eKeyCode::Enter))
	{
		m_pSelectedEffect->Set_AttachDesc(m_tSelected_Effect_AttachDesc);
		m_pSelectedEffect->Update_UV_Desc(m_tSelected_Common_UVDesc);
		dynamic_cast<CEffect_Paticle_Tool*>(m_pSelectedEffect)->Update_Desc(m_tSelected_Particle_Desc, m_tSelected_Common_Desc);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Effect"))
	{
		SaveEffect();
	}
}

void CEffect_Tool::UpdateMesh()
{
	if (CEffect::MESH != m_pSelectedEffect->Get_EffetType())
		return;

	ImGui::Checkbox("Change All(Group, SameType)", &m_bChangeAll);

	// Name
	EffectTagSetting();

	// Time
	if (ImGui::CollapsingHeader("Time##Mesh"))
	{

		// Total Play Time
		Set_Total_Time();

		ImGui::Checkbox("Enable MaintainTime", &m_tSelected_Mesh_Desc.bUseMaintainTime);
		if (true == m_tSelected_Mesh_Desc.bUseMaintainTime)
		{
			_float fTime = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("Mesh Maintain Start Time", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.fMaintainStartTime, &fTime, NULL, "%.4f");

			//ImGui::SameLine();
			ImGui::InputScalar("Maintain Time", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.fMaintainTime, &fTime, NULL, "%.4f");

			ImGui::PopItemWidth();
		}
		else
		{
			m_tSelected_Mesh_Desc.fMaintainStartTime = 0.f;
			m_tSelected_Mesh_Desc.fMaintainTime = 0.f;
		}


		//Delay
		Set_Start_Delay();

	}

	if (ImGui::CollapsingHeader("Loop, Local, Shader"))
	{
		// Loop
		ImGui::Checkbox("Effect Loop", &m_tSelected_Common_Desc.bEffectLoop);
		ImGui::SameLine();

		// Pass
		ImGui::PushItemWidth(90.f);
		ImGui::InputInt("Pass Index", &m_tSelected_Common_Desc.iPassIdx);

		// RenderGroup
		Set_RenderGroup();
	}

	// Distortion
	Set_Distortion();

	//Start Position
	Set_Start_Position();

	// Speed
	Set_SpeedInfo();

	//Size
	if (ImGui::CollapsingHeader("SIZE"))
	{
		_float fStep = 0.01f;
		ImGui::PushItemWidth(80.f);

		ImGui::Checkbox("Enable Lerp Size##Mesh", &m_tSelected_Common_Desc.bLerpSize);

		ImGui::SameLine();


		ImGui::Checkbox("##Change Also Y", &m_bChangeAlsoY);
		ImGui::SameLine();
		ImGui::Checkbox("Change Also Y, Z", &m_bChangeAlsoZ);

		if (ImGui::InputScalar("##Mesh Start Size MinX", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vStartSizeMin.x, &fStep, NULL, "%.4f"))
		{
			if (true == m_bChangeAlsoY)
				m_tSelected_Mesh_Desc.vStartSizeMin.y = m_tSelected_Mesh_Desc.vStartSizeMin.x;
			if (true == m_bChangeAlsoZ)
				m_tSelected_Mesh_Desc.vStartSizeMin.z = m_tSelected_Mesh_Desc.vStartSizeMin.x;
		}
		ImGui::SameLine();
		ImGui::InputScalar("##Mesh Start Size MinY", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vStartSizeMin.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Start Size Min##Mesh", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vStartSizeMin.z, &fStep, NULL, "%.4f");


		if (ImGui::InputScalar("##Mesh Start Size MaxX", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vStartSizeMax.x, &fStep, NULL, "%.4f"))
		{
			if (true == m_bChangeAlsoY)
				m_tSelected_Mesh_Desc.vStartSizeMax.y = m_tSelected_Mesh_Desc.vStartSizeMax.x;
			if (true == m_bChangeAlsoZ)
				m_tSelected_Mesh_Desc.vStartSizeMax.z = m_tSelected_Mesh_Desc.vStartSizeMax.x;
		}
		ImGui::SameLine();
		ImGui::InputScalar("##Mesh Start Size MaxY", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vStartSizeMax.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Start Size Max##Mesh", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vStartSizeMax.z, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##Mesh Size Over LifeX", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vScaleOverLife.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Mesh Size Over LifeY", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vScaleOverLife.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Size Over Life##Mesh", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vScaleOverLife.z, &fStep, NULL, "%.4f");

		ImGui::Checkbox("Lerp Size Second Phase", &m_tSelected_Mesh_Desc.bSizePhaseSecond);


		ImGui::InputScalar("Second Phase Time", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.fSecondPhaseTime, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##Mesh Second Lerp SizeX", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vScaleOverLife_Secod.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Mesh Second Lerp SizeY", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vScaleOverLife_Secod.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Mesh Second Lerp Size", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vScaleOverLife_Secod.z, &fStep, NULL, "%.4f");

	}

	// Color
	Set_Color();

	// Start Rotation
	Set_Rotation();

	// Turn
	if (ImGui::CollapsingHeader("TURN"))
	{
		ImGui::Checkbox("Enable Lerp Rotation", &m_tSelected_Mesh_Desc.bLerpRotation);
		ImGui::SameLine();
		ImGui::Checkbox("Local", &m_tSelected_Common_Desc.bRotationLocal);

		ImGui::PushItemWidth(80.f);
		_float fStep = 0.5f;
		ImGui::InputScalar("Mesh Turn Speed", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.fTurnSpeed, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##Mesh Turn Axis X", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vRotationAxis.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Mesh Turn Axis Y", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vRotationAxis.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Mesh Rotation Axis", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vRotationAxis.z, &fStep, NULL, "%.4f");


		ImGui::InputScalar("##Mesh Turn X", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vRotationOverLife.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Mesh Turn Y", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vRotationOverLife.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Mesh Turn Over Life", ImGuiDataType_Float, &m_tSelected_Mesh_Desc.vRotationOverLife.z, &fStep, NULL, "%.4f");
		ImGui::PopItemWidth();

	}

	// Dissolve
	Set_Dissolve();

	// Mesh
	Set_Mesh();


	// Texture
	Set_Textures();

	// Attach
	Set_AttachDesc();

	if (ImGui::Button("Update Mesh Effect") || m_pGameInstance->GetKeyDown(eKeyCode::Enter))
	{
		m_pSelectedEffect->Set_AttachDesc(m_tSelected_Effect_AttachDesc);
		m_pSelectedEffect->Update_UV_Desc(m_tSelected_Common_UVDesc);
		dynamic_cast<CEffect_Mesh_Tool*>(m_pSelectedEffect)->Update_MeshEffect(m_tSelected_Mesh_Desc, m_tSelected_Common_Desc);

		if (true == m_bChangeAll)
			m_pSelectedGroup->UpdateAll_Desc(CEffect::MESH, m_tSelected_Common_Desc, m_tSelected_Common_UVDesc, m_tSelected_Effect_AttachDesc, &m_tSelected_Mesh_Desc);
	}
	ImGui::SameLine();

	if (ImGui::Button("Save Effect"))
	{
		SaveEffect();
	}
}

void CEffect_Tool::UpdateTrail()
{
	if (CEffect::TRAIL != m_pSelectedEffect->Get_EffetType())
		return;

	ImGui::Checkbox("Effect Loop", &m_tSelected_Common_Desc.bEffectLoop);
	ImGui::SameLine();

	ImGui::PushItemWidth(90.f);
	ImGui::InputInt("Pass Index", &m_tSelected_Common_Desc.iPassIdx);
	ImGui::PopItemWidth();


	// Total Play Time
	Set_Total_Time();

	// Name
	EffectTagSetting();


	// Distortion
	Set_Distortion();

	// Color 
	Set_Color();

	// Start Rotation
	Set_Rotation();

	// MaxVertex
	if (ImGui::CollapsingHeader("Trail Trait"))
	{
		ImGui::PushItemWidth(80.f);
		_uint iStep = { 1 };
		ImGui::InputScalar("Max Vertex", ImGuiDataType_U32, &m_tSelecete_Trail_Desc.iMaxVertexCount, &iStep, NULL, "%d");

		// catmulrom
		ImGui::InputScalar("Catmul Rom Count", ImGuiDataType_U32, &m_tSelecete_Trail_Desc.iCatmullRomCount, &iStep, NULL, "%d");

		ImGui::PopItemWidth();
	}

	// Dissolve
	Set_Dissolve();

	// Texture
	Set_Textures();

	// Attach
	Set_AttachDesc();


	if (ImGui::Button("Update Trail Effect") || m_pGameInstance->GetKeyDown(eKeyCode::Enter))
	{
		m_pSelectedEffect->Set_AttachDesc(m_tSelected_Effect_AttachDesc);
		m_pSelectedEffect->Update_UV_Desc(m_tSelected_Common_UVDesc);
		dynamic_cast<CEffect_Trail_Tool*>(m_pSelectedEffect)->Remake_Trail(m_tSelecete_Trail_Desc, m_tSelected_Common_Desc);
	}
	ImGui::SameLine();

	if (ImGui::Button("Save Effect"))
	{
		SaveEffect();
	}
}

void CEffect_Tool::EffectTick(_float fTimeDelta)
{
	switch (m_iGroupID)
	{
	case 0:
		if (nullptr != m_pSelectedEffect)
			m_pSelectedEffect->Tick(fTimeDelta);
		break;
	case 1:
		if (nullptr != m_pSelectedGroup)
			m_pSelectedGroup->Tick(fTimeDelta);
		break;
	}
}

void CEffect_Tool::EffectLateTick(_float fTimeDelta)
{
	switch (m_iGroupID)
	{
	case 0:
		if (nullptr != m_pSelectedEffect)
			m_pSelectedEffect->LateTick(fTimeDelta);
		break;
	case 1:
		if (nullptr != m_pSelectedGroup)
			m_pSelectedGroup->LateTick(fTimeDelta);
		break;
	}
}



// ***********************
// *					 *
// *		List		 *
// *					 *
// ***********************
void CEffect_Tool::ShowParticleEffectList()
{
	if (true == m_ParticleEffects.empty())
	{
		if (m_pSelectedEffect)
			Safe_Release(m_pSelectedEffect);
		return;
	}


	ImGui::Text("Particle List");
	if (ImGui::BeginListBox("##Particle List", ImVec2(350.f, 200.f)))
	{
		for (size_t i = 0; i < m_ParticleEffects.size(); ++i)
		{
			const _bool bIsSelected = m_iEffctIdx == i;

			string strFileName;
			strFileName.append(m_ParticleEffects[i]->Get_EffectTag());

			if (ImGui::Selectable(strFileName.c_str(), bIsSelected))
			{
				m_iEffctIdx = (_uint)i;
				m_bChangeEffect = true;
			}

			if (true == bIsSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
		MINMAX(m_iEffctIdx, 0, _uint(m_ParticleEffects.size() - 1));
		Chage_SelectEffect(m_ParticleEffects[m_iEffctIdx]);
	}

	if (KEY_DOWN(eKeyCode::Delete))
	{
		if (nullptr != m_pSelectedEffect)
		{
			for (auto iter = m_ParticleEffects.begin(); iter != m_ParticleEffects.end(); ++iter)
			{
				if (*iter == m_pSelectedEffect)
				{
					Safe_Release(*iter);
					m_ParticleEffects.erase(iter);
					break;
				}
			}
			//ADD_EVENT(bind(&CEffect_Tool::Release_Effect, this, m_pSelectedEffect));
			Safe_Release(m_pSelectedGroup);
		}
	}
}

void CEffect_Tool::ShowMeshEffectList()
{
	if (true == m_MeshEffects.empty())
	{
		if (m_pSelectedEffect)
			Safe_Release(m_pSelectedEffect);
		return;
	}

	ImGui::Text("Mesh List");
	if (ImGui::BeginListBox("##Mesh List", ImVec2(350.f, 200.f)))
	{
		for (size_t i = 0; i < m_MeshEffects.size(); ++i)
		{
			const _bool bIsSelected = m_iEffctIdx == i;

			string strFileName;
			strFileName.append(m_MeshEffects[i]->Get_EffectTag());

			if (ImGui::Selectable(strFileName.c_str(), bIsSelected))
			{
				m_iEffctIdx = (_uint)i;
				m_bChangeEffect = true;
				break;
			}

			if (true == bIsSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();

		Chage_SelectEffect(m_MeshEffects[m_iEffctIdx]);
	}
	if (KEY_DOWN(eKeyCode::Delete))
	{
		if (nullptr != m_pSelectedEffect)
		{

			for (auto iter = m_MeshEffects.begin(); iter != m_MeshEffects.end(); ++iter)
			{
				if (*iter == m_pSelectedEffect)
				{
					Safe_Release(*iter);
					m_MeshEffects.erase(iter);
					break;
				}
			}
			ADD_EVENT(bind(&CEffect_Tool::Release_Effect, this, m_pSelectedEffect));
		}
	}
}

void CEffect_Tool::ShowTrailEffectList()
{
	if (true == m_TrailEffects.empty())
	{
		if (m_pSelectedEffect)
			Safe_Release(m_pSelectedEffect);
		return;
	}

	ImGui::Text("Trail List");
	if (ImGui::BeginListBox("##Trail List", ImVec2(350.f, 200.f)))
	{
		for (size_t i = 0; i < m_TrailEffects.size(); ++i)
		{
			const _bool bIsSelected = m_iEffctIdx == i;

			string strFileName;
			strFileName.append(m_TrailEffects[i]->Get_EffectTag());

			if (ImGui::Selectable(strFileName.c_str(), bIsSelected))
			{
				m_iEffctIdx = (_uint)i;
				m_bChangeEffect = true;
			}

			if (true == bIsSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
		Chage_SelectEffect(m_TrailEffects[m_iEffctIdx]);
	}

	if (KEY_DOWN(eKeyCode::Delete))
	{
		if (nullptr != m_pSelectedEffect)
		{

			for (auto iter = m_TrailEffects.begin(); iter != m_TrailEffects.end(); ++iter)
			{
				if (*iter == m_pSelectedEffect)
				{
					Safe_Release(*iter);
					m_TrailEffects.erase(iter);
					break;
				}
			}
			ADD_EVENT(bind(&CEffect_Tool::Release_Effect, this, m_pSelectedEffect));
		}
	}
}




void CEffect_Tool::Set_Mesh()
{
	// BlackHole, Common, Item, Player 
	if (CEffect::MESH != m_pSelectedEffect->Get_EffetType())
		return;
	if (false == ImGui::CollapsingHeader("MODEL"))
		return;

	string ModelTag = Convert_WStrToStr(dynamic_cast<CEffect_Mesh_Tool*>(m_pSelectedEffect)->Get_ModelComp());
	ImGui::Text("ModelTag : "); ImGui::SameLine();
	ImGui::Text(ModelTag.c_str());

	if (ImGui::Button("Change Model"))
	{
		dynamic_cast<CEffect_Mesh_Tool*>(m_pSelectedEffect)->Remove_Model_Component();
		m_bSelectModel = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Model"))
		dynamic_cast<CEffect_Mesh_Tool*>(m_pSelectedEffect)->Remove_Model_Component();

	if (false == m_bSelectModel)
		return;

	ImGui::Begin("Select Model", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	ImGui::SeparatorText("Select Model");

	if (true == m_ModelNames.empty())
		assert(false);

	for (auto& Name : m_ModelNames)
	{
		string strGroup = Convert_WStrToStr(Name.first);
		if (ImGui::TreeNode(strGroup.c_str()))
		{
			string strDirName = strGroup + "List";

			if (ImGui::BeginListBox(strDirName.c_str(), ImVec2(300.f, 200.f)))
			{
				for (size_t i = 0; i < Name.second.size(); ++i)
				{
					const _bool bIsSelected = m_iModelIdx == i;

					string strFileName;
					strFileName = Convert_WStrToStr(Name.second[i]);

					if (ImGui::Selectable(strFileName.c_str(), bIsSelected))
						m_iModelIdx = (_uint)i;

					if (true == bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			if (ImGui::Button("Select Model"))
			{
				dynamic_cast<CEffect_Mesh_Tool*>(m_pSelectedEffect)->Set_Model_Component(Name.second[m_iModelIdx], m_eLevel);
				m_bSelectModel = false;
			}

			ImGui::TreePop();
		}
	}

	if (ImGui::Button("Back") || KEY_DOWN(eKeyCode::ESC))
		m_bSelectModel = false;

	ImGui::End();
}

void CEffect_Tool::ShowGroupList()
{
	if (true == m_EffectGroup.empty())
		return;

	ImGui::Text("Group List");	ImGui::SameLine(210.f, 0.f);
	ImGui::Text("Selected Group Effects");

	const _char** szGroups = new const _char * [m_EffectGroup.size()];
	_int iIdx = 0;
	for (auto& Pair = m_EffectGroup.begin(); Pair != m_EffectGroup.end(); ++Pair)
		szGroups[iIdx++] = Pair->first.c_str();

	ImGui::PushItemWidth(200.f);
	ImGui::ListBox("##Group ListBox", &m_iGroupIdx, szGroups, (_int)m_EffectGroup.size(), 5);
	MINMAX(m_iGroupIdx, 0, (_int)m_EffectGroup.size() - 1);
	auto Pair = m_EffectGroup.find(szGroups[m_iGroupIdx]);

	if (m_EffectGroup.end() != Pair)
		m_pSelectedGroup = Pair->second;

	Safe_Delete_Array(szGroups);

	if (KEY_DOWN(eKeyCode::Delete) && 1 == m_iGroupID && KEY_PUSHING(eKeyCode::RShift))
	{
		if (true != m_EffectGroup.empty())
		{
			auto& Pair = m_EffectGroup.begin();
			advance(Pair, m_iGroupIdx);
			for (size_t i = 0; i < m_pSelectedGroup->Get_GroupSize(); ++i)
			{
				if (m_pSelectedEffect == m_pSelectedGroup->Get_Effect_by_Index((_uint)i))
				{
					ADD_EVENT(bind(&CEffect_Tool::Release_Effect, this, m_pSelectedEffect));
					break;
				}

			}
			ADD_EVENT(bind(&CEffect_Tool::Release_Effect, this, m_pSelectedGroup));
			m_EffectGroup.erase(Pair);

			if (true == m_EffectGroup.empty())
				m_EffectGroup.clear();
		}
	}

	if (nullptr == m_pSelectedGroup || 0 == m_pSelectedGroup->Get_GroupSize())
		return;

	ImGui::SameLine();

	ImGui::BeginChild("##EffectListChild", ImVec2(200.f, 150.f));

	if (ImGui::BeginListBox("##Selected Effects List", ImVec2(200.f, 120.f)))
	{
		for (size_t i = 0; i < m_pSelectedGroup->Get_GroupSize(); ++i)
		{
			const _bool bIsSelected = m_iGroupEffectIdx == i;

			string strFileName;
			strFileName.append(m_pSelectedGroup->Get_Effect_by_Index((_uint)i)->Get_EffectTag());

			if (ImGui::Selectable(strFileName.c_str(), bIsSelected))
			{
				m_iGroupEffectIdx = (_uint)i;
				m_bChangeEffect = true;
			}

			if (true == bIsSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();

		MINMAX(m_iGroupEffectIdx, 0, (_int)m_pSelectedGroup->Get_GroupSize() - 1);
		Chage_SelectEffect(m_pSelectedGroup->Get_Effect_by_Index(m_iGroupEffectIdx));
	}
	ImGui::EndChild();
}

void CEffect_Tool::SelectImage(TEX_TYPE eType, _char* pName)
{
	if (TEX_END == eSelecting_Tex_Type)
		return;

	_char szWindowName[MAX_PATH] = "Select Image";
	strcat_s(szWindowName, MAX_PATH, pName);

	ImGui::Begin(szWindowName, nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	ImGui::SeparatorText("Select Image");

	if (true == m_TexName.empty())
		return;

	for (auto& Name : m_TexName)
	{
		string strGroup = Convert_WStrToStr(Name.first);

		if (ImGui::TreeNode(strGroup.c_str()))
		{
			string Tmp = "##" + strGroup + "Tree";

			ImGui::BeginChild(Tmp.c_str(), ImVec2(180.f, 200.f));

			Tmp = "##" + strGroup + "List";

			if (ImGui::BeginListBox(Tmp.c_str(), ImVec2(170.f, 200.f)))
			{
				for (size_t i = 0; i < Name.second.size(); ++i)
				{
					const _bool bIsSelected = m_iTextureIdx == i;

					string strFileName;
					strFileName = Convert_WStrToStr(Name.second[i]);

					if (ImGui::Selectable(strFileName.c_str(), bIsSelected))
					{
						m_iTextureIdx = (_uint)i;
						string strFileName;
						strFileName = Convert_WStrToStr(Name.second[m_iTextureIdx]);

						m_pSelectedEffect->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);

						if (true == m_bChangeAll)
						{
							for (size_t i = 0; i < m_pSelectedGroup->Get_GroupSize(); ++i)
							{
								if (SAME_TYPE(m_pSelectedGroup->Get_Effect_by_Index((_uint)i)))
								{
									m_pSelectedGroup->Get_Effect_by_Index((_uint)i)->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);
								}
							}
						}
					}

					if (true == bIsSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
			ImGui::EndChild();

			if (KEY_DOWN(eKeyCode::Down))
			{
				++m_iTextureIdx;
				MINMAX(m_iTextureIdx, 0, (_uint)Name.second.size() - 1);
				string strFileName;
				strFileName = Convert_WStrToStr(Name.second[m_iTextureIdx]);

				m_pSelectedEffect->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);

				if (true == m_bChangeAll)
				{
					for (size_t i = 0; i < m_pSelectedGroup->Get_GroupSize(); ++i)
					{
						if (SAME_TYPE(m_pSelectedGroup->Get_Effect_by_Index((_uint)i)))
						{
							m_pSelectedGroup->Get_Effect_by_Index((_uint)i)->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);
						}
					}
				}
			}
			if (KEY_DOWN(eKeyCode::Up))
			{
				--m_iTextureIdx;
				MINMAX(m_iTextureIdx, 0, (_uint)Name.second.size() - 1);
				string strFileName;
				strFileName = Convert_WStrToStr(Name.second[m_iTextureIdx]);

				m_pSelectedEffect->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);

				if (true == m_bChangeAll)
				{
					for (size_t i = 0; i < m_pSelectedGroup->Get_GroupSize(); ++i)
					{
						if (SAME_TYPE(m_pSelectedGroup->Get_Effect_by_Index((_uint)i)))
						{
							m_pSelectedGroup->Get_Effect_by_Index((_uint)i)->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);
						}
					}
				}
			}

			ImGui::SameLine();

			ImGui::BeginChild("##Image", ImVec2(200.f, 200.f));

			auto Image = m_Textures.find(Name.first);

			if (m_iTextureIdx < Image->second.size())
			{
				ImGui::Image((void*)Image->second[m_iTextureIdx], ImVec2(160.f, 160.f));
			}
			ImGui::EndChild();

			if (ImGui::Button("Select"))
			{
				string strFileName;
				strFileName = Convert_WStrToStr(Name.second[m_iTextureIdx]);

				m_pSelectedEffect->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);

				if (true == m_bChangeAll)
				{
					for (size_t i = 0; i < m_pSelectedGroup->Get_GroupSize(); ++i)
					{
						if (SAME_TYPE(m_pSelectedGroup->Get_Effect_by_Index((_uint)i)))
						{
							m_pSelectedGroup->Get_Effect_by_Index((_uint)i)->Set_TextureName(CEffect::TEX_TYPE(eType), strFileName.c_str(), m_eLevel);
						}
					}
				}
			}

			if (ImGui::Button("Back##Image"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_END;

			ImGui::TreePop();
		}
	}
	if (KEY_DOWN(eKeyCode::ESC))
		eSelecting_Tex_Type = TEX_TYPE::TEX_END;
	ImGui::End();
}

void CEffect_Tool::Manage_Group()
{
	ImGui::Begin("Clone To Group", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	if (false == m_EffectGroup.empty())
	{

		ImGui::Text("Group List");
		ImGui::SameLine(200.f, 0.f);
		ImGui::Text("Selected Group Effects");

		const _char** szGroups = new const _char * [m_EffectGroup.size()];
		_int iIdx = 0;
		for (auto& Pair = m_EffectGroup.begin(); Pair != m_EffectGroup.end(); ++Pair)
			szGroups[iIdx++] = Pair->first.c_str();

		ImGui::PushItemWidth(200.f);
		ImGui::ListBox("##Manage Group ListBox", &m_iManageGroupIdx, szGroups, (_int)m_EffectGroup.size(), 5);
		auto Pair = m_EffectGroup.find(szGroups[m_iManageGroupIdx]);

		CEffect_Group_Tool* pGroup = Pair->second;

		Safe_Delete_Array(szGroups);

		ImGui::SameLine();

		ImGui::BeginChild("##GroupEffectsChild", ImVec2(200.f, 100.f));

		const _char** szEffectTags = new const _char * [(_int)pGroup->Get_GroupSize()];
		for (size_t i = 0; i < pGroup->Get_GroupSize(); ++i)
		{
			szEffectTags[i] = pGroup->Get_Effect_by_Index((_uint)i)->Get_EffectTag();
		}

		ImGui::ListBox("##Manage Group Effects ListBox", &m_iManageEffecIdx, szEffectTags, (_int)pGroup->Get_GroupSize(), 5);

		Safe_Delete_Array(szEffectTags);

		ImGui::EndChild();

		if (ImGui::Button("Clone Effect To Selected Group"))
			pGroup->Add_Effect(dynamic_cast<CEffect*>(m_pSelectedEffect->Clone(nullptr)));

		ImGui::SameLine();
	}
	else
		ImGui::Text("Please Add A Group");


	if (ImGui::Button("Clone Effect To New Group"))
		m_bMakeNewGroup = true;

	ImGui::SameLine();

	if (ImGui::Button("Back") || KEY_DOWN(eKeyCode::ESC))
		m_bManagingGroup = false;

	ImGui::End();

	if (false == m_bMakeNewGroup)
		return;

	ImGui::Begin("Add New Group", nullptr);

	ImGui::InputText("New Group Name", m_szChangeGroupName, MAX_PATH);

	if (m_pGameInstance->GetKeyDown(eKeyCode::Enter) && strcmp(m_szChangeGroupName, ""))
	{
		auto Pair = m_EffectGroup.find(m_szChangeGroupName);

		if (m_EffectGroup.end() != Pair)
			MSG_BOX(TEXT("Already Exist!"));
		else
		{
			CEffect_Group_Tool* pNewGroup = dynamic_cast<CEffect_Group_Tool*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_GroupEffect")));
			pNewGroup->Add_Effect(dynamic_cast<CEffect*>(m_pSelectedEffect->Clone(nullptr)));
			pNewGroup->Set_GroupTag(m_szChangeGroupName);
			m_EffectGroup.emplace(m_szChangeGroupName, pNewGroup);
			m_bMakeNewGroup = false;
		}
	}

	ImGui::End();
}
void CEffect_Tool::Change_GroupTag()
{
	ImGui::Begin("Change Group Name", nullptr);

	ImGui::InputText("Group Name##Change", m_szChangeGroupName, MAX_PATH);

	if (KEY_DOWN(eKeyCode::Enter))
	{
		auto Pair = m_EffectGroup.find(m_pSelectedGroup->Get_GroupTag());
		if (m_EffectGroup.end() == Pair)
		{
			MSG_BOX(TEXT("FAILED - Change_GroupTag"));
			return;
		}

		m_EffectGroup.erase(Pair);

		m_EffectGroup.emplace(m_szChangeGroupName, m_pSelectedGroup);

		m_pSelectedGroup->Set_GroupTag(m_szChangeGroupName);
		m_bChange_GroupName = false;
	}

	if (KEY_DOWN(eKeyCode::ESC))
		m_bChange_GroupName = false;


	ImGui::End();

}
void CEffect_Tool::Set_Light()
{
	if (false == ImGui::CollapsingHeader("LIGHT"))
		return;
	_float fStep = 0.1f;

	ImGui::PushItemWidth(80.f);
	ImGui::InputScalar("##Light Ambient R", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vLightAmbient.x, &fStep, NULL, "%.4f");

	ImGui::SameLine();
	ImGui::InputScalar("##Light Ambient G", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vLightAmbient.y, &fStep, NULL, "%.4f");

	ImGui::SameLine();
	ImGui::InputScalar("##Light Ambient B", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vLightAmbient.z, &fStep, NULL, "%.4f");

	ImGui::SameLine();
	ImGui::InputScalar("Light Ambient", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vLightAmbient.w, &fStep, NULL, "%.4f");


	ImGui::InputScalar("##Light Dir X", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vLightDir.x, &fStep, NULL, "%.4f");
	ImGui::SameLine();
	ImGui::InputScalar("##Light Dir Y", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vLightDir.y, &fStep, NULL, "%.4f");
	ImGui::SameLine();
	ImGui::InputScalar("Light Dir", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vLightDir.z, &fStep, NULL, "%.4f");
	ImGui::PopItemWidth();
}

void CEffect_Tool::Set_TargetPos()
{
	if (false == ImGui::CollapsingHeader("Set Target Bone"))
		return;

	ImGui::Checkbox("Enable Target Bone", &m_tSelected_Particle_Desc.bHasTargetPoint);


	if (false == m_tSelected_Particle_Desc.bHasTargetPoint)
		return;

	if (nullptr == m_pSelectedObj)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Select OBj!");
		return;
	}

	ImGui::Text("Current Target Bone Name : ");
	ImGui::SameLine();
	ImGui::Text(m_tSelected_Effect_AttachDesc.szTargetBoneName);

	ImGui::InputText("##Target Bone Name", m_szTargetBoneName, MAX_PATH);
	if (ImGui::Button("Change Bone##Target"))
	{
		if (m_tSelected_Effect_AttachDesc.pTargetBone)
			Safe_Release(m_tSelected_Effect_AttachDesc.pTargetBone);

		strcpy_s(m_tSelected_Effect_AttachDesc.szTargetBoneName, MAX_PATH, m_szTargetBoneName);

		m_tSelected_Effect_AttachDesc.pTargetBone = m_pSelectedObj->Get_Model()->Get_Bone(m_szTargetBoneName);
		if (nullptr != m_tSelected_Effect_AttachDesc.pTargetBone)
			Safe_AddRef(m_tSelected_Effect_AttachDesc.pTargetBone);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Bone##Target"))
	{
		if (m_tSelected_Effect_AttachDesc.pTargetBone)
			Safe_Release(m_tSelected_Effect_AttachDesc.pTargetBone);

		strcpy_s(m_tSelected_Effect_AttachDesc.szTargetBoneName, MAX_PATH, "");
	}

	ImGui::Checkbox("World Offset ##Target", &m_tSelected_Particle_Desc.bTargetWorldOffSet);
	ImGui::SameLine();
	ImGui::Checkbox("Follow TargetPos", &m_tSelected_Particle_Desc.bFollowTargetPos);
	ImGui::SameLine();
	ImGui::Checkbox("Life Zero At Target Pos", &m_tSelected_Particle_Desc.bTargetLifeZero);


	_float fStep = { 0.1f };
	ImGui::PushItemWidth(80.f);
	ImGui::InputScalar("##Target Point Offset X", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vTargetPointOffset.x, &fStep, NULL, "%.4f");
	ImGui::SameLine();
	ImGui::InputScalar("##Target Point Offset Y", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vTargetPointOffset.y, &fStep, NULL, "%.4f");
	ImGui::SameLine();
	ImGui::InputScalar("Target Point Offset", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vTargetPointOffset.z, &fStep, NULL, "%.4f");

	ImGui::InputScalar("Margin", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fMargin, &fStep, NULL, "%.4f");

	ImGui::PopItemWidth();

}
void CEffect_Tool::Bind_KeyFrame_Effect(_bool bSingle, _uint CurKeyFrame)
{
	if (true == bSingle)
	{
		if (nullptr == m_pSelectedEffect)
			return;

		m_iBindedKeyFrame = CurKeyFrame;
		m_bBindedSingle = true;
	}
	else
	{
		if (nullptr == m_pSelectedGroup)
			return;
		m_iBindedKeyFrame = CurKeyFrame;
		m_bBindedSingle = false;
	}
	m_bBindedKeyFrame = true;
}
void CEffect_Tool::Start_KeyFrame_Effect(_uint CurKeyFrame)
{
	if (false == m_bBindedKeyFrame)
		return;


	if (m_bBindedSingle)
	{
		if (nullptr == m_pSelectedEffect)
		{
			m_bBindedKeyFrame = false;
			return;
		}
		if (CurKeyFrame == m_iBindedKeyFrame && true == m_pSelectedEffect->Get_End())
			m_pSelectedEffect->Reset_Effect();
	}
	{
		if (nullptr == m_pSelectedGroup)
		{
			m_bBindedKeyFrame = false;
			return;
		}

		if (CurKeyFrame == m_iBindedKeyFrame)
		{
			//if (false == m_pSelectedGroup->Is_Using())
			m_pSelectedGroup->OnEnter_Layer(nullptr);
		}
	}


}
#pragma warning(disable : 6031)
void CEffect_Tool::SaveFileUpdate(const wstring& wstrFileName)
{
	// 고르면, 불러와서, 그룹에 넣고. 다시 저장.

	ifstream fin(wstrFileName);
	if (!fin.is_open())
	{
		MSG_BOX(TEXT("FAILED TO OPEN SAVE FILE"));
		return;
	}

	_tchar wszDir[MAX_PATH] = { TEXT("") };
	_wsplitpath_s(wstrFileName.c_str(), nullptr, 0, wszDir, MAX_PATH, nullptr, 0, nullptr, 0);

	json jsonData;
	fin >> jsonData;
	fin.close();

	LOAD_TYPE eType = { LOAD_END };
	CEffect::EFFECT_TYPE eEffect_Type = { CEffect::EFFECT_END };

	eType = jsonData["Common"]["Load Type"];

	string strGroupTag;

	if (SINGLE == eType)
	{
		m_iGroupID = 0;

		string strEffetTag = jsonData["Effect Tag"];
		eEffect_Type = jsonData[strEffetTag.c_str()]["Effect Type"];
		CEffect* pEffect = { nullptr };

		switch (eEffect_Type)
		{
		case CEffect::PARTICLE:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleEffect")));
			if (FAILED(dynamic_cast<CEffect_Paticle_Tool*>(pEffect)->Load_Effect(strEffetTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			m_ParticleEffects.emplace_back(pEffect);
			break;
		case CEffect::MESH:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MeshEffect")));
			if (FAILED(dynamic_cast<CEffect_Mesh_Tool*>(pEffect)->Load_Effect(strEffetTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			m_MeshEffects.emplace_back(pEffect);
			break;
		case CEffect::TRAIL:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TrailEffect")));
			if (FAILED(dynamic_cast<CEffect_Trail_Tool*>(pEffect)->Load_Effect(strEffetTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			m_TrailEffects.emplace_back(pEffect);
			break;
		}

		Chage_SelectEffect(pEffect);

		SaveEffect(true);
	}
	else if (GROUP == eType)
	{
		m_iGroupID = 1;

		_char szGroupTag[MAX_PATH] = "";
		strcpy_s(szGroupTag, MAX_PATH, string(jsonData["Common"]["Group Data"]["Group Tag"]).c_str());

		auto Pair = m_EffectGroup.find(szGroupTag);

		if (m_EffectGroup.end() == Pair)
		{
			CEffect_Group_Tool* pNewGroup = dynamic_cast<CEffect_Group_Tool*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_GroupEffect")));
			pNewGroup->Set_GroupTag(szGroupTag);
			pNewGroup->Load_Group(jsonData);
			m_EffectGroup.emplace(szGroupTag, pNewGroup);
		}
		else
			Pair->second->Load_Group(jsonData);

		Pair = m_EffectGroup.find(szGroupTag);

		if (Pair == m_EffectGroup.end())
		{
			MSG_BOX(TEXT("FAILED!"));
			return;
		}

		m_pSelectedGroup = Pair->second;

		Save_Group(true, wszDir);
	}
	else
	{
		MSG_BOX(TEXT("FAILED TO LOAD"));
		return;
	}
}
void CEffect_Tool::SaveFileUpdateFunc(const wstring& wstrFileName)
{
	fs::path SaveFilePath(wstrFileName);

	for (const fs::directory_entry& entry : fs::directory_iterator(SaveFilePath))
	{
		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));
			SaveFileUpdateFunc(wstrPath);
		}
		else
		{
			fs::path fileName = entry.path().filename();
			if (fileName.extension().compare(".json"))
				continue;

			SaveFileUpdate(entry.path());
		}
	}
}
#pragma warning(default:6031)
void CEffect_Tool::SaveEffect(_bool isUpdate)
{
	if (nullptr == m_pSelectedEffect)
		return;

	wstring wstrFileName = wszSaveFilePath + Convert_StrToWStr(m_pSelectedEffect->Get_EffectTag()) + TEXT("_Single") + TEXT(".json");

	ofstream fout(wstrFileName);
	if (!fout.is_open())
	{
		MSG_BOX(TEXT("Failed To Open Save File"));
		return;
	}

	nlohmann::ordered_json jsonData;

	LOAD_TYPE eType = SINGLE;
	jsonData["Common"]["Load Type"] = eType;

	jsonData["Effect Tag"] = m_pSelectedEffect->Get_EffectTag();

	CEffect::EFFECT_TYPE eEffect_Type = m_pSelectedEffect->Get_EffetType();
	switch (eEffect_Type)
	{
	case CEffect::PARTICLE:
		if (FAILED(dynamic_cast<CEffect_Paticle_Tool*>(m_pSelectedEffect)->Save_Effect(m_pSelectedEffect->Get_EffectTag(), jsonData)))
			MSG_BOX(TEXT("FAIL TO SAVE"));
		break;
	case CEffect::MESH:
		if (FAILED(dynamic_cast<CEffect_Mesh_Tool*>(m_pSelectedEffect)->Save_Effect(m_pSelectedEffect->Get_EffectTag(), jsonData)))
			MSG_BOX(TEXT("FAIL TO SAVE"));
		break;
	case CEffect::TRAIL:
		if (FAILED(dynamic_cast<CEffect_Trail_Tool*>(m_pSelectedEffect)->Save_Effect(m_pSelectedEffect->Get_EffectTag(), jsonData)))
			MSG_BOX(TEXT("FAIL TO SAVE"));
		break;
	}

	fout << jsonData.dump(4);
	fout.close();

	if (false == isUpdate)
		MSG_BOX(TEXT("Save Complete"));
}

#pragma warning(disable : 6031)
void CEffect_Tool::LoadEffect(const wstring& wstrFileName)
{

	ifstream fin(wstrFileName);
	if (!fin.is_open())
	{
		MSG_BOX(TEXT("FAILED TO OPEN SAVE FILE"));
		return;
	}

	Safe_Release(m_pSelectedEffect);

	LOAD_TYPE eType = { LOAD_END };

	json jsonData;
	fin >> jsonData;
	fin.close();

	eType = jsonData["Common"]["Load Type"];

	if (SINGLE == eType)
	{
		m_iGroupID = 0;
		CEffect::EFFECT_TYPE eEffect_Type = { CEffect::EFFECT_END };

		string strEffetTag = jsonData["Effect Tag"];
		eEffect_Type = jsonData[strEffetTag.c_str()]["Effect Type"];

		CEffect* pEffect = { nullptr };

		switch (eEffect_Type)
		{
		case CEffect::PARTICLE:
		{

			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleEffect")));
			if (FAILED(dynamic_cast<CEffect_Paticle_Tool*>(pEffect)->Load_Effect(strEffetTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			m_ParticleEffects.emplace_back(pEffect);
			m_iTypeID = 1;
			m_eCurrent_Particle_Type = TYPE_PARTICLE;
		}
		break;
		case CEffect::MESH:
		{

			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MeshEffect")));
			if (FAILED(dynamic_cast<CEffect_Mesh_Tool*>(pEffect)->Load_Effect(strEffetTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			m_MeshEffects.emplace_back(pEffect);
			m_iTypeID = 0;
			m_eCurrent_Particle_Type = TYPE_MESH;
		}
		break;
		case CEffect::TRAIL:
		{

			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TrailEffect")));
			if (FAILED(dynamic_cast<CEffect_Trail_Tool*>(pEffect)->Load_Effect(strEffetTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			m_TrailEffects.emplace_back(pEffect);
			m_eCurrent_Particle_Type = TYPE_TRAIL;
			m_iTypeID = 2;
		}
		break;
		}
	}
	else if (GROUP == eType)
	{
		m_iGroupID = 1;

		_char szGroupTag[MAX_PATH] = "";
		strcpy_s(szGroupTag, MAX_PATH, string(jsonData["Common"]["Group Data"]["Group Tag"]).c_str());

		string strGrouptag = szGroupTag;
		auto Pair = m_EffectGroup.find(strGrouptag.c_str());

		if (m_EffectGroup.end() == Pair)
		{
			CEffect_Group_Tool* pNewGroup = dynamic_cast<CEffect_Group_Tool*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_GroupEffect")));
			pNewGroup->Set_GroupTag(strGrouptag.c_str());
			pNewGroup->Load_Group(jsonData);
			m_EffectGroup.emplace(strGrouptag.c_str(), pNewGroup);
			m_pSelectedGroup = pNewGroup;
		}
		else
		{
			Pair->second->Load_Group(jsonData);
			m_pSelectedGroup = Pair->second;
		}

	}
	else
	{
		MSG_BOX(TEXT("FAILED TO LOAD"));
		return;

	}

	MSG_BOX(TEXT("Load Complete"));
}
#pragma warning(default:6031)

void CEffect_Tool::Save_Group(_bool isUpdate, wstring wstrUpdatePath)
{
	if (nullptr == m_pSelectedGroup || 0 == m_pSelectedGroup->Get_GroupSize())
		return;
	wstring wstrFileName;
	if (true == isUpdate)
		wstrFileName = wstrUpdatePath + Convert_StrToWStr(m_pSelectedGroup->Get_GroupTag()) + TEXT(".json");
	else
		wstrFileName = wszSaveFilePath + Convert_StrToWStr(m_pSelectedGroup->Get_GroupTag()) + TEXT(".json");

	ofstream fout(wstrFileName);
	if (!fout.is_open())
	{
		MSG_BOX(TEXT("Failed To Open Save File"));
		return;
	}

	nlohmann::ordered_json jsonData;

	LOAD_TYPE eType = GROUP;
	jsonData["Common"]["Load Type"] = eType;

	m_pSelectedGroup->Save_Group(fout, jsonData);

	fout << jsonData.dump(4);
	fout.close();
	if (false == isUpdate)
		MSG_BOX(TEXT("Save Complete"));
}

void CEffect_Tool::Set_AnimObject(CToolAnimObj* pObject)
{
	if (nullptr == pObject)
		return;


	Safe_Release(m_pSelectedObj);
	Safe_Release(m_tSelected_Effect_AttachDesc.pParentTransform);
	Safe_Release(m_tSelected_Effect_AttachDesc.pBone);
	Safe_Release(m_tSelected_Effect_AttachDesc.pTargetBone);

	m_pSelectedObj = pObject;
	Safe_AddRef(m_pSelectedObj);

	// 이름, 트랜스폼. 뼈
	m_tSelected_Effect_AttachDesc.pParentTransform = m_pSelectedObj->Get_Transform();
	Safe_AddRef(m_tSelected_Effect_AttachDesc.pParentTransform);


	if (strcmp(m_tSelected_Effect_AttachDesc.szBoneName, ""))
	{
		m_tSelected_Effect_AttachDesc.pBone = m_pSelectedObj->Get_Model()->Get_Bone(m_tSelected_Effect_AttachDesc.szBoneName);
		Safe_AddRef(m_tSelected_Effect_AttachDesc.pBone);
	}
	if (strcmp(m_tSelected_Effect_AttachDesc.szTargetBoneName, ""))
	{
		m_tSelected_Effect_AttachDesc.pTargetBone = m_pSelectedObj->Get_Model()->Get_Bone(m_tSelected_Effect_AttachDesc.szTargetBoneName);
		Safe_AddRef(m_tSelected_Effect_AttachDesc.pTargetBone);
	}

}

void CEffect_Tool::Set_Particle_Stop()
{
	ImGui::Checkbox("Enable Particle Stop", &m_tSelected_Particle_Desc.bStopParticle);

	if (false == m_tSelected_Particle_Desc.bStopParticle)
		return;

	ImGui::RadioButton("Time##ParticleStop", &m_iParticleStopID, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Distance", &m_iParticleStopID, 1);

	m_tSelected_Particle_Desc.iStopType = m_iParticleStopID;
	ImGui::PushItemWidth(80.f);
	_float fStep = 0.5f;
	ImGui::InputScalar("Stop Start Time", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fStopTime, &fStep, NULL, "%.4f");

	if (1 == m_iParticleStopID)
		ImGui::InputScalar("Stop Distance", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fStopDistance, &fStep, NULL, "%.4f");

	ImGui::InputScalar("Maintain Time##Particle", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fMaintainTime, &fStep, NULL, "%.4f");

	ImGui::Checkbox("Change Dir After Stop", &m_tSelected_Particle_Desc.bChangeDirAfterStop);

	if (true == m_tSelected_Particle_Desc.bChangeDirAfterStop)
	{
		ImGui::InputScalar("##After Dir MinX", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vAfterDirMin.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##After Dir MinY", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vAfterDirMin.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("After Dir Min", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vAfterDirMin.z, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##After Dir MaxX", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vAfterDirMax.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##After Dir MaxY", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vAfterDirMax.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("After Dir Max", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vAfterDirMax.z, &fStep, NULL, "%.4f");
	}

	ImGui::PopItemWidth();
}

void CEffect_Tool::Chage_SelectEffect(CEffect* pEffect)
{
	if (false == m_bChangeEffect)
		return;

	if (nullptr != m_pSelectedEffect)
		Safe_Release(m_pSelectedEffect);

	m_pSelectedEffect = pEffect;
	Safe_AddRef(m_pSelectedEffect);

	CEffect::EFFECT_TYPE eType = m_pSelectedEffect->Get_EffetType();
	switch (eType)
	{
	case CEffect::PARTICLE:
		m_tSelected_Particle_Desc = dynamic_cast<CEffect_Paticle_Tool*>(m_pSelectedEffect)->Get_Particle_Desc();
		m_bisSquare = m_tSelected_Particle_Desc.bisSquare;
		break;

	case CEffect::MESH:
		m_tSelected_Mesh_Desc = dynamic_cast<CEffect_Mesh_Tool*>(m_pSelectedEffect)->Get_Mesh_EffectDesc();
		break;

	case CEffect::TRAIL:
		m_tSelecete_Trail_Desc = dynamic_cast<CEffect_Trail_Tool*>(m_pSelectedEffect)->Get_Trail_Desc();
		break;
	}

	m_tSelected_Common_Desc = m_pSelectedEffect->Get_Common_Desc();
	m_tSelected_Common_UVDesc = m_pSelectedEffect->Get_CommonUV_Desc();

	Safe_Release(m_tSelected_Effect_AttachDesc.pBone);
	Safe_Release(m_tSelected_Effect_AttachDesc.pParentTransform);
	Safe_Release(m_tSelected_Effect_AttachDesc.pTargetBone);
	m_tSelected_Effect_AttachDesc = m_pSelectedEffect->Get_Attach_Desc();

	if (m_tSelected_Effect_AttachDesc.pBone)
		Safe_AddRef(m_tSelected_Effect_AttachDesc.pBone);
	if (m_tSelected_Effect_AttachDesc.pParentTransform)
		Safe_AddRef(m_tSelected_Effect_AttachDesc.pParentTransform);
	if (m_tSelected_Effect_AttachDesc.pTargetBone)
		Safe_AddRef(m_tSelected_Effect_AttachDesc.pTargetBone);


	if (m_pSelectedObj)
	{
		Set_AnimObject(m_pSelectedObj);
	}

	m_iTextureUse = m_pSelectedEffect->Get_TexUse();

	for (_uint i = 0; i < TEX_END; ++i)
	{
		m_bUsingTexture[i] = 0 == (m_pSelectedEffect->Get_TexUse() & (1 << (static_cast<_int>(i) + 1))) ? false : true;
	}

	m_bDissolve_Stop = 0 == (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_STOP) ? false : true;
	m_bDissolve_Loop = 0 == (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_LOOP) ? false : true;
	m_bDissolve_Edge = 0 == (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_EDGE) ? false : true;

	if (m_tSelected_Common_UVDesc.iRadialMappingFlag & BASERADIAL)
		m_bBase_Radial = true;
	else
		m_bBase_Radial = false;
	if (m_tSelected_Common_UVDesc.iRadialMappingFlag & ALPHARADIAL)
		m_bAlpha_Radial = true;
	else
		m_bAlpha_Radial = false;

	m_bBase_Normalized_Noise = 1.f == m_tSelected_Common_UVDesc.vBaseNoiseUVInfoFlag.x ? true : false;
	m_bBase_Noise_Offset = 1.f == m_tSelected_Common_UVDesc.vBaseNoiseUVInfoFlag.y ? true : false;
	m_iBase_Noise_OffsetAdd = 1.f == m_tSelected_Common_UVDesc.vBaseNoiseUVInfoFlag.x ? 0 : 1;

	m_bAlpha_Normalized_Noise = 1.f == m_tSelected_Common_UVDesc.vAlphaNoiseUVInfoFlag.x ? true : false;
	m_bAlpha_Noise_Offset = 1.f == m_tSelected_Common_UVDesc.vAlphaNoiseUVInfoFlag.y ? true : false;
	m_iAlpha_Noise_OffsetAdd = 1.f == m_tSelected_Common_UVDesc.vAlphaNoiseUVInfoFlag.z ? 0 : 1;

	m_iBaseNoiseUV = 1.f == m_tSelected_Common_UVDesc.vBaseNoiseRadiusFlag.x ? 0 : 1;
	m_iAlphaNoiseUV = 1.f == m_tSelected_Common_UVDesc.vAlphaNoiseRadiusFlag.x ? 0 : 1;

	m_bChangeEffect = false;

	m_iParticleStopID = m_tSelected_Particle_Desc.iStopType;
}





// ***********************
// *					 *
// * Common Setting Func *
// *					 *
// ***********************


void CEffect_Tool::Release_Effect(CGameObject* pObj)
{
	Safe_Release(pObj);
}

void CEffect_Tool::EffectTagSetting()
{
	if (nullptr == m_pSelectedEffect)
		return;

	// Effect Tag
	//ImGuiTreeNodeFlags_
	ImGuiTreeNodeFlags_ flag = { ImGuiTreeNodeFlags_CollapsingHeader };
	//flag |= 
	if (ImGui::CollapsingHeader("Tag, Group", flag))
	{

		ImGui::SeparatorText("Effect Tag##Single");

		strcpy_s(m_szParticleName, MAX_PATH, m_pSelectedEffect->Get_EffectTag());

		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), m_szParticleName);

		ImGui::PushItemWidth(180.f);
		ImGui::InputText("##Particle Name", m_szChangeName, MAX_PATH);

		if (ImGui::Button("Chanege Name##Effect"))
			if (strcmp(m_szChangeName, ""))
				m_pSelectedEffect->Set_EffectTag(m_szChangeName);



		ImGui::SeparatorText("Group##Group Set");

		if (nullptr != m_pSelectedGroup && 1 == m_iGroupID)
		{
			if (ImGui::Button("Change Group Tag"))
			{
				strcpy_s(m_szChangeGroupName, MAX_PATH, m_pSelectedGroup->Get_GroupTag());
				m_bChange_GroupName = true;
			}

			if (m_bChange_GroupName)
				Change_GroupTag();
		}

		if (ImGui::Button("Clone Effect To Group##Group"))
			m_bManagingGroup = true;

		if (1 == m_iGroupID)
		{
			ImGui::SameLine();
			if (ImGui::Button("Save Group Effects"))
				Save_Group();

			ImGui::SameLine();
			if (ImGui::Button("Remove Effect From Group") || KEY_DOWN(eKeyCode::F8))
			{

				if (m_pSelectedEffect == m_pSelectedGroup->Get_Effect_by_Index(m_iGroupEffectIdx))
					ADD_EVENT(bind(&CEffect_Tool::Release_Effect, this, m_pSelectedEffect));
				m_pSelectedGroup->Remove_Effect(m_iGroupEffectIdx);

				if (0 == m_pSelectedGroup->Get_GroupSize())
				{
					auto& Pair = m_EffectGroup.begin();
					advance(Pair, m_iGroupIdx);
					ADD_EVENT(bind(&CEffect_Tool::Release_Effect, this, m_pSelectedGroup));
					m_EffectGroup.erase(Pair);
				}


				m_iGroupEffectIdx = 0;
			}

			int a = m_pSelectedGroup->m_iCloneNum;
			ImGui::InputInt("Clone Num", &a);
			m_pSelectedGroup->m_iCloneNum = (_uint)a;

		}

		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 20.0f));


		if (true == m_bManagingGroup)
			Manage_Group();
	}
}

void CEffect_Tool::Set_RenderGroup()
{
	m_iRenderGroupId = 0 == m_tSelected_Common_Desc.iRenderGroup ? 0 : 1;

	ImGui::RadioButton("NON_BLEND", &m_iRenderGroupId, 0);
	ImGui::SameLine();
	ImGui::RadioButton("BLEND", &m_iRenderGroupId, 1);

	m_tSelected_Common_Desc.iRenderGroup = 0 == m_iRenderGroupId ? 0 : 1;
}

void CEffect_Tool::Set_Total_Time()
{
	ImGui::PushItemWidth(90.f);
	_float fTotalStep = { 0.5f };
	if (ImGui::InputScalar("Total Play Time", ImGuiDataType_Float, &m_tSelected_Common_Desc.fTotalPlayTime, &fTotalStep, NULL, "%.4f"))
		MINMAX(m_tSelected_Common_Desc.fTotalPlayTime, 0, 10000.f);
	ImGui::PopItemWidth();
}

void CEffect_Tool::Set_TF_Type()
{

	switch (m_tSelected_Common_Desc.iTransformType)
	{
	case CEffect::FIX:
		m_iTFID = 0;
		break;
	case CEffect::FOLLOW:
		m_iTFID = 1;
		break;
	case CEffect::MOVE:
		m_iTFID = 2;
		break;
	}

	ImGui::RadioButton("FIX", &m_iTFID, 0);
	ImGui::SameLine();
	if (CEffect::PARTICLE == m_pSelectedEffect->Get_EffetType())
		ImGui::RadioButton("SPAWN AT BONE", &m_iTFID, 1);
	else
		ImGui::RadioButton("FOLLOW", &m_iTFID, 1);

	ImGui::SameLine();
	ImGui::RadioButton("MOVE", &m_iTFID, 2);

	switch (m_iTFID)
	{
	case 0:
		m_tSelected_Common_Desc.iTransformType = 0;
		break;
	case 1:
		m_tSelected_Common_Desc.iTransformType = 1;
		break;
	case 2:
		m_tSelected_Common_Desc.iTransformType = 2;
		break;
	}
}

void CEffect_Tool::Set_Distortion()
{
	if (ImGui::CollapsingHeader("Distortion"))
	{
		ImGui::Checkbox("Distortion##check", &m_tSelected_Common_Desc.bUseDistortion);

		if (true == m_tSelected_Common_Desc.bUseDistortion)
		{
			ImGui::PushItemWidth(100.f);
			_float fDistortion = { 0.001f };
			ImGui::InputScalar("Distortion Intensity", ImGuiDataType_Float, &m_tSelected_Common_Desc.fDistortionPower, &fDistortion, NULL, "%.5f");
			ImGui::PopItemWidth();
		}
	}
}

void CEffect_Tool::Set_Start_Position()
{

	if (ImGui::CollapsingHeader("Start Pos"))
	{
		_float fPosStep = { 0.01f };

		ImGui::PushItemWidth(80.f);
		ImGui::InputScalar("##Start Position MinX", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartPosMin.x, &fPosStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Start Position MinY", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartPosMin.y, &fPosStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Start Position Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartPosMin.z, &fPosStep, NULL, "%.4f");

		ImGui::InputScalar("##Start Position MaxX", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartPosMax.x, &fPosStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Start Position MaxY", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartPosMax.y, &fPosStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Start Position Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartPosMax.z, &fPosStep, NULL, "%.4f");

		ImGui::PopItemWidth();
	}

}

void CEffect_Tool::Set_Start_Delay()
{
	_float fDelayStep = { 0.1f };
	ImGui::PushItemWidth(80.f);
	ImGui::InputScalar("##Start Delay Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.fStartDelayMin, &fDelayStep, NULL, "%.4f");
	ImGui::SameLine();
	ImGui::InputScalar("Start Delay Min Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.fStartDelayMax, &fDelayStep, NULL, "%.4f");
	ImGui::PopItemWidth();
}

void CEffect_Tool::Set_LifeTime()
{
	if (ImGui::CollapsingHeader("LIFE"))
	{
		_float fStep = { 0.05f };
		ImGui::PushItemWidth(80.f);
		ImGui::InputScalar("##Life Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.fLifeTimeMin, &fStep, NULL, "%.4f");

		ImGui::SameLine(0.f, 15.f);
		ImGui::InputScalar("Life MIN Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.fLifeTimeMax, &fStep, NULL, "%.4f");
		ImGui::PopItemWidth();
	}
}

void CEffect_Tool::Set_SpeedInfo()
{
	if (ImGui::CollapsingHeader("SPEED"))
	{
		_float fStep = { 0.05f };

		// Start Speed
		ImGui::PushItemWidth(80.f);
		ImGui::InputScalar("##Start Speed Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedMinMax.x, &fStep, NULL, "%.4f");

		ImGui::SameLine();
		ImGui::InputScalar("Start Speed Min Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedMinMax.y, &fStep, NULL, "%.4f");

		ImGui::InputScalar("Speed Over Play Time", ImGuiDataType_Float, &m_tSelected_Common_Desc.fSpeedOverLifeTime, &fStep, NULL, "%.4f");


		_float fLimitStep = { 1.f };

		ImGui::InputScalar("##Speed Direction Min X", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedDirMin.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Speed Direction Min Y", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedDirMin.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Speed Dirction Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedDirMin.z, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##Speed Direction Max X", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedDirMax.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Speed Direction Max Y", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedDirMax.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Speed Dirction Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.vSpeedDirMax.z, &fStep, NULL, "%.4f");

		ImGui::PopItemWidth();
	}
}

void CEffect_Tool::Set_Color()
{
	if (ImGui::CollapsingHeader("COLOR"))
	{
		_float fStep = { 0.01f };
		ImGui::PushItemWidth(90.f);
		ImGui::Checkbox("Proportional Alpha To Speed", &m_tSelected_Common_Desc.bProportionalAlphaToSpeed);

		if (true == m_tSelected_Common_Desc.bProportionalAlphaToSpeed)
			ImGui::InputScalar("Max Spped##Proportional", ImGuiDataType_Float, &m_tSelected_Common_Desc.fProportionalAlphaMaxSpeed, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##Start Color Min R", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMin.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Start Color Min G", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMin.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Start Color Min B", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMin.z, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Start Color Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMin.w, &fStep, NULL, "%.4f");


		ImGui::InputScalar("##Start Color Max R", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMax.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Start Color Max G", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMax.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Start Color Max B", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMax.z, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Start Color Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartColorMax.w, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##Add Color R", ImGuiDataType_Float, &m_tSelected_Common_Desc.vAddColor.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Add Color G", ImGuiDataType_Float, &m_tSelected_Common_Desc.vAddColor.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Add Color B", ImGuiDataType_Float, &m_tSelected_Common_Desc.vAddColor.z, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Add Color", ImGuiDataType_Float, &m_tSelected_Common_Desc.vAddColor.w, &fStep, NULL, "%.4f");

		ImGui::InputScalar("##Mutiple Color R", ImGuiDataType_Float, &m_tSelected_Common_Desc.vMultipleColor.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Mutiple Color G", ImGuiDataType_Float, &m_tSelected_Common_Desc.vMultipleColor.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Mutiple Color B", ImGuiDataType_Float, &m_tSelected_Common_Desc.vMultipleColor.z, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Mutiple Color", ImGuiDataType_Float, &m_tSelected_Common_Desc.vMultipleColor.w, &fStep, NULL, "%.4f");


		ImGui::InputScalar("##Clip R", ImGuiDataType_Float, &m_tSelected_Common_Desc.vClipColor.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Clip G", ImGuiDataType_Float, &m_tSelected_Common_Desc.vClipColor.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Clip B", ImGuiDataType_Float, &m_tSelected_Common_Desc.vClipColor.z, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Clip Color", ImGuiDataType_Float, &m_tSelected_Common_Desc.vClipColor.w, &fStep, NULL, "%.4f");

		ImGui::Checkbox("Enable Color Over Life", &m_tSelected_Common_Desc.bEnableColorOverLife);

		if (true == m_tSelected_Common_Desc.bEnableColorOverLife)
		{
			ImGui::InputScalar("##Color Over Life R", ImGuiDataType_Float, &m_tSelected_Common_Desc.vColorOverLife.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("##Color Over Life G", ImGuiDataType_Float, &m_tSelected_Common_Desc.vColorOverLife.y, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("##Color Over Life B", ImGuiDataType_Float, &m_tSelected_Common_Desc.vColorOverLife.z, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Color Over Life (RGBA)", ImGuiDataType_Float, &m_tSelected_Common_Desc.vColorOverLife.w, &fStep, NULL, "%.4f");

		}
	}
}

void CEffect_Tool::Set_Rotation()
{
	if (ImGui::CollapsingHeader("ROTATION"))
	{
		//m_vParticleStartRotMin
		_float fStep = { 1.f };
		ImGui::PushItemWidth(80.f);

		if (ImGui::InputScalar("##Start Rotation Min X", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartRotationMin.x, &fStep, NULL, "%.4f"))
			MINMAX(m_tSelected_Common_Desc.vStartRotationMin.x, -360.f, 360.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("##Start Rotation Min Y", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartRotationMin.y, &fStep, NULL, "%.4f"))
			MINMAX(m_tSelected_Common_Desc.vStartRotationMin.y, -360.f, 360.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("Start Rotation Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartRotationMin.z, &fStep, NULL, "%.4f"))
			MINMAX(m_tSelected_Common_Desc.vStartRotationMin.z, -360.f, 360.f);

		if (ImGui::InputScalar("##Start Rotation Max X", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartRotationMax.x, &fStep, NULL, "%.4f"))
			MINMAX(m_tSelected_Common_Desc.vStartRotationMax.x, -360.f, 360.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("##Start Rotation Max Y", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartRotationMax.y, &fStep, NULL, "%.4f"))
			MINMAX(m_tSelected_Common_Desc.vStartRotationMax.y, -360.f, 360.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("Start Rotation Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.vStartRotationMax.z, &fStep, NULL, "%.4f"))
			MINMAX(m_tSelected_Common_Desc.vStartRotationMax.z, -360.f, 360.f);

		if (CEffect::PARTICLE == m_pSelectedEffect->Get_EffetType())
		{
			ImGui::Checkbox("Local", &m_tSelected_Common_Desc.bRotationLocal);
			ImGui::SameLine();
			//vRotSpeed
			ImGui::InputScalar("Rotation Speed##Particle", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fRotSpeed, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Rotation Aixs X##P", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vRotAxis.x, &fStep, NULL, "%.4f");

			ImGui::SameLine();
			ImGui::InputScalar("##Rotation Axi Y##p", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vRotAxis.y, &fStep, NULL, "%.4f");

			ImGui::SameLine();
			ImGui::InputScalar("Rotation Axis", ImGuiDataType_Float, &m_tSelected_Particle_Desc.vRotAxis.z, &fStep, NULL, "%.4f");

		}

	}
}

void CEffect_Tool::Set_Power()
{
	if (ImGui::CollapsingHeader("POWER"))
	{
		_float fStep = { 1.f };
		ImGui::PushItemWidth(80.f);

		ImGui::InputScalar("##Power Min X", ImGuiDataType_Float, &m_tSelected_Common_Desc.vPowerDirMin.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Power Min Y", ImGuiDataType_Float, &m_tSelected_Common_Desc.vPowerDirMin.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Power Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.vPowerDirMin.z, &fStep, NULL, "%.4f");


		ImGui::InputScalar("##Power Max X", ImGuiDataType_Float, &m_tSelected_Common_Desc.vPowerDirMax.x, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("##Power Max Y", ImGuiDataType_Float, &m_tSelected_Common_Desc.vPowerDirMax.y, &fStep, NULL, "%.4f");
		ImGui::SameLine();
		ImGui::InputScalar("Power Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.vPowerDirMax.z, &fStep, NULL, "%.4f");

		ImGui::PopItemWidth();
	}
}

void CEffect_Tool::Set_Textures()
{
	if (false == ImGui::CollapsingHeader("TEXTURE"))
		return;

	if (ImGui::Checkbox("Base", &m_bUsingTexture[TEX_BASE]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_BASE);
	ImGui::SameLine();
	if (ImGui::Checkbox("Normal", &m_bUsingTexture[TEX_NORM]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_NORM);
	ImGui::SameLine();
	if (ImGui::Checkbox("AlphaMask", &m_bUsingTexture[TEX_ALPHAMASK]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_ALPHAMASK);
	ImGui::SameLine();
	if (ImGui::Checkbox("Color ScaleUV", &m_bUsingTexture[TEX_COLOR_SCALE_UV]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_COLOR_SCALE_UV);
	ImGui::SameLine();
	if (ImGui::Checkbox("Noise", &m_bUsingTexture[TEX_NOISE]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_NOISE);

	if (ImGui::Checkbox("ColorScale", &m_bUsingTexture[TEX_COLORSCALE]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_COLORSCALE);
	ImGui::SameLine();
	if (ImGui::Checkbox("Distortion##TEX", &m_bUsingTexture[TEX_DISTORTION]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_DISTORTION);
	ImGui::SameLine();
	if (ImGui::Checkbox("Dissolve", &m_bUsingTexture[TEX_DISSOLVE]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_DISSOLVE);
	ImGui::SameLine();
	if (ImGui::Checkbox("BaseNoiseUV", &m_bUsingTexture[TEX_BASE_NOISE_UV]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_BASE_NOISE_UV);
	ImGui::SameLine();
	if (ImGui::Checkbox("AlphaMaskNoiseUV", &m_bUsingTexture[TEX_ALPHAMASK_NOISE_UV]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_ALPHAMASK_NOISE_UV);

	if (ImGui::Checkbox("UV Slice Clip", &m_bUsingTexture[TEX_UV_SLICE_CLIP]))
		m_pSelectedEffect->Remove_Texture(CEffect::TEX_UV_SLICE_CLIP);

	if (true == m_bUsingTexture[TEX_BASE])
	{
		ImGui::Text("Base Texure     : ");

		strcpy_s(m_szTextureNames[TEX_BASE], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_BASE));
		if (!strcmp("", m_szTextureNames[TEX_BASE]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select Base"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_BASE;

		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_BASE]);
			ImGui::SameLine();
			if (ImGui::Button("Change Base"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_BASE;
			ImGui::SameLine();
			if (ImGui::Button("Remove Base"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_BASE);
		}

		if (TEX_BASE == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_BASE, " - Base");


		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

		if (ImGui::CollapsingHeader("Base Info"))
		{

			if (CEffect::TRAIL != m_pSelectedEffect->Get_EffetType())
			{

				ImGui::Checkbox("Radial Mapping##Base", &m_bBase_Radial);

				if (m_bBase_Radial)
				{
					if (0 == (m_tSelected_Common_UVDesc.iRadialMappingFlag & BASERADIAL))
						m_tSelected_Common_UVDesc.iRadialMappingFlag |= BASERADIAL;
				}
				else
				{
					if (m_tSelected_Common_UVDesc.iRadialMappingFlag & BASERADIAL)
						m_tSelected_Common_UVDesc.iRadialMappingFlag ^= BASERADIAL;
				}

				if (m_tSelected_Common_UVDesc.iRadialMappingFlag & BASERADIAL)
				{
					ImGui::RadioButton("U Radius", &m_iBaseNoiseUV, 0);
					ImGui::SameLine();
					ImGui::RadioButton("V Radius", &m_iBaseNoiseUV, 1);

					if (0 == m_iBaseNoiseUV)
						m_tSelected_Common_UVDesc.vBaseNoiseRadiusFlag = { 1.f,0.f };
					else
						m_tSelected_Common_UVDesc.vBaseNoiseRadiusFlag = { 0.f,1.f };
				}

				ImGui::Checkbox("Sprite Base Texture", &m_tSelected_Common_Desc.bBaseSprite);

				if (true == m_tSelected_Common_Desc.bBaseSprite)
				{

					_int iStep = 1;
					ImGui::PushItemWidth(80.f);
					ImGui::InputScalar("##Base Sprite Row", ImGuiDataType_S32, &m_tSelected_Common_Desc.iBaseSpriteMaxRow, &iStep, NULL, "%d");
					ImGui::SameLine();
					ImGui::InputScalar("Max Row / Col", ImGuiDataType_S32, &m_tSelected_Common_Desc.iBaseSpriteMaxCol, &iStep, NULL, "%d");

					ImGui::Checkbox("Sprite Change Over Time", &m_tSelected_Common_Desc.ChangeBaseSprite);

					_float fStep = 0.05f;
					if (true == m_tSelected_Common_Desc.ChangeBaseSprite)
					{
						ImGui::InputScalar("Sprite Change Time", ImGuiDataType_Float, &m_tSelected_Common_Desc.fBaseSpriteChangeTime, &fStep, NULL, "%.3f");

					}

					if (CEffect::PARTICLE == m_pSelectedEffect->Get_EffetType())
					{
						ImGui::InputScalar("Change Time Tolerance", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fSpriteChangeTolerance, &fStep, NULL, "%.3f");
					}
					ImGui::PopItemWidth();
				}
			}

			// UV Offset
			ImGui::SeparatorText("UV##Base");

			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##Base UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartBaseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Base UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartBaseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Base UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vBaseUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Base UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vBaseUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Base UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinBaseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Base UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinBaseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Base UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxBaseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Base UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxBaseUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##Base", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iBaseSampler, &iStep, NULL, "%d");

			ImGui::PopItemWidth();
		}



		ImGui::End();

	}
	if (true == m_bUsingTexture[TEX_NORM])
	{
		ImGui::Text("Normal Texure   : ");

		strcpy_s(m_szTextureNames[TEX_NORM], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_NORM));
		if (!strcmp("", m_szTextureNames[TEX_NORM]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select Normal"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_NORM;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_NORM]);
			ImGui::SameLine();
			if (ImGui::Button("Change Normal"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_NORM;
			ImGui::SameLine();
			if (ImGui::Button("Remove Normal"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_NORM);
		}

		if (TEX_NORM == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_NORM, " - Normal");
	}
	if (true == m_bUsingTexture[TEX_ALPHAMASK])
	{
		ImGui::Text("AlphaMask Texure     : ");

		strcpy_s(m_szTextureNames[TEX_ALPHAMASK], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_ALPHAMASK));
		if (!strcmp("", m_szTextureNames[TEX_ALPHAMASK]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select AlphaMask"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_ALPHAMASK;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_ALPHAMASK]);
			ImGui::SameLine();
			if (ImGui::Button("Change AlphaMask"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_ALPHAMASK;
			ImGui::SameLine();
			if (ImGui::Button("Remove AlphaMask"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_ALPHAMASK);
		}
		if (TEX_ALPHAMASK == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_ALPHAMASK, " - AlphaMask");


		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		// Sprite

		if (ImGui::CollapsingHeader("Alpha Mask Info"))
		{

			// RADIAL MAPPING
			ImGui::Checkbox("Radial Mapping##Alpha", &m_bAlpha_Radial);

			if (m_bAlpha_Radial)
			{
				if (0 == (m_tSelected_Common_UVDesc.iRadialMappingFlag & ALPHARADIAL))
					m_tSelected_Common_UVDesc.iRadialMappingFlag |= ALPHARADIAL;
			}
			else
			{
				if (m_tSelected_Common_UVDesc.iRadialMappingFlag & ALPHARADIAL)
					m_tSelected_Common_UVDesc.iRadialMappingFlag ^= ALPHARADIAL;
			}

			if (m_tSelected_Common_UVDesc.iRadialMappingFlag & ALPHARADIAL)
			{
				ImGui::RadioButton("U Radius", &m_iAlphaNoiseUV, 0);
				ImGui::SameLine();
				ImGui::RadioButton("V Radius", &m_iAlphaNoiseUV, 1);

				if (0 == m_iAlphaNoiseUV)
					m_tSelected_Common_UVDesc.vAlphaNoiseRadiusFlag = { 1.f,0.f };
				else
					m_tSelected_Common_UVDesc.vAlphaNoiseRadiusFlag = { 0.f,1.f };
			}


			// SPRITE
			if (CEffect::TRAIL != m_pSelectedEffect->Get_EffetType())
			{
				ImGui::SeparatorText("Sprite##AlphaMask");
				ImGui::Checkbox("Sprite AlphaMask Texture", &m_tSelected_Common_Desc.bMaskSprite);

				if (true == m_tSelected_Common_Desc.bMaskSprite)
				{
					auto Pair = m_Textures.find(TEXT("Mask"));

					_int iStep = 1;
					ImGui::PushItemWidth(80.f);
					ImGui::InputScalar("##Alpha Sprite Row", ImGuiDataType_S32, &m_tSelected_Common_Desc.iMaskSpriteMaxRow, &iStep, NULL, "%d");
					ImGui::SameLine();
					ImGui::InputScalar("Max Row / Col##Alpha", ImGuiDataType_S32, &m_tSelected_Common_Desc.iMaskSpriteMaxCol, &iStep, NULL, "%d");
					ImGui::Checkbox("Sprite Change Over Time##Mask", &m_tSelected_Common_Desc.ChangeMaskSprite);

					_float fStep = 0.05f;
					if (true == m_tSelected_Common_Desc.ChangeMaskSprite)
					{
						ImGui::InputScalar("Sprite Change Time##Mask", ImGuiDataType_Float, &m_tSelected_Common_Desc.fMaskSpriteChangeTime, &fStep, NULL, "%.3f");

					}

					if (CEffect::PARTICLE == m_pSelectedEffect->Get_EffetType())
					{
						ImGui::InputScalar("Change Time Tolerance", ImGuiDataType_Float, &m_tSelected_Particle_Desc.fSpriteChangeTolerance, &fStep, NULL, "%.3f");
					}
					ImGui::PopItemWidth();
				}
			}


			// UV Offset
			ImGui::SeparatorText("Mask UV##AlPha");

			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##AlphaMask UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartAlphaMaskUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();

			ImGui::InputScalar("AlphaMask UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartAlphaMaskUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##AlphaMask UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vAlphaMaskUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("AlphaMask UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vAlphaMaskUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##AlphaMask UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinAlphaMaskUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("AlphaMask UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinAlphaMaskUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##AlphaMask UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxAlphaMaskUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("AlphaMask UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxAlphaMaskUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##AlphaMask", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iAlphaMaskSampler, &iStep, NULL, "%d");


		}
		ImGui::End();
	}
	if (true == m_bUsingTexture[TEX_COLOR_SCALE_UV])
	{
		ImGui::Text("ColorScaleUV Texure     : ");

		strcpy_s(m_szTextureNames[TEX_COLOR_SCALE_UV], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_COLOR_SCALE_UV));
		if (!strcmp("", m_szTextureNames[TEX_COLOR_SCALE_UV]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select ColorScaleUV"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_COLOR_SCALE_UV;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_COLOR_SCALE_UV]);
			ImGui::SameLine();
			if (ImGui::Button("Change ColorScaleUV"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_COLOR_SCALE_UV;
			ImGui::SameLine();
			if (ImGui::Button("Remove ColorScaleUV"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_COLOR_SCALE_UV);
		}
		if (TEX_COLOR_SCALE_UV == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_COLOR_SCALE_UV, " - ColorScaleUV");


		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

		// UV Offset
		if (ImGui::CollapsingHeader("ColorScaleUV Info"))
		{

			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##ColorScale UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartColorScaleUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("ColorScale UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartColorScaleUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##ColorScale UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vColorScaleUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("ColorScale UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vColorScaleUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##ColorScale UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinColorScaleUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("ColorScale UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinColorScaleUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##ColorScale UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxColorScaleUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("ColorScale UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxColorScaleUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##ColorScale", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iColorScaleUVSampler, &iStep, NULL, "%d");
			ImGui::PopItemWidth();
		}

		ImGui::End();
	}
	if (true == m_bUsingTexture[TEX_COLORSCALE])
	{
		ImGui::Text("ColorScale Texure     : ");

		strcpy_s(m_szTextureNames[TEX_COLORSCALE], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_COLORSCALE));
		if (!strcmp("", m_szTextureNames[TEX_COLORSCALE]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select ColorScale"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_COLORSCALE;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_COLORSCALE]);
			ImGui::SameLine();
			if (ImGui::Button("Change ColorScale"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_COLORSCALE;
			ImGui::SameLine();
			if (ImGui::Button("Remove ColorScale"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_COLORSCALE);
		}
		if (TEX_COLORSCALE == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_COLORSCALE, " - ColorScale");
	}
	if (true == m_bUsingTexture[TEX_NOISE])
	{
		ImGui::Text("Noise Texure    : ");

		strcpy_s(m_szTextureNames[TEX_NOISE], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_NOISE));
		if (!strcmp("", m_szTextureNames[TEX_NOISE]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select Noise"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_NOISE;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_NOISE]);
			ImGui::SameLine();
			if (ImGui::Button("Change Noise"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_NOISE;
			ImGui::SameLine();
			if (ImGui::Button("Remove Noise"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_NOISE);
		}
		if (TEX_NOISE == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_NOISE, " - Noise");

		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		// UV Offset

		if (ImGui::CollapsingHeader("Noise Info"))
		{
			ImGui::Checkbox("Also Multiply Alpha", &m_tSelected_Common_Desc.bNoiseMultiplyAlpha);

			ImGui::Checkbox("Radial Mapping##Noise", &m_bNoise_Radial);

			if (m_bNoise_Radial)
			{
				if (0 == (m_tSelected_Common_UVDesc.iRadialMappingFlag & NOISERADIAL))
					m_tSelected_Common_UVDesc.iRadialMappingFlag |= NOISERADIAL;
			}
			else
			{
				if (m_tSelected_Common_UVDesc.iRadialMappingFlag & NOISERADIAL)
					m_tSelected_Common_UVDesc.iRadialMappingFlag ^= NOISERADIAL;
			}

			if (m_tSelected_Common_UVDesc.iRadialMappingFlag & NOISERADIAL)
			{
				ImGui::RadioButton("U Radius##Noise", &m_iNoiseUV_ID, 0);
				ImGui::SameLine();
				ImGui::RadioButton("V Radius##Noise", &m_iNoiseUV_ID, 1);

				if (0 == m_iNoiseUV_ID)
					m_tSelected_Common_UVDesc.vNoise_RadiusFlag = { 1.f,0.f };
				else
					m_tSelected_Common_UVDesc.vNoise_RadiusFlag = { 0.f,1.f };
			}

			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##Noise UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Noise UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartNoiseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Noise UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vNoiseUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Noise UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vNoiseUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Noise UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Noise UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinNoiseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Noise UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Noise UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxNoiseUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##Noise", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iNoiseSampler, &iStep, NULL, "%d");

		}
		ImGui::End();
	}
	if (true == m_bUsingTexture[TEX_DISTORTION])
	{
		ImGui::Text("Distortion Texure : ");

		strcpy_s(m_szTextureNames[TEX_DISTORTION], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_DISTORTION));
		if (!strcmp("", m_szTextureNames[TEX_DISTORTION]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select Distortion"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_DISTORTION;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_DISTORTION]);
			ImGui::SameLine();
			if (ImGui::Button("Change Distortion"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_DISTORTION;
			ImGui::SameLine();
			if (ImGui::Button("Remove Distortion"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_DISTORTION);
		}
		if (TEX_DISTORTION == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_DISTORTION, " - Distortion");

		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		// UV Offset

		if (ImGui::CollapsingHeader("Distortion Info##TEX"))
		{

			ImGui::Checkbox("Radial Mapping##Distortion", &m_bDistortion_Radial);

			if (m_bDistortion_Radial)
			{
				if (0 == (m_tSelected_Common_UVDesc.iRadialMappingFlag & DISTORTIONRADIAL))
					m_tSelected_Common_UVDesc.iRadialMappingFlag |= DISTORTIONRADIAL;
			}
			else
			{
				if (m_tSelected_Common_UVDesc.iRadialMappingFlag & DISTORTIONRADIAL)
					m_tSelected_Common_UVDesc.iRadialMappingFlag ^= DISTORTIONRADIAL;
			}

			if (m_tSelected_Common_UVDesc.iRadialMappingFlag & DISTORTIONRADIAL)
			{
				ImGui::RadioButton("U Radius##Distortion", &m_iDistortionUV_ID, 0);
				ImGui::SameLine();
				ImGui::RadioButton("V Radius##Distortion", &m_iDistortionUV_ID, 1);

				if (0 == m_iDistortionUV_ID)
					m_tSelected_Common_UVDesc.vDistortion_RadiusFlag = { 1.f,0.f };
				else
					m_tSelected_Common_UVDesc.vDistortion_RadiusFlag = { 0.f,1.f };
			}
			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##Distortion UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartDistortionUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Distortion UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartDistortionUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Distortion UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vDistortionUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Distortion UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vDistortionUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Distortion UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinDistortionUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Distortion UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinDistortionUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Distortion UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxDistortionUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Distortion UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxDistortionUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##Distortion", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iDistortionSampler, &iStep, NULL, "%d");
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}
	if (true == m_bUsingTexture[TEX_DISSOLVE])
	{
		ImGui::Text("Dissolve Texure : ");

		strcpy_s(m_szTextureNames[TEX_DISSOLVE], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_DISSOLVE));
		if (!strcmp("", m_szTextureNames[TEX_DISSOLVE]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select Dissolve"))
				eSelecting_Tex_Type = TEX_DISSOLVE;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_DISSOLVE]);
			ImGui::SameLine();
			if (ImGui::Button("Change Dissolve"))
				eSelecting_Tex_Type = TEX_DISSOLVE;
			ImGui::SameLine();
			if (ImGui::Button("Remove Dissolve"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_DISSOLVE);
		}
		if (TEX_DISSOLVE == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_DISSOLVE, " - Dissolve");

		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		// UV Offset

		if (ImGui::CollapsingHeader("Dissolve Info"))
		{
			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##Dissolve UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartDissolveUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Dissolve UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartDissolveUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Dissolve UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vDissolveUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Dissolve UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vDissolveUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Dissolve UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinDissolveUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Dissolve UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinDissolveUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Dissolve UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxDissolveUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Dissolve UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxDissolveUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##Dissolve", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iDissolveSampler, &iStep, NULL, "%d");
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}
	if (true == m_bUsingTexture[TEX_BASE_NOISE_UV])
	{
		ImGui::Text("Base NoiseUV Texture : ");

		strcpy_s(m_szTextureNames[TEX_BASE_NOISE_UV], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_BASE_NOISE_UV));
		if (!strcmp("", m_szTextureNames[TEX_BASE_NOISE_UV]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select NoiseUV"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_BASE_NOISE_UV;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_BASE_NOISE_UV]);
			ImGui::SameLine();
			if (ImGui::Button("Change BaseNoiseUV"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_BASE_NOISE_UV;
			ImGui::SameLine();
			if (ImGui::Button("Remove BaseNoiseUV"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_BASE_NOISE_UV);
		}
		if (TEX_BASE_NOISE_UV == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_BASE_NOISE_UV, " - BaseNoiseUV");

		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		// UV Offset

		if (ImGui::CollapsingHeader("BaseNoise Info"))
		{

			ImGui::Checkbox("-1 ~ 1 Normalize##Base", &m_bBase_Normalized_Noise);
			ImGui::Checkbox("Noise Offset##BAse", &m_bBase_Noise_Offset);

			m_tSelected_Common_UVDesc.vBaseNoiseUVInfoFlag.x = true == m_bBase_Normalized_Noise ? 1.f : 0.f;
			m_tSelected_Common_UVDesc.vBaseNoiseUVInfoFlag.y = true == m_bBase_Noise_Offset ? 1.f : 0.f;

			if (m_bBase_Noise_Offset)
			{
				ImGui::RadioButton("Offset_Add##Base", &m_iBase_Noise_OffsetAdd, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Offset_Sub##Base", &m_iBase_Noise_OffsetAdd, 1);

				m_tSelected_Common_UVDesc.vBaseNoiseUVInfoFlag.z = 0 == m_iBase_Noise_OffsetAdd ? 1.f : 0.f;
			}


			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##BaseNoise UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartBaseNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("BaseNoise UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartBaseNoiseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##BaseNoise UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vBaseNoiseUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("BaseNoise UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vBaseNoiseUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##BaseNoise UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinBaseNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("BaseNoise UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinBaseNoiseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##BaseNoise UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxBaseNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("BaseNoise UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxBaseNoiseUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##BaseNoise", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iBaseNoiseSampler, &iStep, NULL, "%d");
			ImGui::PopItemWidth();


		}
		ImGui::End();
	}
	if (true == m_bUsingTexture[TEX_ALPHAMASK_NOISE_UV])
	{
		ImGui::Text("AlphaNoise Texure : ");

		strcpy_s(m_szTextureNames[TEX_ALPHAMASK_NOISE_UV], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_ALPHAMASK_NOISE_UV));
		if (!strcmp("", m_szTextureNames[TEX_ALPHAMASK_NOISE_UV]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select AlphaNoise"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_ALPHAMASK_NOISE_UV;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_ALPHAMASK_NOISE_UV]);
			ImGui::SameLine();
			if (ImGui::Button("Change AlphaNoise"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_ALPHAMASK_NOISE_UV;
			ImGui::SameLine();
			if (ImGui::Button("Remove AlphaNoise"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_ALPHAMASK_NOISE_UV);
		}
		if (TEX_ALPHAMASK_NOISE_UV == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_ALPHAMASK_NOISE_UV, " - AlphaNoise");

		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		// UV Offset

		if (ImGui::CollapsingHeader("AlphaNoise Info"))
		{

			ImGui::Checkbox("-1 ~ 1 Normalize##Alpha", &m_bAlpha_Normalized_Noise);
			ImGui::Checkbox("Noise Offset##Alpha", &m_bAlpha_Noise_Offset);

			m_tSelected_Common_UVDesc.vAlphaNoiseUVInfoFlag.x = true == m_bAlpha_Normalized_Noise ? 1.f : 0.f;
			m_tSelected_Common_UVDesc.vAlphaNoiseUVInfoFlag.y = true == m_bAlpha_Noise_Offset ? 1.f : 0.f;

			if (m_bAlpha_Noise_Offset)
			{
				ImGui::RadioButton("Offset_Add##Alpha", &m_iAlpha_Noise_OffsetAdd, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Offset_Sub##Alpha", &m_iAlpha_Noise_OffsetAdd, 1);

				m_tSelected_Common_UVDesc.vAlphaNoiseUVInfoFlag.z = 0 == m_iAlpha_Noise_OffsetAdd ? 1.f : 0.f;
			}

			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##AlphaNoise UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartAlphaMaskNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("AlphaNoise UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartAlphaMaskNoiseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##AlphaNoise UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vAlphaMaskNoiseUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("AlphaNoise UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vAlphaMaskNoiseUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##AlphaNoise UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinAlphaMaskNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("AlphaNoise UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinAlphaMaskNoiseUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##AlphaNoise UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxAlphaMaskNoiseUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("AlphaNoise UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxAlphaMaskNoiseUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##AlphaNoise", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iAlphaMaskNoiseSampler, &iStep, NULL, "%d");
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}
	if (true == m_bUsingTexture[TEX_UV_SLICE_CLIP])
	{
		ImGui::Text("UVSliceClip Texure : ");

		strcpy_s(m_szTextureNames[TEX_UV_SLICE_CLIP], MAX_PATH, m_pSelectedEffect->Get_TextureName(CEffect::TEX_UV_SLICE_CLIP));
		if (!strcmp("", m_szTextureNames[TEX_UV_SLICE_CLIP]))
		{
			ImGui::SameLine();
			if (ImGui::Button("Select UVSliceClip"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_UV_SLICE_CLIP;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(m_szTextureNames[TEX_UV_SLICE_CLIP]);
			ImGui::SameLine();
			if (ImGui::Button("Change UVSliceClip"))
				eSelecting_Tex_Type = TEX_TYPE::TEX_UV_SLICE_CLIP;
			ImGui::SameLine();
			if (ImGui::Button("Remove UVSliceClip"))
				m_pSelectedEffect->Remove_Texture(CEffect::TEX_UV_SLICE_CLIP);
		}
		if (TEX_UV_SLICE_CLIP == eSelecting_Tex_Type)
			SelectImage(TEX_TYPE::TEX_UV_SLICE_CLIP, " - UVSliceClip");


		ImGui::Begin("Texture Info", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

		if (ImGui::CollapsingHeader("UV Slice Clip Info"))
		{
			_float fStep = { 0.01f };
			ImGui::PushItemWidth(80.f);
			ImGui::InputScalar("##UVSliceClip UV OffsetX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartUVSliceClipUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("UVSliceClip UV Offset", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vStartUVSliceClipUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##UVSliceClip UV SpeedX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vUVSliceClipUVSpeed.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("UVSliceClip UV Speed", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vUVSliceClipUVSpeed.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##UVSliceClip UV Offset MinX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinUVSliceClipUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("UVSliceClip UV Offset Min", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMinUVSliceClipUVOffset.y, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##UVSliceClip UV Offset MaxX", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxUVSliceClipUVOffset.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("UVSliceClip UV Offset Max", ImGuiDataType_Float, &m_tSelected_Common_UVDesc.vMaxUVSliceClipUVOffset.y, &fStep, NULL, "%.4f");

			_int iStep = 1;
			ImGui::InputScalar("Sampler Number##UVSliceClip", ImGuiDataType_S32, &m_tSelected_Common_UVDesc.iUVSliceClipSampler, &iStep, NULL, "%d");

			ImGui::InputScalar("Clip Factor##UVSliceClip", ImGuiDataType_Float, &m_tSelected_Common_Desc.fClipFactor, &fStep, NULL, "%.4f");


			ImGui::Checkbox("Use Greater Than-U", &m_tSelected_Common_Desc.bUseGreaterThan_U);
			ImGui::SameLine();
			ImGui::Checkbox("Use Greater Than-V", &m_tSelected_Common_Desc.bUseGreaterThan_V);

			if (true == m_tSelected_Common_Desc.bUseGreaterThan_U)
				ImGui::InputScalar("U Cmp Value", ImGuiDataType_Float, &m_tSelected_Common_Desc.fGreaterCmpVal_U, &fStep, NULL, "%.4f");

			ImGui::SameLine();

			if (true == m_tSelected_Common_Desc.bUseGreaterThan_V)
				ImGui::InputScalar("V Cmp Value", ImGuiDataType_Float, &m_tSelected_Common_Desc.fGreaterCmpVal_V, &fStep, NULL, "%.4f");

			ImGui::Dummy(ImVec2(0.f, 20.f));

			ImGui::Checkbox("Use Less Than-U", &m_tSelected_Common_Desc.bUseLessThan_U);
			ImGui::SameLine();
			ImGui::Checkbox("Use Less Than-V", &m_tSelected_Common_Desc.bUseLessThan_V);

			if (true == m_tSelected_Common_Desc.bUseLessThan_U)
				ImGui::InputScalar("U Cmp Value##Less", ImGuiDataType_Float, &m_tSelected_Common_Desc.fLessCmpVal_U, &fStep, NULL, "%.4f");

			ImGui::SameLine();

			if (true == m_tSelected_Common_Desc.bUseLessThan_V)
				ImGui::InputScalar("V Cmp Value##Less", ImGuiDataType_Float, &m_tSelected_Common_Desc.fLessCmpVal_V, &fStep, NULL, "%.4f");

			ImGui::PopItemWidth();

		}
		ImGui::End();
	}
}

void CEffect_Tool::Set_AttachDesc()
{
	if (false == ImGui::CollapsingHeader("ATTACH"))
		return;

	//if (nullptr == m_pSelectedObj)
	//{
	//	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Select OBj!");
	//	return;
	//}

	ImGui::Text("Current Spawn Bone Name : ");
	ImGui::SameLine();
	ImGui::Text(m_tSelected_Effect_AttachDesc.szBoneName);

	ImGui::InputText("##Bone Name", m_szChangeBoneName, MAX_PATH);
	if (ImGui::Button("Change Bone"))
	{
		if (strcmp(m_szChangeBoneName, ""))
		{
			Safe_Release(m_tSelected_Effect_AttachDesc.pBone);
			m_tSelected_Effect_AttachDesc.pBone = m_pSelectedObj->Get_Model()->Get_Bone(m_szChangeBoneName);
			if (nullptr != m_tSelected_Effect_AttachDesc.pBone)
			{
				Safe_AddRef(m_tSelected_Effect_AttachDesc.pBone);
				strcpy_s(m_tSelected_Effect_AttachDesc.szBoneName, MAX_PATH, m_szChangeBoneName);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Bone"))
	{
		Safe_Release(m_tSelected_Effect_AttachDesc.pBone);
		strcpy_s(m_tSelected_Effect_AttachDesc.szBoneName, MAX_PATH, "");
	}

	Set_TF_Type();
	ImGui::Checkbox("Fllow Only Position", &m_tSelected_Effect_AttachDesc.bOnlyPosition);

	ImGui::SameLine();

	ImGui::Checkbox("World Offset", &m_tSelected_Effect_AttachDesc.bWorldOffset);

	ImGui::SameLine();
	ImGui::Checkbox("Discard Rotation", &m_tSelected_Mesh_Desc.bDiscardRotation);


	_float fOffsetStep = { 0.1f };
	ImGui::PushItemWidth(80.f);
	if (ImGui::InputScalar("##Offset X", ImGuiDataType_Float, &m_tSelected_Effect_AttachDesc.vOffSet.x, &fOffsetStep, NULL, "%.4f"))
		MINMAX(m_tSelected_Effect_AttachDesc.vOffSet.x, -30.f, 30.f);
	ImGui::SameLine();
	if (ImGui::InputScalar("##Offset Y", ImGuiDataType_Float, &m_tSelected_Effect_AttachDesc.vOffSet.y, &fOffsetStep, NULL, "%.4f"))
		MINMAX(m_tSelected_Effect_AttachDesc.vOffSet.y, -30.f, 30.f);
	ImGui::SameLine();
	if (ImGui::InputScalar("Offset", ImGuiDataType_Float, &m_tSelected_Effect_AttachDesc.vOffSet.z, &fOffsetStep, NULL, "%.4f"))
		MINMAX(m_tSelected_Effect_AttachDesc.vOffSet.z, -30.f, 30.f);
	ImGui::PopItemWidth();


	if (CEffect::TRAIL == m_pSelectedEffect->Get_EffetType())
	{
		ImGui::PushItemWidth(80.f);

		if (ImGui::InputScalar("##Trail Start Offset X", ImGuiDataType_Float, &m_tSelecete_Trail_Desc.vStartPos.x, &fOffsetStep, NULL, "%.4f"))
			MINMAX(m_tSelecete_Trail_Desc.vStartPos.x, -30.f, 30.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("##Trail Start Offset Y", ImGuiDataType_Float, &m_tSelecete_Trail_Desc.vStartPos.y, &fOffsetStep, NULL, "%.4f"))
			MINMAX(m_tSelecete_Trail_Desc.vStartPos.y, -30.f, 30.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("Trail Start Offset", ImGuiDataType_Float, &m_tSelecete_Trail_Desc.vStartPos.z, &fOffsetStep, NULL, "%.4f"))
			MINMAX(m_tSelecete_Trail_Desc.vStartPos.z, -30.f, 30.f);


		if (ImGui::InputScalar("##Trail End Offset X", ImGuiDataType_Float, &m_tSelecete_Trail_Desc.vEndPos.x, &fOffsetStep, NULL, "%.4f"))
			MINMAX(m_tSelecete_Trail_Desc.vEndPos.x, -30.f, 30.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("##Trail End Offset Y", ImGuiDataType_Float, &m_tSelecete_Trail_Desc.vEndPos.y, &fOffsetStep, NULL, "%.4f"))
			MINMAX(m_tSelecete_Trail_Desc.vEndPos.y, -30.f, 30.f);
		ImGui::SameLine();
		if (ImGui::InputScalar("Trail End Offset", ImGuiDataType_Float, &m_tSelecete_Trail_Desc.vEndPos.z, &fOffsetStep, NULL, "%.4f"))
			MINMAX(m_tSelecete_Trail_Desc.vEndPos.z, -30.f, 30.f);

		ImGui::PopItemWidth();
	}
}

void CEffect_Tool::Set_Dissolve()
{

	if (false == m_bUsingTexture[TEX_DISSOLVE])
	{
		m_tSelected_Common_Desc.fDissolveSpeed = 0.f;
		return;
	}
	if (ImGui::CollapsingHeader("DISSOLVE#G"))
	{


		ImGui::Checkbox("Dissolve Reverse", &m_tSelected_Common_Desc.bDissolveReverse);

		ImGui::PushItemWidth(80.f);

		_float fStep = 0.01f;
		ImGui::InputScalar("Dissolve StartTime", ImGuiDataType_Float, &m_tSelected_Common_Desc.fDissolveStartTime, &fStep, NULL, "%.3f");
		ImGui::InputScalar("Dissolve Speed", ImGuiDataType_Float, &m_tSelected_Common_Desc.fDissolveSpeed, &fStep, NULL, "%.3f");



		ImGui::Checkbox("Dissolve Stop", &m_bDissolve_Stop);
		if (true == m_bDissolve_Stop)
		{
			if (0 == (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_STOP))
				m_tSelected_Common_Desc.iDissolveFlag |= DISSOLVE_STOP;

			ImGui::InputScalar("Dissolve Stop Amount", ImGuiDataType_Float, &m_tSelected_Common_Desc.fStopDissolveAmount, &fStep, NULL, "%.4f");

		}
		else
		{
			if (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_STOP)
				m_tSelected_Common_Desc.iDissolveFlag ^= DISSOLVE_STOP;
		}



		ImGui::Checkbox("Dissolve Loop", &m_bDissolve_Loop);
		if (true == m_bDissolve_Loop)
		{
			if (0 == (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_LOOP))
				m_tSelected_Common_Desc.iDissolveFlag |= DISSOLVE_LOOP;

			ImGui::InputScalar("##Dissolve Loop Min", ImGuiDataType_Float, &m_tSelected_Common_Desc.fDissolveLoopMin, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Dissolve Loop Min Max", ImGuiDataType_Float, &m_tSelected_Common_Desc.fDissolveLoopMax, &fStep, NULL, "%.4f");

		}
		else
		{
			if (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_LOOP)
				m_tSelected_Common_Desc.iDissolveFlag ^= DISSOLVE_LOOP;
		}



		ImGui::Checkbox("Dissolve Edge", &m_bDissolve_Edge);
		if (true == m_bDissolve_Edge)
		{
			if (0 == (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_EDGE))
				m_tSelected_Common_Desc.iDissolveFlag |= DISSOLVE_EDGE;

			ImGui::InputScalar("Edge Width", ImGuiDataType_Float, &m_tSelected_Common_Desc.fDissolveEdgeWidth, &fStep, NULL, "%.4f");

			ImGui::InputScalar("##Dissolve Edge Color x", ImGuiDataType_Float, &m_tSelected_Common_Desc.vEdgeColor.x, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("##Dissolve Edge Color y", ImGuiDataType_Float, &m_tSelected_Common_Desc.vEdgeColor.y, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("##Dissolve Edge Color z", ImGuiDataType_Float, &m_tSelected_Common_Desc.vEdgeColor.z, &fStep, NULL, "%.4f");
			ImGui::SameLine();
			ImGui::InputScalar("Dissolve Edge Color", ImGuiDataType_Float, &m_tSelected_Common_Desc.vEdgeColor.w, &fStep, NULL, "%.4f");

		}
		else
		{
			if (m_tSelected_Common_Desc.iDissolveFlag & DISSOLVE_EDGE)
				m_tSelected_Common_Desc.iDissolveFlag ^= DISSOLVE_EDGE;
		}


		ImGui::PopItemWidth();



	}
}


CEffect_Tool* CEffect_Tool::Create()
{
	CEffect_Tool* pInstance = new CEffect_Tool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Tool::Free()
{
	__super::Free();


	for (auto& pEffect : m_ParticleEffects)
		Safe_Release(pEffect);
	for (auto& pEffect : m_MeshEffects)
		Safe_Release(pEffect);
	for (auto& pEffect : m_TrailEffects)
		Safe_Release(pEffect);

	for (auto& Pair : m_TexObj)
		for (auto& iter : Pair.second)
			Safe_Release(iter);

	for (auto& Pair : m_EffectGroup)
		Safe_Release(Pair.second);

	Safe_Release(m_tSelected_Effect_AttachDesc.pParentTransform);
	Safe_Release(m_tSelected_Effect_AttachDesc.pBone);
	Safe_Release(m_pSelectedEffect);
	Safe_Release(m_pSelectedObj);

	m_ParticleEffects.clear();
	m_MeshEffects.clear();
	m_TrailEffects.clear();

}
